// $Id: Socket.h 7231 2010-08-26 06:05:51Z kyuhun.lee $
#ifndef __J9_SOCKET_H__
#define __J9_SOCKET_H__

#include <winsock2.h>
#pragma comment (lib, "WS2_32.LIB") // win sock version 2.0

#include "Thread.h"
#include "Mutex.h"
#include "Buffer.h"

#ifdef J9_SOL_USE_XTRAP
	// xtrap crytp
	#include "../../../Externals/xTrapEncrypt/XTrapSDK_Crypto/XTrapSDK_Crypto.h"

	// Link SDK library file
	#if(defined _MT)
		#if(defined _DLL)
			#pragma comment(lib, "XTrapSDK_Crypto_mtdll.lib")
		#else
			#pragma comment(lib, "XTrapSDK_Crypto_mt.lib")
		#endif
	#else
		#pragma comment(lib, "XTrapSDK_Crypto_st.lib")
	#endif

	typedef struct
	{
		unsigned long pKey[8];
		unsigned int nKeySize;
		unsigned char	iEncryptAlgorithm;
		unsigned char cMode;
	} PER_SDK_DATA, *LPPER_SDK_DATA;
	// end of xtrap
#endif//J9_SOL_USE_XTRAP

namespace J9
{
	class Packet;
	struct SOCKETINFO;
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		TCP Socket Header
	///
	/// WinSock2 library 를 이용한 TCP Socket Implementation 이다.
	/// WinSock Method 들을 wrapping 한 함수들도 함께 제공한다.
	/// Connect, Listen 등의 static method 도 함께 제공한다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
	enum EnumSocketError
	{
		SOCKERR_NO = 0,
		SOCKERR_WOULDBLOCK	= 1000,
		SOCKERR_INTR		= 1001,
	};

	typedef SOCKET SOCKET_TYPE;	// in case of windows
	typedef INT socklen_t;

	// socket function wrapping
	inline static SOCKET_TYPE	SOCKET_socket(S32 af, S32 type, S32 protocol)
	{
		return socket(af, type, protocol);
	}

