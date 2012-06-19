// $Id: Socket.cpp 7231 2010-08-26 06:05:51Z kyuhun.lee $
#include "StdAfx.h"
#include "Socket.h"
#include "Thread.h"
#include "Containers.h"

#include "UtilityString.h"
#include "UtilityWin32.h"
#include "Log.h"
#include "Application.h"
#include "SecureValue.h"


using namespace J9;

J9::String J9::Socket::sHostName_;

#ifdef _LOG_TRAFFIC_
STATIC_SINGLETON_IMPL(J9::_TrafficStat);
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Packet Class Implementation
//
////////////////////////////////////////////////////////////////////////////////
Packet::Packet(Size i)
	: Buffer(i)
{
	
}

Packet::Packet(const U8 *p, Size i)
	: Buffer(p, i)
{
	
}

Packet::~Packet()
{
	
}

void
Packet::Reset()
{
	Size i = cBuffer_.size();
	Clear();
	Resize(i);
}

void
Packet::Dump(const J9::String& sReason, bool bCut)
{
	StringW	sDump;

	for (Size i = 0; i < iWriteOffset_; ++i)
	{
		if (bCut && i < iReadOffset_) continue;
		sDump += StringUtil::Format(_W("0x%x "), cBuffer_[i]);
	}

	LOG(_W("- Packet::Dump (%s) %d/%d (%s)\r\n"), sReason.c_str(), iReadOffset_, iWriteOffset_, sDump.c_str());
}

Size
Packet::Ekam(const char* pFormat, va_list cArgList)
{
	Size iOffset = iReadOffset_;

	for (int i = 0; pFormat[i]; ++i) {
		switch (pFormat[i]) {
		case '1': *this >> *(va_arg(cArgList, U8*)); break;
		case '2': *this >> *(va_arg(cArgList, U16*)); break;
		case '4': *this >> *(va_arg(cArgList, U32*)); break;
		case '8': *this >> *(va_arg(cArgList, U64*)); break;
		//case 'f': *this >> *(va_arg(cArgList, F64*)); break; // make not implemented
		case 's': *this >> *(va_arg(cArgList, StringA*)); break;
		case 'S': *this >> *(va_arg(cArgList, StringW*)); break;
		default: ASSERT(false); break;
		}
	}

	return iReadOffset_ - iOffset;
}

Size
Packet::Ekam(const char* pFormat, ...)
{
	va_list cArgList;
	va_start(cArgList, pFormat);
	Size iOffset = Ekam(pFormat, cArgList);
	va_end(cArgList);
	return iOffset;
}

Size
Packet::Make(const char* pFormat, va_list cArgList)
{
	Size iOffset = iWriteOffset_;

	for (int i = 0; pFormat[i]; ++i) {
		switch (pFormat[i]) {
		case '1': *this << (va_arg(cArgList, U8)); break;
		case '2': *this << (va_arg(cArgList, U16)); break;
		case '4': *this << (va_arg(cArgList, U32)); break;
		case '8': *this << (va_arg(cArgList, U64)); break;
		//case 'f': *this << (va_arg(cArgList, F64)); break; // takes too much cost for Float
		case 's': *this << J9::StringA(va_arg(cArgList, const char*)); break;
		case 'S': *this << J9::StringW(va_arg(cArgList, const WChar*)); break;
		default: ASSERT(false); break;
		}
	}

	return iWriteOffset_ - iOffset;
}

Size
Packet::Make(const char* pFormat, ...)
{
	va_list cArgList;
	va_start(cArgList, pFormat);
	Size iOffset = Make(pFormat, cArgList);
	va_end(cArgList);
	return iOffset;
}

////////////////////////////////////////////////////////////////////////////////
//
// Socket Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

static const char*	pCryptKey = "JustNineCo.Ltd";
static const Size	iCryptKeyLength = 14;

#ifdef J9_SOL_ENCRYPT_SOCKET_KEY
class SocketKey 
{
public:
	U8	iKeyBuffer_[iCryptKeyLength*4];
};
class SocketKeys
{
public:
	static void		Setup()
	{
		iCurrentSocketKey = 0;
		SocketKey	cKey;
		::memcpy(cKey.iKeyBuffer_ + iCryptKeyLength * 0, pCryptKey, iCryptKeyLength);
		::memcpy(cKey.iKeyBuffer_ + iCryptKeyLength * 1, pCryptKey, iCryptKeyLength);
		::memcpy(cKey.iKeyBuffer_ + iCryptKeyLength * 2, pCryptKey, iCryptKeyLength);
		::memcpy(cKey.iKeyBuffer_ + iCryptKeyLength * 3, pCryptKey, iCryptKeyLength);
		for (Size i = 0; i < iNumSocketKeys; ++i)
		{
			cSocketKeys[i].Set(cKey);
		}
	}
	static void		GetSocketKey(SocketKey& r)
	{
		cSocketKeys[iCurrentSocketKey].Get(r);
		++iCurrentSocketKey;
		iCurrentSocketKey %= iNumSocketKeys;

	}
	static void		TeaseBadGuy(int iBad)
	{
		SocketKey	cBadKey;

		ASSERT(false); // this shouldn't happen to us...

		for (int i = 0; i < countof(cBadKey.iKeyBuffer_); ++i)
		{
			cBadKey.iKeyBuffer_[i] = 'b';
		}
		Size	iBadKey = _sc<Size>(iBad) % iNumSocketKeys;
		cSocketKeys[iBadKey].Set(cBadKey);
	}

private:
	static const Size					iNumSocketKeys = 64;
	static J9::SecureValue<SocketKey>	cSocketKeys[iNumSocketKeys];
	static Size							iCurrentSocketKey;
};
J9::SecureValue<SocketKey>	SocketKeys::cSocketKeys[iNumSocketKeys];
Size						SocketKeys::iCurrentSocketKey = 0;

