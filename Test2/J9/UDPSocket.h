// $Id: UDPSocket.h 3959 2009-09-25 09:50:04Z hyojin.lee $

#ifndef __J9_UDP_SOCKET_H__
#define __J9_UDP_SOCKET_H__

#include "Socket.h"

namespace J9
{
	class UDPPacket;

	class UDPSocket
	{
	private:
#ifdef _J9_USE_PTHREAD
		typedef PThread		ThreadModel;
#else
		typedef Win32Thread	ThreadModel;
#endif

	public:
		static void	Setup();
		static SOCKET_TYPE UDP_Bind(const SOCKADDR_IN* pAddr, S32 iAddrLen);

		UDPSocket(SOCKET_TYPE iFd = 0);
		~UDPSocket();

		void				SetFD(SOCKET_TYPE iFd, bool bBlock = false);
		SOCKET_TYPE			GetFD() { return iFd_; }
		void				Close();
		void				Reset();

		S32					SendTo(U32 iPeerID, J9::Ptr<J9::UDPPacket> pPacket, const SOCKADDR_IN* pAddr);
		S32					SendTo(const U8* p, Size iSize, const SOCKADDR_IN* pAddr);

		bool					Receive(SOCKADDR* pAddr, S32* pSize);
		J9::Ptr<J9::UDPPacket>	Read(U32* pPeerID);

		// 서버 전용 함수
		bool					Receive(J9::Buffer& rBuffer, SOCKADDR* pAddr, S32* pSize);
		J9::Ptr<J9::UDPPacket>	Read(J9::Buffer* pBuffer, U32* pPeerID);

	public:
		static const S32	iUDPHeaderLength = 7; // header (1), length (2), peer id (4)
		static const char	iUDPSocketHeader = '~';

	private:
		ThreadModel::Lock	cLock_;

		SOCKET_TYPE			iFd_;
		U8*					pRcvBuf_;
		U8*					pRcvPtr_;
		U8*					pRcvEnd_;

		bool				bBlock_;
	};

	class UDPPacket : public Packet
	{
	public:
		enum UDPPacketType
		{
			eUPTRaw,
			eUPTSend,
			eUPTNotify,
			eUPTReply,
			eUPTRequest,
			eUPTHolePunch,
			eUPTRelay,
		};
	public:

		//static J9::Ptr<J9::UDPPacket> _Create(Size i);
		//static J9::Ptr<J9::UDPPacket> _Create(const U8* p, Size i);

		// 1 byte type, 4 byte index, 1 byte protocol
		inline U8					GetProtocol() { return cBuffer_[5]; }
		virtual U8			GetType() { return eUPTRaw; }

		UDPPacket(Size i) : Packet(i) { Make("14", eUPTRaw, 0); }
		UDPPacket(const U8* p, Size i) : Packet(i) { Write(i, p); } 	// 이경우는 udp packet copy only, all other headers should already present ;;;

		void SetIndex(U32 iIndex);

		void	Dump(J9::String sReason, bool bCut = false);
	protected:
		void SetType(UDPPacketType eType);

	private:
		static const Size iUDPPacketHeaderSize = 6; // packet type (1), index (4), packet protocol (1)
	};

	class UDPSendPacket : public UDPPacket
	{
	public:
		UDPSendPacket(Size i) : UDPPacket(i) { SetType(eUPTSend); }
		virtual U8			GetType() { return eUPTSend; }
	};

	class UDPReplyPacket : public UDPPacket
	{
	public:
		UDPReplyPacket(Size i) : UDPPacket(i) { SetType(eUPTReply); }
		virtual U8			GetType() { return eUPTReply; }
	};

	class UDPRequestPacket : public UDPPacket
	{
	public:
		UDPRequestPacket(Size i) : UDPPacket(i) { SetType(eUPTRequest); }
		virtual U8			GetType() { return eUPTRequest; }
	};

	class UDPHolePunchPacket : public UDPPacket
	{
	public:
		UDPHolePunchPacket(Size i) : UDPPacket(i) { SetType(eUPTHolePunch); }
		virtual U8			GetType() { return eUPTHolePunch; }
	};

	class UDPRelayPacket : public UDPPacket
	{
	public:
		UDPRelayPacket(Size i) : UDPPacket(i) { SetType(eUPTRelay); }
		virtual U8			GetType() { return eUPTRelay; }
	};

	class UDPNotifyPacket : public UDPPacket
	{
	public:
		UDPNotifyPacket(Size i) : UDPPacket(i) { SetType(eUPTNotify); }
		virtual U8			GetType() { return eUPTNotify; }
	};


};

#endif//__J9_UDP_SOCKET_H__
