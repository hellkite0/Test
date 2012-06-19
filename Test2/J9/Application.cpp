// $Id$

#include "stdafx.h"
#include "Application.h"

#include "Tokens.h"
#include "Hardware.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#ifdef J9_USE_VSPROFILER
#include <VSPerf.h>
#endif//J9_USE_VSPROFILER
#include "Socket.h"

using namespace J9;

const F32	Application::fTickInterval = _sc<F32>(TickInterval) * 0.001f;

Application::Application()
: bExit_(false),
  bWindowed_(true),
  bPaused_(false),
  iScreenWidth_(1024),
  iScreenHeight_(768)
{
	Hardware::Init();

	iCurUpdatedTick_ = 0;
	bResourceLoadingProcess_ = false;

	if (J9_AUTO_START_PROFILE)
		StartProfile();

	if (J9_AUTO_RESUME_PROFILE)
		ResumeProfile();
	else
		SuspendProfile();

	cSystemTime_.SyncToSystem();
	sSystemTime_ = cSystemTime_.ToString();

	iTickCount_ = 0;
}

U16
Application::GetCodePage()
{
	if (HasInstance())
		return Instance()->_GetCodePage();

	return 949;
}

U16
Application::_GetCodePage()
{
	return 949;
}

bool
Application::IsNT()
{
	return ::GetVersion() < 0x80000000;
}

U32
Application::GetMilliSec()
{
	return ::timeGetTime();
}

void
Application::StartTick()
{
	LOG(_W("cpu info physical %u, logical %u"),
		Hardware::CPU::GetNumPhysicalCores(),
		Hardware::CPU::GetNumLogicalCores());

	iTick_ = 0;
	iTickTime_ = 0;
	iSystemTime_ = GetMilliSec();
	iCurSystemTime_ = GetMilliSec();
}

U32
Application::AdvanceTick()
{
	iCurSystemTime_ = GetMilliSec();
	if (!bPaused_ && TickInterval <= iCurSystemTime_ - iSystemTime_)
	{
		++iTick_;
		iTickTime_ += TickInterval;
		iSystemTime_ += TickInterval;
		cSystemTime_.SyncToSystem();
		sSystemTime_ = cSystemTime_.ToString();
	}

	return iTick_;
}

U32
Application::GetAdvanceTickToProcess()
{
	iTickCount_++;
	U32 iTickToProcess = 0;
	iCurSystemTime_ = GetMilliSec();
	if (!bPaused_ && TickInterval <= iCurSystemTime_ - iSystemTime_)
	{
		//LOG(_W("! Tick Count: %d\r\n"), iTickCount_);
		iTickCount_ = 0;
		iTickToProcess = (iCurSystemTime_ - iSystemTime_) / TickInterval;
	}
	return iTickToProcess;
}

void
Application::PauseTick(bool bPause)
{
	bPaused_ = bPause;

	if (!bPaused_)
	{
		// ¸®ÁÜ½Ã Áï½Ã ÇÑÆ½ °¡µµ·Ï
		iSystemTime_ = GetMilliSec() - TickInterval;
	}
}

bool
J9::Application::IsBubuging() const
{
	return !bPaused_ && TickInterval < (iCurSystemTime_ - iSystemTime_) && iCurUpdatedTick_ > 1;
}

void
Application::LoadProfile(const WChar* pProfileName, const WChar* pProfilePath)
{
	StringW	sProfile;
	StringW sProfilePath;
	if (!pProfileName || ::wcscmp(pProfileName, _W("")) == 0)
		sProfile = GetConfig().GetChild(_W("profile")).GetData();
	else
		sProfile = pProfileName;

	ASSERT(!sProfile.empty());

	if (!pProfilePath || ::wcscmp(pProfilePath, _W("")) == 0)
		sProfilePath = _W("data/xml/profiles");
	else
		sProfilePath = pProfilePath;

	ASSERT(!sProfilePath.empty());

	xProfile_.Load(StringUtil::Format(_W("%s/%s.xml"), sProfilePath.c_str(), sProfile.c_str()).c_str());

	SetPublisherName(xProfile_.GetChild(Tokens::publisher::W()).GetData().c_str());
	SetCompanyName(xProfile_.GetChild(Tokens::company::W()).GetData().c_str());
	SetProductName(xProfile_.GetChild(Tokens::product::W()).GetData().c_str());
}