#endif

static inline void
XOR4(const U8* pSrc, U8* pDst, Size iLen, U32* pKey, Size iKeyLen)
{
    Size iWCount, iCurrent, iRemain;
    U32 iWKey, *pSrcW, *pDstW;
    U8* pSrcb, *pDstb;

	U32 iSrcBuf[J9::Socket::iSocketBufferLength / 4], iDstBuf[J9::Socket::iSocketBufferLength / 4];

    // copy to word pointed buffer
    memcpy((U8*) iSrcBuf, pSrc, iLen);

    pSrcW = iSrcBuf;
    pDstW = iDstBuf;
    iWCount = iLen / sizeof(U32);


	for (iCurrent = 0; iCurrent < iWCount; ++iCurrent)
	{
		*pDstW = *pSrcW ^ pKey[iCurrent % iKeyLen];
		++pSrcW; ++pDstW;
    }

    pSrcb = (U8*) pSrcW;
    pDstb = (U8*) pDstW;
    iWKey = pKey[iCurrent % iKeyLen];
    iRemain = iLen % sizeof(U32);

#ifdef __BIG_ENDIAN__
    switch (iRemain)
	{
    case 1:
		*(pDstb + 0) = *(pSrcb + 0) ^ (U8) ((iWKey & 0xff000000) >> 24);
		break;
    case 2:
		*(pDstb + 0) = *(pSrcb + 0) ^ (U8) ((iWKey & 0xff000000) >> 24);
		*(pDstb + 1) = *(pSrcb + 1) ^ (U8) ((iWKey & 0x00ff0000) >> 16);
		break;
    case 3:
		*(pDstb + 0) = *(pSrcb + 0) ^ (U8) ((iWKey & 0xff000000) >> 24);
		*(pDstb + 1) = *(pSrcb + 1) ^ (U8) ((iWKey & 0x00ff0000) >> 16);
		*(pDstb + 2) = *(pSrcb + 2) ^ (U8) ((iWKey & 0x0000ff00) >> 8);
		break;
    default:
		break;
    }
#else
    switch (iRemain)
	{
	case 1:
		*(pDstb + 0) = *(pSrcb + 0) ^ (U8) ((iWKey & 0x000000ff) >> 0);
		break;
    case 2:
		*(pDstb + 0) = *(pSrcb + 0) ^ (U8) ((iWKey & 0x000000ff) >> 0);
		*(pDstb + 1) = *(pSrcb + 1) ^ (U8) ((iWKey & 0x0000ff00) >> 8);
		break;
    case 3:
		*(pDstb + 0) = *(pSrcb + 0) ^ (U8) ((iWKey & 0x000000ff) >> 0);
		*(pDstb + 1) = *(pSrcb + 1) ^ (U8) ((iWKey & 0x0000ff00) >> 8);
		*(pDstb + 2) = *(pSrcb + 2) ^ (U8) ((iWKey & 0x00ff0000) >> 16);
		break;
    default:
		break;
    }
#endif

    // copy dstination buffer to original destination pointer

    memcpy(pDst, (U8*) iDstBuf, iLen);
}

void
Socket::Setup()
{
	static bool bInitialized = false;
    if (bInitialized) return; else bInitialized = true;

	// WinSock API specific
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	//if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		LOG(_W("! windows socket initialization failed\r\n"));
		ThreadModel::SignalShutdown();
	}

	char sHostName[128] = "";

	::gethostname(sHostName, sizeof(sHostName) - 1);
	sHostName_ = J9::StringUtil::ToStringW(sHostName, sizeof(sHostName) - 1);

#ifdef J9_SOL_ENCRYPT_SOCKET_KEY
	SocketKeys::Setup();
#else
	Size	iKeySize = sizeof(U32) * iCryptKeyLength + 1;	
	pSocketKey = (U32*) malloc(iKeySize);
	sprintf_s((char*) pSocketKey, iKeySize, "%s%s%s%s", pCryptKey, pCryptKey, pCryptKey, pCryptKey);
