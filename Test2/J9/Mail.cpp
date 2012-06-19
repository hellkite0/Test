// $Id: Mail.cpp 8528 2011-05-24 03:12:42Z junghyun.bae $

#include "StdAfx.h"
#include "Mail.h"

#include "Socket.h"
#include "Tokens.h"
#include "Application.h"
#include "UtilityString.h"
#include "UtilityWin32.h"
#include "Log.h"
#include "Time.h"

using namespace J9;

#ifdef SEND_CRASH_MAIL
Mail	Mail::cAlertMail;
U32		Mail::iSMTPServerIP	= inet_addr("121.189.61.151");
U16		Mail::iSMTPPort		= htons(IPPORT_SMTP);

J9::XMLNode
Mail::GetConfig()
{
	return pApp()->GetConfig().GetChild(Tokens::mails::W());
}

void
Mail::Setup()
{
	static bool bInitialized = false;
    if (bInitialized) return; else bInitialized = true;

	Parse(GetConfig());
}

bool
Mail::Parse(J9::XMLNode cNode)
{
	J9::String s = cNode.GetChild(Tokens::smtp::W());
	iSMTPServerIP = inet_addr(StringUtil::ToStringA(s).c_str());

	U16 iPort = cNode.GetChild(Tokens::port::W());
	iSMTPPort = htons(iPort);

	J9::String sSender = cNode.GetChild(Tokens::sender::W());
	J9::String sSubject = cNode.GetChild(Tokens::subject::W());
	J9::XMLNode cReceivers = cNode.GetChild(Tokens::receivers::W());
	J9::XMLNodeList cList = cReceivers.GetChildList(Tokens::receiver::W());
	Size iSize = cList.GetSize();
	for (Size i = 0; i < iSize; ++i)
	{
		J9::XMLNode cReceiverNode = cList.Get(i);
		J9::String sReceiver = cReceiverNode.GetData();
		cAlertMail.AddReceiver(sReceiver);
	}

	cAlertMail.SetSubject(sSubject);
	cAlertMail.SetSender(sSender);
	cAlertMail.SetPriority(1);
	return true;
}

bool
Mail::SendAlert()
{
	return SendAlert(_W("abort / application kill by non-assert"));
}

bool
Mail::SendAlert(const J9::String& rCallStack)
{
	J9::Time	cTime;
	cAlertMail.SetSubject(StringUtil::Format(_W("%s at %04d:%02d:%02d %02d:%02d:%02d from %s"), _W("CrashAlert"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), cAlertMail.sSubject_.c_str()));
	J9::String sCallStack = rCallStack;
	J9::StringUtil::ReplaceAll(sCallStack, J9::String(_W("\r\n")), J9::String(_W("<br />")));
	J9::String sContent = J9::StringUtil::Format(_W("<style> td { font-family: Helvetica Neue, verdana, tahoma, helvetica, serif, arial; font-size: 9pt; }</style><body><table><tr><td>%s</td></tr></table></body>"),
		sCallStack.c_str());
	cAlertMail.SetContents(sContent);
	return cAlertMail.SendMail(iSMTPServerIP, iSMTPPort);
}

Mail::Mail()
: sSender_(_W("")), sSubject_(_W("")), sContents_(_W("")), iPriority_(1), iFD_(0)
{
}

Mail::~Mail()
{
}

bool
Mail::Connect(U32 iAddr, U16 iPort)
{
	struct sockaddr_in cSock;
	memset(&cSock, 0, sizeof(cSock));
	cSock.sin_family	 		= AF_INET;
	cSock.sin_addr.s_addr		= iAddr;
	cSock.sin_port				= iPort;

	if (iFD_ = (S32) J9::SOCKET_socket(AF_INET, SOCK_STREAM, 0), iFD_ < 0)
	{
		J9::Win32::LogError(errno, _W(" ! socket to mail server: %s\r\n"));
		return false;
	}

	if (J9::SOCKET_connect(iFD_, (struct sockaddr *) &cSock, sizeof(cSock)) < 0)
	{
		J9::Win32::LogError(errno, _W(" ! connect to mail server: %s\r\n"));
		J9::SOCKET_close(iFD_);
		return false;
	}

	//AChar	cWriteBuf[DEFAULT_NUMBER];
	U8		cReadBuf[DEFAULT_NUMBER];

	//S32 iOffset = sprintf_s(cWriteBuf, "HELO %s\r\n", inet_ntoa(cSock.sin_addr));

	if (J9::SOCKET_read(iFD_, cReadBuf, sizeof(cReadBuf)) <= 0 || GetResponseCode(cReadBuf) != 220)
	{
		LOG(_W(" ! mail connection error\r\n"));
		return false;
	}

	/*if (J9::SOCKET_write(iFD_, (const U8*) cWriteBuf, iOffset) <= 0)	
	{
		LOG(_W(" ! mail socket write helo error\r\n"));
		return false;
	}
	if (J9::SOCKET_read(iFD_, cReadBuf, sizeof(cReadBuf)) <= 0 || GetResponseCode(cReadBuf) != 250)
	{
		LOG(_W(" ! mail socket read helo error\r\n"));
		return false;
	}*/

	return true;
}

