// $Id: Thread.cpp 7208 2010-08-25 04:55:41Z kyuhun.lee $

#include "StdAfx.h"
#include "Thread.h"

#ifdef _J9_USE_PTHREAD
#else//_J9_USE_PTHREAD
	#include <process.h>
#endif//_J9_USE_PTHREAD
#include "Log.h"

using namespace J9;

#ifdef _J9_USE_PTHREAD
////////////////////////////////////////////////////////////////////////////////
// PTW32_Init
////////////////////////////////////////////////////////////////////////////////
#ifdef PTW32_STATIC_LIB
PTW32_Init::PTW32_Init()
{
	VERIFY(pthread_win32_process_attach_np());
}

PTW32_Init::~PTW32_Init()
{
	VERIFY(pthread_win32_process_detach_np());
}
#endif

////////////////////////////////////////////////////////////////////////////////
// PThread
////////////////////////////////////////////////////////////////////////////////
typedef	stdext::hash_map<S32, PThread*>
ThreadPool;

ThreadPool PThread::cThreadsPool;

U32			iThreadIdentifier = 0;
bool		PThread::bShutdown;
bool		PThread::bDoneShuttingdown;

void
PThread::Setup()
{
	PThread::bShutdown = false;
	PThread::bDoneShuttingdown = false;
}

void
PThread::Shutdown()
{
	static bool bShutdown = false;
    if (bShutdown) return; else bShutdown = true;

	PThread* pThreads[SMALL_NUMBER];

	ThreadPool::iterator cIter, cEnd = cThreadsPool.end();
	S32 i = 0;
	for (cIter = cThreadsPool.begin(); cIter != cEnd; ++cIter)
	{
		pThreads[i++] = cIter->second;
	}

	for (S32 j = 0; j < i; ++j)
	{
		//Kill(pThreads[j]->GetID());
		delete pThreads[j];
	}
}

void
PThread::SignalShutdown()
{
	PThread::bShutdown = true;
}

PThread::PThread(bool bScope, bool bDetached)
{
	pthread_attr_init(&cAttr_);

	if (bScope) pthread_attr_setscope(&cAttr_, PTHREAD_SCOPE_SYSTEM);
    if (bDetached) pthread_attr_setdetachstate(&cAttr_, PTHREAD_CREATE_DETACHED);
}

PThread::~PThread()
{
	cThreadsPool.erase(this->cId_.x);
	pthread_attr_destroy(&cAttr_);
}

void
PThread::Start(Function pFunc, void* pArg)
{
	pFunc_ = pFunc;
	pArg_ = pArg;

	if (!pFunc_ || pthread_create(&cId_, &cAttr_, pFunc_, pArg_) < 0)
	{
		// warning that thread could not be started	and return
		LOG(_W(" ! thread (%d) could not be started\r\n"), cId_.x);
	}
	else
	{
		// put the thread in the thread pool for thread management
		cLock.Lock();
		cId_.x = ++iThreadIdentifier;
		cThreadsPool.insert(ThreadPool::value_type(this->cId_.x, this));
		cLock.Unlock();
	}
}

/// U32 iTime in mili-second
void
PThread::Sleep(U32 iTime)
{
	::Sleep(iTime);
}
#else//_J9_USE_PTHREAD
////////////////////////////////////////////////////////////////////////////////
// Win32Thread
////////////////////////////////////////////////////////////////////////////////
typedef	stdext::hash_map<S32, Win32Thread*>
ThreadPool;

ThreadPool Win32Thread::cThreadsPool;

U32			iThreadIdentifier = 0;
bool		Win32Thread::bShutdown;

void
Win32Thread::Setup()
{
	Win32Thread::bShutdown = false;
}

void
Win32Thread::Shutdown()
{
	static bool bShutdown = false;
    if (bShutdown) return; else bShutdown = true;

	Win32Thread* pThreads[SMALL_NUMBER];

	ThreadPool::iterator cIter, cEnd = cThreadsPool.end();
	S32 i = 0;
	for (cIter = cThreadsPool.begin(); cIter != cEnd; ++cIter)
	{
		pThreads[i++] = cIter->second;
	}

	for (S32 j = 0; j < i; ++j)
	{
		//Kill(pThreads[j]->GetID());
		delete pThreads[j];
	}
}