#endif
}

void
Socket::Shutdown()
{
	static bool bShutdown= false;
    if (bShutdown) return; else bShutdown = true;	

	if (WSACleanup() != 0)
	{
		LOG(_W("! winsock clean up failed\r\n"));
	}

#ifndef J9_SOL_ENCRYPT_SOCKET_KEY
	free(pSocketKey);
#endif
}

void
Socket::Close()
{
	LOCK_SCOPE(&cLock_);
	if (iFd_ > 0)
	{
		Send();	

		int	iRet = SOCKET_shutdown(iFd_, SD_BOTH);
		if (iRet != 0)
			LOG(_W(" ! socket shutdown err: error code %d\r\n"), WSAGetLastError());
	}

    if (iFd_ > 0 && SOCKET_close(iFd_) < 0)
		Win32::LogError(errno, _W(" ! socket close err: %s\r\n"));

    iFd_ = 0;	
	Reset();
}

Socket::Socket(SOCKET_TYPE iFd)
	: pSndBuf_(0), pSndPtr_(0), bBlock_(true)
{
	if (iFd > 0) SetFD(iFd);
	else iFd_ = 0;

	pSockInfo_ = NULL;
	iRcvSeq_ = 0;
	iSndSeq_ = 0;
	iPrevSeq_ = 0;
	iErrCnt_ = 0;

	pSndPtr_ = pSndBuf_ = _sc<U8*>(malloc(Socket::iSocketBufferLength));
	pRcvPtr_ = pRcvEnd_ = pRcvBuf_ = _sc<U8*>(malloc(Socket::iSocketBufferLength));	
#ifdef J9_SOL_USE_XTRAP
	iEncryptType_ = eEncryptNone;
#endif
}

Socket::~Socket()
{
	Close();
	
	if (pSndBuf_) free(pSndBuf_);
	if (pRcvBuf_) free(pRcvBuf_);
	if (pSockInfo_) delete pSockInfo_;	
}

void
Socket::SetFD(SOCKET_TYPE iFd, bool bBlock)
{
	iFd_ = iFd;

	if (SOCKET_setsockopt(iFd_, SOL_SOCKET, SO_SNDBUF, (char*) &Socket::iSocketBufferLength, sizeof(Socket::iSocketBufferLength)) < 0)
	{
		Win32::LogError(errno, _W("set sndbuf size err [%s]\r\n"));
	}

    if (SOCKET_setsockopt(iFd_, SOL_SOCKET, SO_RCVBUF, (char*) &Socket::iSocketBufferLength, sizeof(Socket::iSocketBufferLength)) < 0)
	{
		Win32::LogError(errno, _W("set rcvbuf size err [%s]\r\n"));
	}

    if (bBlock) return;
	else bBlock_ = false;

	if (!SOCKET_setblocking(iFd_, false))
		Win32::LogError(errno, _W("socket nonblock err %s\r\n"));

#ifdef J9_SOL_USE_XTRAP
	SetEncryption(eEncryptNone);
#endif
}

void
Socket::SetSocketInfo(J9::SOCKETINFO* pSockInfo)
{
	LOCK_SCOPE(&cLock_);
	if (pSockInfo_) delete pSockInfo_;	
	pSockInfo_ = pSockInfo;
	pRcvPtr_ = pRcvEnd_ = pRcvBuf_;
}

void
Socket::Reset()
{	
	LOCK_SCOPE(&cLock_);

	pSndPtr_ = pSndBuf_;
	pRcvPtr_ = pRcvEnd_ = pRcvBuf_;

	iSndSeq_ = 0;
	iPrevSeq_ = 0;
	iRcvSeq_ = 0;
	//LOG(_W("socket reset\r\n"));
}