bool
Mail::Disconnect()
{
	AChar	cWriteBuf[DEFAULT_NUMBER];
	U8		cReadBuf[DEFAULT_NUMBER];
	S32 iOffset = sprintf_s(cWriteBuf, "QUIT\r\n");

	if (J9::SOCKET_write(iFD_, (const U8*) cWriteBuf, iOffset) <= 0)		LOG(_W(" ! mail socket write error\r\n"));
	if (J9::SOCKET_read(iFD_, cReadBuf, sizeof(cReadBuf)) <= 0) LOG(_W(" ! mail socket read error\r\n"));

	J9::SOCKET_close(iFD_);
	return true;
}

bool
Mail::SendMail(U32 iAddr, U16 iPort)
{
	if (cReceivers_.size() == 0)	return false;
	if (sContents_.empty())			return false;
	if (sSubject_.empty())			return false;

	if (!Connect(iAddr, iPort))		return false;

	SendMessage();

	Disconnect();
	LOG(_W("! mail send complete\r\n"));
	return true;
}

U32
Mail::GetResponseCode(const U8* pResponse)
{	
	return atoi((const char* ) pResponse);
}

bool
Mail::SendMessage(U32 iCode, U8* pMessage)
{
	AChar	cWriteBuf[HUGE_NUMBER] = {0,};
	U8		cReadBuf[BIG_NUMBER] = {0,};
	S32 iOffset = 0;

	iOffset = sprintf_s(cWriteBuf, "%s\r\n", pMessage);

	if (J9::SOCKET_write(iFD_, (const U8*) cWriteBuf, iOffset) <= 0)		
	{
		LOG(_W(" ! mail socket write error: [%S]\r\n"), pMessage);
		return false;
	}

	if (J9::SOCKET_read(iFD_, cReadBuf, sizeof(cReadBuf)) <= 0 || GetResponseCode(cReadBuf) != iCode)
	{
		LOG(_W(" ! mail socket read error: [%S]\r\n"), cReadBuf);
		return false;
	}

	return true;
}

void
Mail::SendMessage()
{
	J9::Time cTime;
	AChar	cWriteBuf[HUGE_NUMBER] = {0,};
	U8		cReadBuf[BIG_NUMBER] = {0,};
	S32 iOffset = 0;

	if (!SendMessage(250, (U8 *) "ehlo")) return;
	if (!SendMessage(334, (U8 *) "AUTH LOGIN")) return;
	
	// j9mailer@just9.co.kr / 081880 to Mailnara SMTP server
	//if (!SendMessage(334, (U8 *) "ajltYWlsZXJAanVzdDkuY28ua3I=")) return;
	//if (!SendMessage(235, (U8 *) "MDgxODgw")) return;

	// j9mailer1 / !apdlf999 to justnine office mailserver (192.168.1.12)
	if (!SendMessage(334, (U8 *) "ajltYWlsZXIx")) return;
	if (!SendMessage(235, (U8 *) "IWFwZGxmOTk5")) return;
	
	iOffset = sprintf_s(cWriteBuf, "MAIL From:<%S>", sSender_.c_str());
	if (!SendMessage(250, (U8 *) cWriteBuf)) return;
	
	ZeroMemory(cReadBuf,BIG_NUMBER);
	std::list<J9::String>::iterator cIter, cEnd = cReceivers_.end();
	for (cIter = cReceivers_.begin(); cIter != cEnd; ++cIter)
	{
		iOffset = sprintf_s(cWriteBuf, "RCPT To:<%S>\r\n", cIter->c_str());
		//iBufOffset += sprintf_s(cBuf + iBufOffset, countof(cBuf) - iBufOffset, "%S;", cIter->c_str());

		if (J9::SOCKET_write(iFD_, (const U8*) cWriteBuf, iOffset) <= 0)		LOG(_W(" ! mail socket write error\r\n"));
		if (J9::SOCKET_read(iFD_, cReadBuf, sizeof(cReadBuf)) <= 0 || (GetResponseCode(cReadBuf) != 251 && GetResponseCode(cReadBuf) != 250))   
			LOG(_W(" ! mail socket mail rcpt read error [%S]\r\n"), cReadBuf);
	}

	if (!SendMessage(354, (U8 *) "DATA")) return;

	iOffset = sprintf_s(cWriteBuf, "Date: %S, %02d %S %04d %02d:%02d:%02d +0900\r\n", 
		cTime.GetWeekDayString(), cTime.GetDay(), cTime.GetMonthString(), cTime.GetYear(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
	
	iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "From:<%S>\r\n", sSender_.c_str());

	cEnd = cReceivers_.end();
	for (cIter = cReceivers_.begin(); cIter != cEnd; ++cIter)
	{
		iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "To:<%S>\r\n", cIter->c_str());		
	}	
	
	iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "Subject: %S\n", sSubject_.c_str());
	iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "MIME-Version: 1.0\r\n");
	iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "Content-Type: text/html; charset=utf-8\r\n");
	iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "X-Priority: %d\r\n", iPriority_);
	iOffset += sprintf_s(cWriteBuf + iOffset, countof(cWriteBuf) - iOffset, "\r\n%S\r\n.", sContents_.c_str());

	if (!SendMessage(250, (U8 *) cWriteBuf)) return;
}
#endif