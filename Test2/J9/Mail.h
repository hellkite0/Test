// $Id: Mail.h 7065 2010-08-21 09:55:45Z hyojin.lee $

#ifndef __J9_MAIL_H__
#define __J9_MAIL_H__

#include "XML.h"

#if defined(_SERVER) || defined(_DEBUG) || defined(FAST_DEBUG)
	#define SEND_CRASH_MAIL
#endif

namespace J9
{
#ifdef SEND_CRASH_MAIL
	class Mail
	{
	public:
		static void Setup();
		static bool Parse(J9::XMLNode cNode);
		static bool SendAlert(const J9::String& rCallStack);
		static bool SendAlert();
		static J9::XMLNode GetConfig();
	public:
		Mail();
		~Mail();

		inline void AddReceiver(const J9::String& rReceiver)	{ cReceivers_.push_back(rReceiver); }
		inline void SetSender(const J9::String& rSender)		{ sSender_ = rSender; }
		inline void SetSubject(const J9::String& rSubject)		{ sSubject_ = rSubject; }
		inline void SetContents(const J9::String& rContents)	{ sContents_ = rContents; }
		inline void SetPriority(U8 i)							{ iPriority_ = i; }

		bool SendMail(U32 iAddr, U16 iPort);
		U32 GetResponseCode(const U8* pResponse);

	private:		
		void SendMessage();
		bool Disconnect();
		bool Connect(U32 iAddr, U16 iPort);

		bool SendMessage(U32 iCode, U8* pMessage);

	public:
		static Mail cAlertMail;
		static U32 iSMTPServerIP;
		static U16 iSMTPPort;

	private:
		std::list<J9::String> cReceivers_;
		J9::String sSender_;
		J9::String sSubject_;
		J9::String sContents_;
		U8 iPriority_;
		S32 iFD_;
	};

	class MailManager
	{
	public:
		static void Setup();
		static void Shutdown();

	public:

	private:
		//J9::Que<J9::Ptr<Mail> > cMailQue_;
	};
#endif
};

#endif // __J9_MAIL_H_