bool
Socket::Send()
{
	LOCK_SCOPE(&cLock_);

	if (iFd_ <= 0)
	{
		LOG(_W("! socket write error (wrong FD:%d)\r\n"), iFd_);
		return false;
	}

	S32 iSendSize = (S32) (pSndPtr_ - pSndBuf_);

	if (iSendSize <= 0) return true;

	//if (pSockInfo_) pSockInfo_->cState_ = J9::IOCPSocketState_Send;
	S32 iSentSize = SOCKET_write(iFd_, pSndBuf_, iSendSize);

	if (iSentSize > 0)
	{
		if (iSentSize != iSendSize)
			memmove(pSndBuf_, pSndBuf_ + iSentSize, iSendSize - iSentSize);
		pSndPtr_ -= iSentSize;
		iErrCnt_ = 0;
		return true;
	}

	if (iSentSize == SOCKET_ERROR)
	{
		int		iErr = WSAGetLastError();
		LOG(_W("! [-1:%d] socket write error [Socket:%d, ErrorCount:%d]\r\n"), iErr, iFd_, ++iErrCnt_);
		if (iErr == WSAEWOULDBLOCK) return true; // this can happen in normal situation, let's ignore
		if (iErrCnt_ >=3)
		{
			// 3rd attempt faild, let's dump current packet
			pSndPtr_ = pSndBuf_;
			if (iErrCnt_ >= 20)
			{
				// enough error to close
				int	iRet = SOCKET_shutdown(iFd_, SD_BOTH);
				if (iRet != 0)
					LOG(_W(" ! socket close err1: error code %d\r\n"), WSAGetLastError());
			    if (iFd_ > 0 && SOCKET_close(iFd_) < 0)
					LOG(_W(" ! socket close err2: error code %d\r\n"), WSAGetLastError());
				iFd_ = 0;
				return false;
			}
		}
		return true;
	}

	if (iSentSize == 0)
	{
		int		iErr = WSAGetLastError();
		LOG(_W("! [0:%d] socket write error [Socket:%d, ErrorCount:%d]\r\n"), iErr, iFd_, iErrCnt_);
		return false;
	}

	ASSERT(false);
	return false;
}

void
Socket::Decrypt(const U8* pSrc, U8* pDst, Size iLen)
{
#ifdef J9_SOL_USE_XTRAP
	switch (iEncryptType_)
	{
	case eEncryptNone:
		{
			XOR4(pSrc, pDst, iLen, pSocketKey, iCryptKeyLength);
		}
		break;
	case eEncryptXTrap:
		{
			memcpy(pDst, pSrc, iLen);
			XTrap_SDK1_Decrypt_Buf(pDst, iLen, cPerSDKData_.pKey, cPerSDKData_.nKeySize, cPerSDKData_.iEncryptAlgorithm, cPerSDKData_.cMode);
		}
		break;
	default:
		ASSERT(false);
	}
#elif defined(J9_SOL_ENCRYPT_SOCKET_KEY)
	SocketKey	cKey;
	SocketKeys::GetSocketKey(cKey);
	XOR4(pSrc, pDst, iLen, _rc<U32*>(cKey.iKeyBuffer_), iCryptKeyLength);
#else
	XOR4(pSrc, pDst, iLen, pSocketKey, iCryptKeyLength);
#endif
}

void
Socket::Encrypt(const U8* pSrc, U8* pDst, Size iLen)
{	
#ifdef J9_SOL_USE_XTRAP
	switch (iEncryptType_)
	{
	case eEncryptNone:
		{
			XOR4(pSrc, pDst, iLen, pSocketKey, iCryptKeyLength);
		}
		break;
	case eEncryptXTrap:
		{
			memcpy(pDst, pSrc, iLen);
			XTrap_SDK1_Encrypt_Buf(pDst, iLen, cPerSDKData_.pKey, cPerSDKData_.nKeySize, cPerSDKData_.iEncryptAlgorithm, cPerSDKData_.cMode);
		}
		break;
	}
#elif defined(J9_SOL_ENCRYPT_SOCKET_KEY)
	SocketKey	cKey;
	SocketKeys::GetSocketKey(cKey);
	XOR4(pSrc, pDst, iLen, _rc<U32*>(cKey.iKeyBuffer_), iCryptKeyLength);
#else
	XOR4(pSrc, pDst, iLen, pSocketKey, iCryptKeyLength);
#endif
}

bool
Socket::Write(J9::Packet* p)
{
	return Write(p->GetPointer(), p->GetWriteOffset());
}

bool
Socket::Write(const U8* pPacket, Size iLen)
{	
	LOCK_SCOPE(&cLock_);

	Size iCurSize = pSndPtr_ - pSndBuf_;
	if (iCurSize + iLen + iHeaderLength > iSocketBufferLength)
	{
		LOG(_W("! [%d] packet overflow (len: %d)\r\n"), pPacket[0], iLen);
		if (!Send()) 
		{
			LOG(_W("0 : Socket SendFailed\r\n"));
			return false;
		}
		iCurSize = pSndPtr_ - pSndBuf_;
		if (iCurSize + iLen + iHeaderLength > iSocketBufferLength)
		{
			LOG(_W("1 :Socket SendFailed (iCurSize + iLen + iHeaderLength > iSocketBufferLength)\r\n"));
			return false;
		}
	}
	Size iOffset = 0;
	*pSndPtr_ = *GetSocketHeader();
	iOffset += 1;
	
	//////////////////////////////////////////////////////////////////////////
	// add information about encryption stuff (encryption type with 3 more bytes of padding)	
	//U32 iHeaderInfo = iEncryptType_ << 3 * sizeof(U8) | 0 << 2 * sizeof(U8) | 0 << 1 * sizeof(U8) | 0;
	//memcpy(pSndPtr_ + iOffset, &(iHeaderInfo), sizeof(iHeaderInfo));
	iOffset += WriteCustomHeader(pSndPtr_ + iOffset);
	//////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////
	// sequence part, maybe should be moved to encryption part?	
	memcpy(pSndPtr_ + iOffset, &(++iSndSeq_), sizeof(iSndSeq_));	
	iOffset += sizeof(iSndSeq_);	
	///////////////////////////////////////////////////////////////

#define iLenSize 2
	memcpy(pSndPtr_ + iOffset, &iLen, iLenSize);
	iOffset += iLenSize;

	// encrypt the packet here
	U8 cBuf[iSocketBufferLength * 2];
	
	Encrypt(pPacket, cBuf, iLen);

	// then write it to socket buffer
	memcpy(pSndPtr_ + iOffset, cBuf, iLen);
	pSndPtr_ += iOffset + iLen;

	return true;
}

