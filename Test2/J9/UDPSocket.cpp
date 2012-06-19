// $Id: UDPSocket.cpp 3966 2009-09-25 09:59:18Z hyojin.lee $

#include "Stdafx.h"
#include "UDPSocket.h"

#include "Thread.h"
#include "Log.h"
#include "UtilityString.h"
#include "UtilityWin32.h"

using namespace J9;

#pragma pack(push, 1)
struct UDPHeader
{
	U8	iHeader_;
	U16	iLength_;
};
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
// UDPSocket
////////////////////////////////////////////////////////////////////////////////
SOCKET_TYPE
UDPSocket::UDP_Bind(const SOCKADDR_IN* pAddr, S32 iAddrLen)
{
	static S32 iOne = 1;

	SOCKET_TYPE iFd = SOCKET_udp();

	if (iFd == INVALID_SOCKET )
	{
		Win32::LogError(errno, _W("! udp socket creation err %s\r\n"));
		return ThreadModel::SignalShutdown(), 0;
	}

	// winsock2 and windows 2000 or higher only
	if (SOCKET_setsockopt(iFd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*) &iOne, sizeof(iOne)) != 0)
	{
		Win32::LogError(errno, _W("! setsockopt() exclusive address use err : %s\r\n"));
		return ThreadModel::SignalShutdown(), 0;
	}

	if (SOCKET_setblocking(iFd, false) == false)
	{
		SOCKET_close(iFd);
		Win32::LogError(errno, _W("! udp socket blocking err %s\r\n"));
		return ThreadModel::SignalShutdown(), 0;
	}

	if (SOCKET_bind(iFd, (SOCKADDR *) pAddr, iAddrLen) == SOCKET_ERROR)
	{
		SOCKET_close(iFd);
		int	iErrorNo = WSAGetLastError();
		Win32::LogError(iErrorNo, _W("! udp socket binding err %s\r\n"));
		return ThreadModel::SignalShutdown(), 0;
	}

	LOG(_W("! udp socket {%d} binding addr = %S, port %d\r\n"), iFd, inet_ntoa(pAddr->sin_addr), ntohs(pAddr->sin_port));

	return iFd;
}

void
UDPSocket::Setup()
{
	static bool bInitialized = false;
    if (bInitialized) return; else bInitialized = true;

	Socket::Setup();
}

void
UDPSocket::Close()
{
	if (iFd_ > 0 && SOCKET_close(iFd_) < 0)
	{
		Win32::LogError(errno, _W(" ! socket close err: %s\r\n"));
	}

    iFd_ = 0;
}

UDPSocket::UDPSocket(SOCKET_TYPE iFd)
	: bBlock_(true)
{
	if (iFd > 0) SetFD(iFd);
	else iFd_ = 0;

	pRcvPtr_ = pRcvEnd_ = pRcvBuf_ = _sc<U8*>(malloc(Socket::iSocketBufferLength));
}

UDPSocket::~UDPSocket()
{
	Close();
	free(pRcvBuf_);
}

void
UDPSocket::SetFD(SOCKET_TYPE iFd, bool bBlock)
{
	iFd_ = iFd;

	if (SOCKET_setsockopt(iFd_, SOL_SOCKET, SO_SNDBUF, (char*) &Socket::iSocketBufferLength, sizeof(Socket::iSocketBufferLength)) < 0)
	{
		Win32::LogError(errno, _W("set sndbuf size err : %s\r\n"));
	}

    if (SOCKET_setsockopt(iFd_, SOL_SOCKET, SO_RCVBUF, (char*) &Socket::iSocketBufferLength, sizeof(Socket::iSocketBufferLength)) < 0)
	{
		Win32::LogError(errno, _W("set rcvbuf size err : %s\r\n"));
	}

    if (bBlock) return;
	else bBlock_ = false;

	if (!SOCKET_setblocking(iFd_, false))
	{
		Win32::LogError(errno, _W("socket nonblock err %s\r\n"));
	}
}

void
UDPSocket::Reset()
{
	LOCK_SCOPE(&cLock_);

	pRcvPtr_ = pRcvEnd_ = pRcvBuf_;
}