	inline static SOCKET_TYPE	SOCKET_tcp()
	{
		return SOCKET_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	inline static SOCKET_TYPE	SOCKET_udp()
	{
		return SOCKET_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	inline static SOCKET_TYPE	SOCKET_iocp()
	{
		return WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}

	inline static SOCKET_TYPE	SOCKET_accept(SOCKET_TYPE fd, sockaddr* addr, socklen_t* addrlen)
	{
		return accept(fd, addr, addrlen);
	}
	inline static S32			SOCKET_getpeername(SOCKET_TYPE fd, sockaddr* addr, S32* addrlen)
	{
		return getpeername(fd, (sockaddr*) addr, (socklen_t*) addrlen);
	}

	inline static S32			SOCKET_getsockname(SOCKET_TYPE fd, sockaddr* addr, S32* addrlen)
	{
		return getsockname(fd, (sockaddr*) addr, (socklen_t*) addrlen);
	}
	inline static S32			SOCKET_connect(SOCKET_TYPE fd, const sockaddr FAR* name, S32 namelen)
	{
		return connect(fd, name, namelen);
	}

	inline static S32			SOCKET_write(SOCKET_TYPE fd, const U8* buf, S32 bytelen)
	{
		return send(fd, (const char *) buf, bytelen, 0);
	}

	inline static S32			SOCKET_writeto(SOCKET_TYPE fd, const U8* buf, S32 bytelen, const SOCKADDR FAR* to, S32 tolen)
	{
		return sendto(fd, (const char *) buf, bytelen, 0, to, tolen);
	}

	inline static S32			SOCKET_read(SOCKET_TYPE fd, U8* buf, S32 bytelen)
	{
		return recv(fd, (char *) buf, bytelen, 0);
	}

	inline static S32			SOCKET_readfrom(SOCKET_TYPE fd, U8* buf, S32 bytelen, SOCKADDR* from, S32* fromlen)
	{
		return recvfrom(fd, (char *) buf, bytelen, 0, from, (int*) fromlen);
	}

	inline static S32			SOCKET_close(SOCKET_TYPE fd)
	{
		return closesocket(fd);
	}

	inline static S32			SOCKET_setsockopt (SOCKET_TYPE fd, S32 level, S32 optname, const char FAR * optval, S32 optlen)
	{
		return setsockopt(fd, level, optname, optval, optlen);
	}

	inline static S32			SOCKET_shutdown(SOCKET_TYPE fd, S32 how)
	{
		return shutdown(fd, how);
	}
	inline static S32			SOCKET_getlasterror()
	{
		switch (WSAGetLastError())
		{
		case WSAEWOULDBLOCK:
			return SOCKERR_WOULDBLOCK;
		case WSAEINTR:
			return SOCKERR_INTR;
		default:
			return SOCKERR_NO;
		}
	}

	inline static S32			SOCKET_bind(SOCKET_TYPE fd, const sockaddr FAR* name, S32 namelen)
	{
		return bind(fd, name, namelen);
	}

	inline static S32			SOCKET_listen(SOCKET_TYPE fd, S32 backlog)
	{
		return listen(fd, backlog);
	}

	inline static S32			SOCKET_select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, const struct timeval FAR * timeout)
	{
		return select(nfds, readfds, writefds, exceptfds, timeout);
	}

	inline static bool			SOCKET_isvalid(SOCKET_TYPE fd)
	{
		return fd != INVALID_SOCKET;
	}

	inline static bool			SOCKET_setblocking(SOCKET_TYPE fd, bool bBlock)
	{
		U32 iArg = bBlock ? 0 : 1;

		return ioctlsocket(fd, FIONBIO, &iArg) == 0;
	}

#ifndef J9_SOL_ENCRYPT_SOCKET_KEY
	extern U32*				pSocketKey;
#endif

	class Socket
	{
		friend struct SOCKETINFO;
	private:
#ifdef _J9_USE_PTHREAD
		typedef PThread	ThreadModel;
#else
		typedef Win32Thread	ThreadModel;
#endif

	public:
		static void Setup();
		static void Shutdown();
#ifdef J9_SOL_ENCRYPT_SOCKET_KEY
		static void	TeaseBadGuy(int iBad);
#endif

		static SOCKET_TYPE TCP_Connect(U32 iAddr, U16 iPort);
		static SOCKET_TYPE TCP_Connect(const AChar* pAddr, U16 iPort);
		static SOCKET_TYPE TCP_Listen(U32 iAddr, U16 iPort);
		static SOCKET_TYPE TCP_Listen(const AChar* pAddr, U16 iPort);

		static J9::String const GetHostName() { return sHostName_; }
		
		Socket(SOCKET_TYPE iFd = 0);
		virtual ~Socket();

		SOCKET_TYPE		GetFD()				{ return iFd_; }
		bool			IsBlocked()			{ return pRcvEnd_ == pRcvPtr_ && SOCKET_getlasterror() == SOCKERR_WOULDBLOCK && !bBlock_; }
		bool			CheckHeader(U8 i)	{ /*UNUSED(i); ASSERT(i == *GetSocketHeader()); */ return (i == *GetSocketHeader());}
		const char*		GetSocketHeader()	{ return &iTCPSocketHeader; }

		void		SetFD(SOCKET_TYPE iFd, bool bBlock = false);
		void		Close();
		void		Reset();

		bool		Receive();
		bool		Send();

		J9::Ptr<Packet>		Read();

		bool		Write(const U8* pPacket, Size iLen);
		bool		Write(J9::Packet* p);

		void		Encrypt(const U8* pSrc, U8* pDst, Size iLen);
		void		Decrypt(const U8* pSrc, U8* pDst, Size iLen);
		void		SetSocketInfo(SOCKETINFO* pSockInfo);
		inline SOCKETINFO*	GetSockInfo()	{ return pSockInfo_; }

		bool			TryRead(); // for IOCP use only
		J9::Ptr<Packet> ReadIOCP(); // for IOCP use only
		bool			ReceiveIOCP(); // for IOCP use only

	public:
		static const S32	iDefaultPendingConnections = 10;
		static const Size	iSocketBufferLength = HUGE_NUMBER;//LARGE_NUMBER;	// 16k bytes
		static const S32	iHeaderLength = 7;
		//static const S32	iHeaderLength = 5;
		static const char	iTCPSocketHeader = '!';

		static J9::String	sHostName_;

	protected:
		SOCKET_TYPE			iFd_;
		U16					iRcvSeq_;
		U16					iSndSeq_;
		U16					iPrevSeq_;
		U32					iErrCnt_;

		U8*					pSndBuf_;
		U8*					pSndPtr_;

		U8*					pRcvBuf_;
		U8*					pRcvPtr_;
		U8*					pRcvEnd_;

		bool				bBlock_;
		ThreadModel::Lock	cLock_;
		SOCKETINFO*			pSockInfo_;
#ifdef J9_SOL_USE_XTRAP
		PER_SDK_DATA	cPerSDKData_;
		U8				iEncryptType_;

	public:
		enum eEncryptType
		{
			eEncryptNone,
			eEncryptXTrap,
			eEncryptJust9,
		};
		U32					SetEncryption(U8 iType, unsigned long *pKey);
		void				SetEncryptionkey(U8 iType, unsigned long *pKey, U32 iKeySize);
		// Key가 생성되어 있어야만 한다!!
		void				SetEncryption (U8 iType)		{ iEncryptType_ = iType; }
		inline U8			GetEncyrption() const			{ return iEncryptType_; }
		inline const PER_SDK_DATA&	GetEncryptData() const	{ return cPerSDKData_; }
		bool				NeedEncryption(const U8* pSrc);
#endif
		// 서버의 소켓도 xtrap를 사용하지 않게 되면
		// 4바이트 0을 더미로 쓰고 읽는 루틴은 삭제해도 된다
		Size		WriteCustomHeader(U8* pBuf);
		U32			ReadCustomHeader(const U8* pBuf);
	};

	enum IOCPSocketState
	{
		IOCPSocketState_None,
		IOCPSocketState_Recv,
		IOCPSocketState_Send,
	};
		
	struct SOCKETINFO// 소켓의 버퍼 정보를 구조체화.
	{		
		OVERLAPPED		cOverlapped_;
		SOCKET_TYPE		cSock_;
		WSABUF			cWSABuf_;
		SOCKADDR_IN		cAddr_;
		U16				iPort_;
		IOCPSocketState	cState_;
		Socket*			pSocket_;
		U8*				pBuffer_;

		SOCKETINFO()
			: cSock_(INVALID_SOCKET), iPort_(0), pSocket_(NULL)
		{
			Initialize();
		}

		SOCKETINFO(SOCKET_TYPE cSock, SOCKADDR_IN cAddr, U16 iPort, Socket* pSock)
			: cSock_(cSock), cAddr_(cAddr), iPort_(iPort), pSocket_(pSock)
		{
			Initialize();
		}

		void Initialize()
		{
			ASSERT(pSocket_ != NULL);
			ASSERT(pSocket_->pRcvBuf_ != NULL);

			pBuffer_ = pSocket_->pRcvBuf_;
			memset(pBuffer_, 0, J9::Socket::iSocketBufferLength);
			memset(&(cWSABuf_), 0, sizeof(WSABUF));

			cWSABuf_.len = J9::Socket::iSocketBufferLength;
			cWSABuf_.buf = (char*) pBuffer_;
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Packet Class Header
	///
	/// Socket 간 주고 받는 내용을 담는 Packet Class 이다.
	/// 단순한 char buffer 를 잘못사용하는 경우를 방지하고자 설계되었다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
	class Packet : public Buffer
	{
	public:
		Packet(Size i);
		Packet(const U8* p, Size i);
		virtual ~Packet();

		inline bool	IsResizable() { return iWriteOffset_ != cBuffer_.size(); }

		void	Reset();
		void	Dump(const J9::StringW& sReason, bool bCut = false);
		Size	Make(const char* pFormat, ...);
		Size	Make(const char* pFormat, va_list cArgList);

		Size	Ekam(const char* pFormat, ...);
		Size	Ekam(const char* pFormat, va_list cArgList);
	};

	typedef J9::Packet TCPPacket;

#ifdef _LOG_TRAFFIC_
	class _TrafficStat
	{
		//#define pTrafficStat J9::_TrafficStat::Instance()
		STATIC_SINGLETON(J9::_TrafficStat);
	public:
		_TrafficStat();

		void SetStatType(U8 iType) { Reset(); iType_ = iType; }

		void PrintTraffic();

		void Reset();

		void AddData(U8  iType, bool bIsSending, J9::Packet* pPacket);

		U8 GetStatType() { return iType_; }

		typedef struct
		{
			U8 iType_;
			U32 iCount_;
			U32 iSize_;
		}TrafficData;

		enum TrafficType
		{
			eTTUDP,		//UDP
			eTTInter,	//InterServer Traffic
			eTTSC,		//Server Client
		};
	private:
		U8	iType_;

		J9::IndexPool<TrafficData*> cSndTraffic_;
		J9::IndexPool<TrafficData*> cRcvTraffic_;

		#define ADDUDPSENDTRAFFIC(pPacket)			J9::_TrafficStat::Instance()->AddData(J9::_TrafficStat::eTTUDP, true, pPacket)
		#define ADDINTERSENDTRAFFIC(pPacket)		J9::_TrafficStat::Instance()->AddData(J9::_TrafficStat::eTTInter, true, pPacket)
		#define ADDSCSENDTRAFFIC(pPacket)			J9::_TrafficStat::Instance()->AddData(J9::_TrafficStat::eTTSC, true, pPacket)

		#define ADDUDPRECVTRAFFIC(pPacket)			J9::_TrafficStat::Instance()->AddData(J9::_TrafficStat::eTTUDP, false, pPacket)
		#define ADDINTERRECVTRAFFIC(pPacket)		J9::_TrafficStat::Instance()->AddData(J9::_TrafficStat::eTTInter, false, pPacket)
		#define ADDSCRECVTRAFFIC(pPacket)			J9::_TrafficStat::Instance()->AddData(J9::_TrafficStat::eTTSC, false, pPacket)

		#define ADDSENDTRAFFIC(iType, pPacket)		J9::_TrafficStat::Instance()->AddData(iType, true, pPacket)
		#define ADDRECVTRAFFIC(iType, pPacket)		J9::_TrafficStat::Instance()->AddData(iType, false, pPacket)

		#define STARTUDPTRACK()						J9::_TrafficStat::Instance()->SetStatType(J9::_TrafficStat::eTTUDP)
		#define STARTINTERTRACK()					J9::_TrafficStat::Instance()->SetStatType(J9::_TrafficStat::eTTInter)
		#define STARTSCTRACK()						J9::_TrafficStat::Instance()->SetStatType(J9::_TrafficStat::eTTSC)

		#define PRINTTRAFFIC()						J9::_TrafficStat::Instance()->PrintTraffic()
	};
#else

		#define ADDUDPSENDTRAFFIC(pPacket)
		#define ADDINTERSENDTRAFFIC(pPacket)
		#define ADDSCSENDTRAFFIC(pPacket)

		#define ADDUDPRECVTRAFFIC(pPacket)
		#define ADDINTERRECVTRAFFIC(pPacket)
		#define ADDSCRECVTRAFFIC(pPacket)

		#define ADDSENDTRAFFIC(iType, pPacket)
		#define ADDRECVTRAFFIC(iType, pPacket)

		#define STARTUDPTRACK()
		#define STARTINTERTRACK()
		#define STARTSCTRACK()

		#define PRINTTRAFFIC()
#endif
}

#endif//__J9_SOCKET_H__