bool
Socket::Receive()
{
	LOCK_SCOPE(&cLock_);

	Size iRemain = (pRcvEnd_ - pRcvPtr_);
	S32 iToRead = (S32) (Socket::iSocketBufferLength - iRemain);
	if (pRcvPtr_ != pRcvBuf_)
	{
		memmove(pRcvBuf_, pRcvPtr_, iRemain);
		pRcvPtr_ = pRcvBuf_;
		pRcvEnd_ = pRcvBuf_ + iRemain;
	}

	S32 iBytes = SOCKET_read(iFd_, pRcvEnd_, iToRead);

	if (iBytes > 0) return pRcvEnd_ += iBytes, true;

	if (iBytes == 0) 
		LOG(_W("! socket connection closed {%d}\r\n"), iFd_);

	// nothing read or error has been occurred, return error
	if (iBytes == SOCKET_ERROR)
	{
		if (SOCKET_getlasterror() == SOCKERR_WOULDBLOCK) return !bBlock_;
		else LOG(_W("! socket connection error {%d} error code {%d}\r\n"), iFd_, WSAGetLastError());
	}

	return false;
}

J9::Ptr<Packet>
Socket::Read()
{
	#define iLenSize 2
	LOCK_SCOPE(&cLock_);
	ASSERT(pRcvPtr_ <= pRcvEnd_);
	Size iAvail = (pRcvEnd_ - pRcvPtr_);

	if (iAvail < iHeaderLength + iLenSize) return NULL;

	Size iOffset = 0;
	if (!CheckHeader(*pRcvPtr_)) 
	{
		LOG(_W("Header check failed\r\n"));
		return NULL;
	}
	iOffset += 1;

	//////////////////////////////////////////////////////////////////////////
	// read the custom headers
	iOffset += ReadCustomHeader(pRcvPtr_ + iOffset);
	//////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////
	// sequence part, maybe should be moved to encryption part?
	U16 iPrevSeq, iRcvSeq;
	iPrevSeq = iRcvSeq_;
	memcpy(&iRcvSeq, pRcvPtr_ + iOffset, sizeof(iRcvSeq));
	iOffset += sizeof(iRcvSeq);
	if (_sc<U16>(iPrevSeq + 1) != _sc<U16>(iRcvSeq))
	{
		LOG(_W(" !!! socket sequence mismatch %d => %d\r\n"), iPrevSeq, iRcvSeq);
//		ASSERT(0);
	}
	//
	///////////////////////////////////////////////////////////////	
	Size iLen = 0;
	memcpy(&iLen, pRcvPtr_ + iOffset, iLenSize);
	iOffset += iLenSize;

	if (_sc<S32>(iLen) > _sc<S32>(iAvail) - _sc<S32>(iOffset))  return NULL;

	iPrevSeq_ = iPrevSeq;
	iRcvSeq_ = iRcvSeq;
	// decrypt packet here
	U8 cBuf[iSocketBufferLength * 2];

	Decrypt(pRcvPtr_ + iOffset, cBuf,iLen);
	// then make a new packet and return
	Ptr<Packet> p = Ptr<Packet>(ALLOC, cBuf, iLen);
	pRcvPtr_ += iOffset + iLen;
	ASSERT(pRcvPtr_ <= pRcvEnd_);
	return p;
}

bool
Socket::TryRead()
{	
	if (iFd_ <= 0) return false;
	LOCK_SCOPE(&cLock_);
	ASSERT(pSockInfo_);
	static U32 iAddr = sizeof(pSockInfo_->cAddr_);
	U32 iRecvBytes, iFlags = 0;

	Size iRemain = (pRcvEnd_ - pRcvPtr_);
	
	if (iRemain > 0)
		memmove(pRcvBuf_, pRcvPtr_, iRemain);
	else
		::ZeroMemory(&(pSockInfo_->cOverlapped_), sizeof(pSockInfo_->cOverlapped_));

	pSockInfo_->cWSABuf_.buf = (CHAR*) (pRcvBuf_ + iRemain);
	pSockInfo_->cWSABuf_.len = J9::Socket::iSocketBufferLength - iRemain;
	pSockInfo_->cOverlapped_.OffsetHigh = iRemain;
	pRcvPtr_ = pRcvBuf_;
	
	pSockInfo_->cState_ = J9::IOCPSocketState_Recv;
	U32 iRet = WSARecv(pSockInfo_->cSock_, &(pSockInfo_->cWSABuf_), 1, &iRecvBytes, &iFlags, &(pSockInfo_->cOverlapped_), NULL);
	//LOG(_W(" ! socket try reading %d = %d\r\n"), iRet, WSAGetLastError() == ERROR_IO_PENDING);
	if (iRet == SOCKET_ERROR) return false;

	return true;
}