J9::Ptr<J9::UDPPacket>
UDPSocket::Read(U32* pPeerID)
{
	LOCK_SCOPE(&cLock_);
	ASSERT(pRcvPtr_ <= pRcvEnd_);

	Size iAvail = (pRcvEnd_ - pRcvPtr_);

	if (iAvail < J9::UDPSocket::iUDPHeaderLength) return NULL;

	ASSERT(iAvail < J9::Socket::iSocketBufferLength);
	ASSERT(iAvail >= J9::UDPSocket::iUDPHeaderLength);

	U32 iOffset = 0;
	UDPHeader	cHeader;

	::memcpy(&cHeader, pRcvPtr_ + iOffset, sizeof(cHeader));	iOffset += sizeof(cHeader);

	if (cHeader.iHeader_ != iUDPSocketHeader)					{ ASSERT(0); return NULL; }
	if (cHeader.iLength_ > iAvail - sizeof(cHeader))			{ ASSERT(0); return NULL; }

	// cHeader.iLength_ 에 Peer ID 의 4바이트만큼이 들어가 있다.
	::memcpy(pPeerID, pRcvPtr_ + iOffset, sizeof(*pPeerID));	 iOffset += sizeof(*pPeerID);

	//J9::Ptr<J9::UDPPacket> pPacket = J9::UDPPacket::Create(pRcvPtr_ + iOffset, cHeader.iLength_ - sizeof(*pPeerID));
	J9::Ptr<J9::UDPPacket> pPacket(ALLOC, pRcvPtr_ + iOffset, cHeader.iLength_ - sizeof(*pPeerID));

	pRcvPtr_ += iOffset + cHeader.iLength_ - sizeof(*pPeerID);

	return pPacket;
}

bool
UDPSocket::Receive(SOCKADDR* pAddr, S32* pSize)
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
	S32 iBytes = SOCKET_readfrom(iFd_, pRcvEnd_, iToRead, pAddr, pSize);
	if (iBytes > 0)
	{
		pRcvEnd_ += iBytes;
		return true;
	}

	//if (iBytes <= 0) return false;

	return false;
}

///////////////////////////////////////////
//
// UDPSocket read buffer 관련 서버 전용 함수들이다.
//

bool
UDPSocket::Receive(J9::Buffer& rBuffer, SOCKADDR* pAddr, S32* pSize)
{
	rBuffer.Clear();

	// read from socket
	U8	cReceived[J9::Socket::iSocketBufferLength];
	S32 iReceived = SOCKET_readfrom(iFd_, cReceived, J9::Socket::iSocketBufferLength, pAddr, pSize);

	// check error
	if (iReceived <= 0)	return false;

	// initialize to read
	ASSERT(iReceived < J9::Socket::iSocketBufferLength);

	// create buffer
	rBuffer.Resize(iReceived);
	rBuffer.Write(iReceived, cReceived);
	return true;
}

J9::Ptr<J9::UDPPacket>
UDPSocket::Read(J9::Buffer* pBuffer, U32* pPeerID)
{
	ASSERT(pBuffer->GetReadOffset() <= pBuffer->GetWriteOffset());

	Size iAvail = (pBuffer->GetWriteOffset() - pBuffer->GetReadOffset());

	if (iAvail < J9::UDPSocket::iUDPHeaderLength) return NULL;

	ASSERT(iAvail < J9::Socket::iSocketBufferLength);

	UDPHeader	cHeader;
	pBuffer->Read(sizeof(cHeader), &cHeader);

	if (cHeader.iHeader_ != iUDPSocketHeader)					{ ASSERT(0); return NULL; }
	if (cHeader.iLength_ > iAvail - sizeof(cHeader))			{ ASSERT(0); return NULL; }

	// cHeader.iLength_ 에 Peer ID 의 4바이트만큼이 들어가 있다.
	pBuffer->Read(*pPeerID);

	//J9::Ptr<J9::UDPPacket> pPacket = J9::UDPPacket::Create(pRcvPtr_ + iOffset, cHeader.iLength_ - sizeof(*pPeerID));
	J9::Ptr<J9::UDPPacket> pPacket(ALLOC, pBuffer->GetReadPointer(), cHeader.iLength_ - sizeof(*pPeerID));
	pBuffer->SetReadOffset(pBuffer->GetReadOffset() + cHeader.iLength_ - sizeof(*pPeerID));
	return pPacket;
}