void
Win32Thread::SignalShutdown()
{
	Win32Thread::bShutdown = true;
}

Win32Thread::Win32Thread()
: iId_(0), iHandle_(0)
{
}

Win32Thread::~Win32Thread()
{
	cThreadsPool.erase(iId_);
}

void
Win32Thread::Start(Function pFunc, void* pArg)
{
	pFunc_ = pFunc;
	pArg_ = pArg;

	if (!pFunc_)
	{
		// warning that thread could not be started	and return
		LOG(_W(" Thread function is null. cannot start thread\r\n"));
	}

	iHandle_ = _beginthreadex(NULL, 0, pFunc_, pArg_, 0, &iId_);
	if (iHandle_ == -1L)
	{
	}
	else if (iHandle_ == 0)
	{
	}
	else
	{
		// put the thread in the thread pool for thread management
		cLock.Lock();
		cThreadsPool.insert(ThreadPool::value_type(iId_, this));
		cLock.Unlock();
	}
}

/// U32 iTime in mili-second
void
Win32Thread::Sleep(U32 iTime)
{
	::Sleep(iTime);
}

bool
Win32Thread::Kill(U32 iExitCode)
{
	if (iHandle_ == 0)	return true;
	bool bTerminated = ::TerminateThread((HANDLE) iHandle_, iExitCode) != 0;

	if (bTerminated)	iHandle_ = 0;

	return bTerminated;
}

////////////////////////////////////////////////////////////////////////////////
// Win32ThreadEx
////////////////////////////////////////////////////////////////////////////////
UINT
Win32ThreadEx::ThreadFunc(void* pArg)
{
	Win32ThreadEx*	pThread = _rc<Win32ThreadEx*>(pArg);

	UINT	iRet;

	iRet = pThread->Initialize();
	if (iRet)	return iRet;

	iRet = pThread->RunLoop();
	if (iRet)	return iRet;

	iRet = pThread->Finalize();
	if (iRet)	return iRet;

	return 0;
}

Win32ThreadEx::Win32ThreadEx()
: iSleepTime_(10),
bTerminate_(false),
bRunning_(false)
{
}

Win32ThreadEx::~Win32ThreadEx()
{
	bTerminate_ = true;
}

void
Win32ThreadEx::Start()
{
	Win32Thread::Start(ThreadFunc, this);
}

UINT
Win32ThreadEx::Initialize()
{
	bRunning_ = true;
	return 0;
}

UINT
Win32ThreadEx::RunLoop()
{
	while(!IsTerminating())
	{
		Sleep(iSleepTime_);

		UINT	iRet = RunOnce();
		if (iRet)	return iRet;
	}
	return 0;
}

UINT
Win32ThreadEx::RunOnce()
{
	// 이 함수가 불렸다는 것은 사용자가 클래스 구현을 덜했다는 것이다
	ASSERT(0);
	return 0;
}

UINT
Win32ThreadEx::Finalize()
{
	bRunning_ = false;
	iHandle_ = NULL;
	return 0;
}

bool
Win32ThreadEx::IsTerminating() const
{
	return bTerminate_ || bShutdown;
}

bool
Win32ThreadEx::IsRunning() const
{
	return bRunning_;
}

bool
Win32ThreadEx::Exit(U32 iExitCode, U32 iCheckCount, U32 iCheckInterval)
{
	if (!Terminate(iCheckCount, iCheckInterval))
		return Kill(iExitCode);

	return true;
}

bool
Win32ThreadEx::Terminate(U32 iCheckCount, U32 iCheckInterval)
{
	bTerminate_ = true;

	for (U32 iCheck = 0; iCheck < iCheckCount; ++iCheck)
	{
		Sleep(iCheckInterval);

		if (!IsRunning())
			return true;
	}

	return false;
}

bool
Win32ThreadEx::Kill(U32 iExitCode)
{
	if (iHandle_ == 0)	return true;
	bool bTerminated = ::TerminateThread((HANDLE) iHandle_, iExitCode) != 0;

	if (bTerminated)	iHandle_ = 0;

	return bTerminated;
}

#endif//_J9_USE_PTHREAD