bool
Socket::ReceiveIOCP()
{	
	LOCK_SCOPE(&cLock_);

	if (!iFd_) return false;
	ASSERT(pRcvPtr_ - pRcvBuf_ + pSockInfo_->cOverlapped_.InternalHigh + pSockInfo_->cOverlapped_.OffsetHigh <= J9::Socket::iSocketBufferLength);
	pRcvEnd_ = pRcvPtr_ + pSockInfo_->cOverlapped_.InternalHigh + pSockInfo_->cOverlapped_.OffsetHigh;
	ASSERT(pRcvPtr_ <= pRcvEnd_);
	return true;
}

J9::Ptr<Packet>
Socket::ReadIOCP()
{	
	#define iLenSize 2
	LOCK_SCOPE(&cLock_);	

	ASSERT(pRcvPtr_ <= pRcvEnd_);
	Size iAvail = (pRcvEnd_ - pRcvPtr_);
	Size iOffset = 0;

	if (iAvail < iHeaderLength + iLenSize) return NULL;
	if (*pRcvPtr_ != *GetSocketHeader())
	{
		LOG(_W(" ! Socket header mismatch [%c]\r\n"), *pRcvPtr_);
		this->Close();
		return NULL;
	}
	
	iOffset += 1;

	//////////////////////////////////////////////////////////////////////////
	// read the custom headers
	iOffset += ReadCustomHeader(pRcvPtr_ + iOffset);
	//////////////////////////////////////////////////////////////////////////

	U16 iPrevSeq, iRcvSeq;
	///////////////////////////////////////////////////////////////
	// sequence part, maybe should be moved to encryption part?
	iPrevSeq = iRcvSeq_;
	memcpy(&iRcvSeq, pRcvPtr_ + iOffset, sizeof(iRcvSeq));
	iOffset += sizeof(iRcvSeq);

	if (_sc<U16>(iPrevSeq + 1) != _sc<U16>(iRcvSeq))
	{
		LOG(_W(" ! Socket sequence mismatch prev = %d, rcv = %d\r\n"), iPrevSeq, iRcvSeq);
		this->Close();
		return NULL;
		//ASSERT(false);
	}
	//
	///////////////////////////////////////////////////////////////	
	Size iLen = 0;
	memcpy(&iLen, pRcvPtr_ + iOffset, iLenSize);
	iOffset += iLenSize;

	if (iLen > iAvail - iOffset) return NULL;

	iPrevSeq = iPrevSeq_;
	iRcvSeq_ = iRcvSeq;
	// decrypt packet here
	U8 cBuf[iSocketBufferLength * 2];
	Decrypt(pRcvPtr_ + iOffset, cBuf, iLen);

	// then make a new packet and return
	Ptr<Packet> p = Ptr<Packet>(ALLOC, cBuf, iLen);
	pRcvPtr_ += iOffset + iLen;
	ASSERT(pRcvPtr_ <= pRcvEnd_);
	return p;
}

SOCKET_TYPE
Socket::TCP_Listen(U32 iAddr, U16 iPort)
{
	static S32 iOne = 1;
    SOCKET_TYPE iFd;
    SOCKADDR_IN cAddr;

    if (iFd = SOCKET_tcp(), iFd < 0)
	{
		Win32::LogError(errno, _W("! socket() err : %s\r\n"));
		ThreadModel::SignalShutdown();
	}

    if (SOCKET_setsockopt(iFd, SOL_SOCKET, SO_SNDBUF, (char*) &Socket::iSocketBufferLength, sizeof(Socket::iSocketBufferLength)) < 0)
	{
		Win32::LogError(errno, _W("! setsockopt() sendbuf size err : %s\r\n"));
		return ThreadModel::SignalShutdown(), 0;
	}

	// windows 2000 or higher and winsock2 가 아닌 경우 so_reuseaddr 옵션을 사용해서 time_wait bind error 문제를 해결한다.
	if (SOCKET_setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, (char*) &iOne, sizeof(iOne)) < 0)
	{
		Win32::LogError(errno, _W("! setsockopt() sendbuf size err : %s\r\n"));
		exit(0);
	}

	// winsock2 and windows 2000 or higher only
	//if (SOCKET_setsockopt(iFd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*) &iOne, sizeof(iOne)) < 0)
	//{
	//	LOG(_W("! setsockopt() exclusive address use err : %S\r\n"), strerror(errno));
	//	exit(0);
	//}

	UNUSED(iAddr);
	cAddr.sin_family = AF_INET;
	//cAddr.sin_addr.s_addr = iAddr;
	cAddr.sin_addr.s_addr = INADDR_ANY;
    cAddr.sin_port = htons(iPort);

	if (SOCKET_bind(iFd, (sockaddr *) &cAddr, sizeof(cAddr)) == SOCKET_ERROR)
	{
		LOG(_W("! socket binding error, port %d already in use\r\n"), iPort);
		exit(0);
	}

	if (SOCKET_listen(iFd, Socket::iDefaultPendingConnections) == SOCKET_ERROR)
	{
		LOG(_W("! socket listenning error, port %d already in use\r\n"), iPort);
		exit(0);
	}

	LOG(_W("! tcp socket {%d} binding port %d\r\n"), iFd, iPort);

	return iFd;
}