//J9::Ptr<J9::UDPPacket>
//UDPSocket::Receive(U32* pPeerID, SOCKADDR* pAddr, S32* pSize)
//{
//	// read from socket
//	U8	cReceived[J9::Socket::iSocketBufferLength];
//	S32 iReceived = SOCKET_readfrom(iFd_, cReceived, J9::Socket::iSocketBufferLength, pAddr, pSize);
//
//	// check error
//	if (iReceived <= 0)	return false;
//
//	// initialize to read
//	ASSERT(iReceived < J9::Socket::iSocketBufferLength);
//	ASSERT(iReceived >= J9::UDPSocket::iUDPHeaderLength);
//
//	U32 iOffset = 0;
//	UDPHeader	cHeader;
//
//	::memcpy(&cHeader, cReceived + iOffset, sizeof(cHeader));	iOffset += sizeof(cHeader);
//
//	if (cHeader.iHeader_ != iUDPSocketHeader)					{ ASSERT(0); return false; }
//	if (cHeader.iLength_ != iReceived - sizeof(cHeader))		{ ASSERT(0); return false; }
//
//	::memcpy(pPeerID, cReceived + iOffset, sizeof(*pPeerID));	iOffset += sizeof(*pPeerID);
//
//	//return J9::UDPPacket::Create(cReceived + iOffset, iReceived - iOffset);
//	return J9::Ptr<J9::UDPPacket>(ALLOC, cReceived + iOffset, iReceived - iOffset);
//}

S32
UDPSocket::SendTo(const U8* p, Size iSize, const SOCKADDR_IN* pAddr)
{
	return SOCKET_writeto(iFd_, p, (S32) iSize, _rc<const SOCKADDR*>(pAddr), sizeof(*pAddr));
}

S32
UDPSocket::SendTo(U32 iPeerID, J9::Ptr<UDPPacket> pPacket, const SOCKADDR_IN* pAddr)
{
	S32	iTotalSize = _sc<S32>(pPacket->GetWriteOffset() + sizeof(iPeerID));

	if (J9::Socket::iSocketBufferLength < iTotalSize)	return -1;

	Buffer cBuffer(sizeof(UDPHeader) + iTotalSize);
	cBuffer.Write(1, &iUDPSocketHeader);
	cBuffer.Write(2, &iTotalSize);
	cBuffer.Write(4, &iPeerID);
	cBuffer.Write(pPacket->GetWriteOffset(), pPacket->GetPointer());

	// encrypt the packet (p of length i) here if necessary
	return SendTo(cBuffer.GetPointer(), cBuffer.GetSize(), pAddr);
}

////////////////////////////////////////////////////////////////////////////////
// UDPPacket
////////////////////////////////////////////////////////////////////////////////

//J9::Ptr<J9::UDPPacket>
//UDPPacket::_Create(Size i)
//{
//	ASSERT(i >= J9::UDPPacket::iUDPPacketHeaderSize);
//	J9::Ptr<J9::UDPPacket> pPacket = J9::Ptr<J9::UDPPacket>(ALLOC, i);
//	return pPacket;
//}
//
//J9::Ptr<J9::UDPPacket>
//UDPPacket::_Create(const U8* p, Size i)
//{
//	ASSERT(i >= J9::UDPPacket::iUDPPacketHeaderSize);
//	J9::Ptr<J9::UDPPacket> pPacket = J9::Ptr<J9::UDPPacket>(ALLOC, p, i);
//	return pPacket;
//}

void
UDPPacket::SetType(UDPPacketType eType)
{
	Write(1, &eType, 0);
}

void
UDPPacket::SetIndex(U32 iIndex)
{
	Write(4, &iIndex, 1);
}

void
UDPPacket::Dump(J9::String sReason, bool bCut)
{
	StringW	sDump;

	for (Size i = 0; i < iWriteOffset_; ++i)
	{
		if (bCut && i < iReadOffset_) continue;
		sDump += StringUtil::Format(_W("0x%x "), cBuffer_[i]);
	}

	LOG(_W("- UDPPacket::Dump (%s) protocol/type/index = (%d:%d) (read %d / write %d) (%s)\r\n"), sReason.c_str(), GetProtocol(), GetType(), iReadOffset_, iWriteOffset_, sDump.c_str());
}
