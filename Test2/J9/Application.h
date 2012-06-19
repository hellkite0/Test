// $Id$

#ifndef __J9_APPLICATION_H__
#define __J9_APPLICATION_H__

#include "XML.h"
#include "Time.h"

#ifndef J9_AUTO_START_PROFILE
	#define J9_AUTO_START_PROFILE	(true)
#endif//J9_AUTO_START_PROFILE
#ifndef J9_AUTO_RESUME_PROFILE
	#define J9_AUTO_RESUME_PROFILE	(true)
#endif//J9_AUTO_RESUME_PROFILE

namespace J9
{
	class Application
	{
		DYNAMIC_SINGLETON(Application);

	public:
		enum
		{
			TickInterval = 20,
			TicksPerSecond = 50,	// 1000 / 20
		};

		static const F32	fTickInterval;

	public:
		Application();
		virtual ~Application() {}

		static U16			GetCodePage();
		bool				IsNT();
		U32					GetMilliSec();
		Size				GetScreenWidth() const					{ return iScreenWidth_; }
		Size				GetScreenHeight() const					{ return iScreenHeight_; }
		F32					GetScreenAspectRatio()					{ return _sc<F32>(iScreenWidth_) / _sc<F32>(iScreenHeight_); }

		void				StartTick();
		U32					AdvanceTick();
		U32					GetAdvanceTickToProcess();
		void				PauseTick(bool bPause = true);
		bool				IsPaused() const						{ return bPaused_; }
		void				ResumeTick()							{ PauseTick(false); }
		__forceinline U32	GetTick() const							{ return iTick_; }
		__forceinline U32	GetTickTime() const						{ return iTickTime_; }
		__forceinline F32	GetTickInterval() const					{ return fTickInterval; }
		bool				IsBubuging() const;
		const Time&			GetTime() const							{ return cSystemTime_; }
		const J9::StringW&	GetTimeString() const					{ return sSystemTime_; }

		const StringW&		GetPublisherName() const				{ return sPublisherName_; }
		void				SetPublisherName(const WChar* pName)	{ sPublisherName_ = pName; }
		const StringW&		GetCompanyName() const					{ return sComplayName_; }
		void				SetCompanyName(const WChar* pName)		{ sComplayName_ = pName; }
		const StringW&		GetProductName() const					{ return sProductName_; }
		void				SetProductName(const WChar* pName)		{ sProductName_ = pName; }

		virtual void		LoadConfig(const WChar* pFileName)		{ if (!xConfig_.Load(pFileName))	ASSERT(false); }
		virtual void		SaveConfig(const WChar* pFileName)		{ if (!xConfig_.Save(pFileName))	ASSERT(false); }
		const XMLNode&		GetConfig() const						{ return xConfig_; }

		void				LoadProfile(const WChar* pProfileName, const WChar* pProfilePath = NULL);
		XMLNode				GetProfile() const						{ return xProfile_; }

		bool				IsDebuggerPresent() const;

		virtual void		Exit() { bExit_ = true; }

#ifdef J9_USE_VSPROFILER
		void				StartProfile(bool bThreadLevel = false);
		void				StopProfile(bool bThreadLevel = false);
		void				ResumeProfile(bool bThreadLevel = false);
		void				SuspendProfile(bool bThreadLevel = false);
#else
		void				StartProfile(bool bThreadLevel = false)			{ UNUSED(bThreadLevel); }
		void				StopProfile(bool bThreadLevel = false)			{ UNUSED(bThreadLevel); }
		void				ResumeProfile(bool bThreadLevel = false)		{ UNUSED(bThreadLevel); }
		void				SuspendProfile(bool bThreadLevel = false)		{ UNUSED(bThreadLevel); }
#endif

#ifdef J9_SOL_USE_SECURITY_VALUE
		void				TeaseBadGuy(int iBad);
#endif

	private:
		U16					_GetCodePage();

	protected:
		bool				bExit_;
		bool				bWindowed_;
		bool				bResourceLoadingProcess_;

		Size				iScreenWidth_;
		Size				iScreenHeight_;
		XMLNode				xConfig_;
		XMLNode				xProfile_;

		U32					iCurUpdatedTick_;

	private:
		U32					iTick_;
		U32					iTickTime_;
		U32					iSystemTime_;
		U32					iCurSystemTime_;
		Time				cSystemTime_;
		StringW				sSystemTime_;
		StringW				sPublisherName_;
		StringW				sComplayName_;
		StringW				sProductName_;
		bool				bPaused_;
		mutable bool		bDebuggerPresent_;

		U32 iTickCount_;
	};

	__inline Application*
	pApp()
	{
		return Application::Instance();
	}
}

inline U32
CurTime()
{
	return J9::pApp()->GetTickTime();
}

inline F32
TickInterval()
{
	return J9::pApp()->GetTickInterval();
}

#endif//__J9_APPLICATION_H__