SOCKET_TYPE
Socket::TCP_Listen(const AChar* pAddr, U16 iPort)
{
	return TCP_Listen(inet_addr(pAddr), iPort);
}

SOCKET_TYPE
Socket::TCP_Connect(U32 iAddr, U16 iPort)
{
	SOCKET_TYPE	iFd;
	SOCKADDR_IN cAddr;

	cAddr.sin_family = AF_INET;
    cAddr.sin_port = htons(iPort);
    cAddr.sin_addr.s_addr = iAddr;

	if (iFd = SOCKET_tcp(), iFd <= 0)
	{
		Win32::LogError(errno, _W("! tcp socket creation err %s\r\n"));
		return 0;
	} 
	else if (SOCKET_connect(iFd, (SOCKADDR *) &cAddr, sizeof(cAddr)) == SOCKET_ERROR)
	{
		Win32::LogError(errno, _W("! tcp socket connect err %s\r\n"));
		SOCKET_close(iFd);
		return 0;
	}

	LOG(_W("* connection established to %S:%d {%d}\r\n"), inet_ntoa(cAddr.sin_addr), iPort, iFd);

	return iFd;
}

SOCKET_TYPE
Socket::TCP_Connect(const char* pAddr, U16 iPort)
{
	U32 iAddr = inet_addr(pAddr);
	hostent* pHostEntry = ::gethostbyname(pAddr);
	if (pHostEntry != NULL)
	{
		iAddr = *(u_long *) pHostEntry->h_addr_list[0];
	}
	
	return TCP_Connect(iAddr, iPort);
}


#ifdef _LOG_TRAFFIC_

_TrafficStat::_TrafficStat()
{

}

void
_TrafficStat::Reset()
{
	cSndTraffic_.Clear();
	cRcvTraffic_.Clear();
}

void
_TrafficStat::AddData(U8 iType, bool bIsSending, J9::Packet *pPacket)
{
	/*
	UNUSED(iType);
	UNUSED(bIsSending);
	UNUSED(pPacket);
	*/
	if (iType != GetStatType()) return;

	U8 iPacketType = *pPacket->GetPointer();
	U8 iSize = (U8)pPacket->GetWriteOffset();
	TrafficData* cTempData = new TrafficData;
	if (bIsSending)
	{
		if (cSndTraffic_.Has(iPacketType)) 
		{
			cTempData = cSndTraffic_.Get(iPacketType);
			cTempData->iCount_++;
			cTempData->iSize_ += iSize;
		}
		else
		{
			cTempData->iCount_ = 1;
			cTempData->iType_ = iPacketType;
			cTempData->iSize_ = iSize;
			cSndTraffic_.Put(iPacketType, cTempData);
		}
	}
	else
	{
		if (cRcvTraffic_.Has(iPacketType)) 
		{
			cTempData = cRcvTraffic_.Get(iPacketType);
			cTempData->iCount_++;
			cTempData->iSize_ += iSize;
		}
		else
		{
			cTempData->iCount_ = 1;
			cTempData->iType_ = iPacketType;
			cTempData->iSize_ = iSize;
			cRcvTraffic_.Put(iPacketType, cTempData);
		}
	}
}