bool
Application::IsDebuggerPresent() const
{
	static bool	bChecked = false;
	if (!bChecked)
	{
		bChecked = true;
		bDebuggerPresent_ = ::IsDebuggerPresent() != 0;
	}

	return bDebuggerPresent_;
}

#ifdef J9_USE_VSPROFILER
void
J9::Application::StartProfile(bool bThreadLevel)
{
	// StartProfile and StopProfile control the
	// Start/Stop state for the profiling level. 
	// The default initial value of Start/Stop is 1. 
	// The initial value can be changed in the registry. 
	// Each call to StartProfile sets Start/Stop to 1; 
	// each call to StopProfile sets it to 0. 

	// Declare enumeration to hold return value of 
	// the call to StartProfile.
	PROFILE_COMMAND_STATUS iProfileResult;

	PROFILE_CONTROL_LEVEL	iLevel = bThreadLevel ? PROFILE_THREADLEVEL : PROFILE_PROCESSLEVEL;

	iProfileResult = ::StartProfile(
		iLevel,
		PROFILE_CURRENTID);

	if (iProfileResult != PROFILE_OK)
	{
		//LOG(_W("StartProfile returned %d.\r\n"), iProfileResult);
	}
}

void
J9::Application::StopProfile(bool bThreadLevel)
{
	// StartProfile and StopProfile control the 
	// Start/Stop state for the profiling level. 
	// The default initial value of Start/Stop is 1. 
	// The initial value can be changed in the registry. 
	// Each call to StartProfile sets Start/Stop to 1; 
	// each call to StopProfile sets it to 0. 

	// Declare enumeration to hold result of call
	// to StopProfile.
	PROFILE_COMMAND_STATUS iProfileResult;

	PROFILE_CONTROL_LEVEL	iLevel = bThreadLevel ? PROFILE_THREADLEVEL : PROFILE_PROCESSLEVEL;

	iProfileResult = ::StopProfile(
		iLevel,
		PROFILE_CURRENTID);

	if (iProfileResult != PROFILE_OK)
	{
		//LOG(_W("StopProfile returned %d.\r\n"), iProfileResult);
	}
}

void
J9::Application::ResumeProfile(bool bThreadLevel)
{
	// The initial value of the Suspend/Resume counter is 0. 
	// Each call to SuspendProfile adds 1 to the Suspend/Resume 
	// count; each call to ResumeProfile subtracts 1. 

	// Declare enumeration to hold result of call to ResumeProfile
	PROFILE_COMMAND_STATUS iProfileResult;

	PROFILE_CONTROL_LEVEL	iLevel = bThreadLevel ? PROFILE_THREADLEVEL : PROFILE_PROCESSLEVEL;

	iProfileResult = ::ResumeProfile(
		iLevel,
		PROFILE_CURRENTID);

	if (iProfileResult != PROFILE_OK)
	{
		//LOG(_W("ResumeProfile returned %d.\r\n"), iProfileResult);
	}
}

void
J9::Application::SuspendProfile(bool bThreadLevel)
{
	// The initial value of the Suspend/Resume counter is 0.
	// Each call to SuspendProfile adds 1 to the
	// Suspend/Resume count; each call
	// to ResumeProfile subtracts 1.

	// Declare enumeration to hold result of call
	// to SuspendProfile
	PROFILE_COMMAND_STATUS iProfileResult;

	PROFILE_CONTROL_LEVEL	iLevel = bThreadLevel ? PROFILE_THREADLEVEL : PROFILE_PROCESSLEVEL;

	iProfileResult = ::SuspendProfile(
		iLevel,
		PROFILE_CURRENTID);

	if (iProfileResult != PROFILE_OK)
	{
		//LOG(_W("SuspendProfile returned %d.\r\n"), iProfileResult);
	}
}

#endif//J9_USE_VSPROFILER

#ifdef J9_SOL_USE_SECURITY_VALUE
void
J9::Application::TeaseBadGuy(int iBad)
{
	if ((iBad & 0x000000ff) == iBad)
	{
		Socket::TeaseBadGuy(iBad);
	}
}
#endif