void _TrafficStat::PrintTraffic()
{
	LOG(_W("===================Statistics Sending=========================\r\n"));
	{
		J9::IndexPool < TrafficData* >::Iterator cIter, cEnd =  cSndTraffic_.End();
		for (cIter = cSndTraffic_.Begin(); cIter != cEnd; ++cIter)
		{
			TrafficData* cTempData = cIter->second;
			LOG(_W("==============================================================\r\n"));
			LOG(_W("==Packet Type:          %d====================================\r\n"), cTempData->iType_);
			LOG(_W("==Sent Total            %d====================================\r\n"), cTempData->iCount_);
			LOG(_W("==Sent Size             %d====================================\r\n"), cTempData->iSize_);
			LOG(_W("==============================================================\r\n"));
		}
	}
	
	LOG(_W("\r\n\r\n===================Statistics Receiving=======================\r\n"));
	{
		J9::IndexPool < TrafficData* >::Iterator cIter, cEnd =  cRcvTraffic_.End();
		for (cIter = cRcvTraffic_.Begin(); cIter != cEnd; ++cIter)
		{
			TrafficData* cTempData = cIter->second;
			LOG(_W("==============================================================\r\n"));
			LOG(_W("==Packet Type:          %d====================================\r\n"), cTempData->iType_);	
			LOG(_W("==Received Total        %d====================================\r\n"), cTempData->iCount_);
			LOG(_W("==Received SIze         %d====================================\r\n"), cTempData->iSize_);
			LOG(_W("==============================================================\r\n"));
		}
	}
	LOG(_W("== Reseting Traffic Log==\r\n"));
	Reset();
}
#endif

#ifdef J9_SOL_USE_XTRAP
#define SIZEOFBYTEINBITS	8
Size
Socket::WriteCustomHeader(U8* pBuf)
{
	U32 iHeaderInfo = (iEncryptType_ != eEncryptNone) << 3 * SIZEOFBYTEINBITS | iEncryptType_ << 2 * SIZEOFBYTEINBITS | 0 << 1 * SIZEOFBYTEINBITS | 0;
	memcpy(pBuf, &(iHeaderInfo), sizeof(iHeaderInfo));
	return sizeof(iHeaderInfo);
}

U32
Socket::ReadCustomHeader(const U8* pBuf)
{
	U32 iHeaderInfo;
	memcpy(&iHeaderInfo, pBuf, sizeof(iHeaderInfo));
	U8	bIsEncrypted = (U8)(iHeaderInfo >> 3 * SIZEOFBYTEINBITS);
	U8	iEncryptType = (U8)(iHeaderInfo >> 2 * SIZEOFBYTEINBITS);
	// Encryption Check.. make the socket cut if this is not right
	if (bIsEncrypted && iEncryptType != iEncryptType_) 
	{
	//	Close();	// encryption이 틀렸네? 짤라버려~ 근데 걍 이렇게 짤라도 되나?
		ASSERT(false);
		LOG(_W("encryption error!!!\r\n "));
	}
	return sizeof(iHeaderInfo);
}
#else//J9_SOL_USE_XTRAP
#define SIZEOFBYTEINBITS	8
Size
Socket::WriteCustomHeader(U8* pBuf)
{
	U32 iHeaderInfo = 0;
	memcpy(pBuf, &(iHeaderInfo), sizeof(iHeaderInfo));
	return sizeof(iHeaderInfo);
}

U32
Socket::ReadCustomHeader(const U8* pBuf)
{
	U32 iHeaderInfo;
	memcpy(&iHeaderInfo, pBuf, sizeof(iHeaderInfo));
	// Encryption Check.. make the socket cut if this is not right
	if (iHeaderInfo != 0)
	{
		//	Close();	// encryption이 틀렸네? 짤라버려~ 근데 걍 이렇게 짤라도 되나?
		LOG(_W("encryption error!!!\r\n "));
	}
	return sizeof(iHeaderInfo);
}
#endif//J9_SOL_USE_XTRAP

#ifdef J9_SOL_USE_XTRAP
U32
Socket::SetEncryption(U8 iType, unsigned long *pKey)
{
	iEncryptType_ = iType;
	cPerSDKData_.iEncryptAlgorithm = ARIA;
	cPerSDKData_.cMode = MODE_CTR;
	cPerSDKData_.nKeySize = XTrap_SDK1_Make_Key(cPerSDKData_.pKey);
	memcpy(pKey, cPerSDKData_.pKey, cPerSDKData_.nKeySize);
	return cPerSDKData_.nKeySize;
}

void
Socket::SetEncryptionkey(U8 iType, unsigned long *pKey, U32 iKeySize)
{
	iEncryptType_ = iType;
	cPerSDKData_.iEncryptAlgorithm = ARIA;
	cPerSDKData_.cMode = MODE_CTR;
	//			cPerSDKData_.pKey = pKey;
	memcpy(cPerSDKData_.pKey, pKey, iKeySize);
	cPerSDKData_.nKeySize = iKeySize;
}

bool
Socket::NeedEncryption(const U8* pSrc)
{
	//	P_Transfer				= 14,
	//	P_Version				= 1,
	if (pSrc[0] == 14) return false;
	if (pSrc[0] == 1) return false;
	return true;
}
#endif

#ifdef J9_SOL_ENCRYPT_SOCKET_KEY
void
J9::Socket::TeaseBadGuy(int iBad)
{
	SocketKeys::TeaseBadGuy(iBad);
}
#endif