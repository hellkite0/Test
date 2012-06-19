// $Id: Mutex.cpp 7174 2010-08-24 13:26:03Z junghyun.bae $

#include "StdAfx.h"
#include "Mutex.h"

#include "Thread.h"
#include "Log.h"
#include "Time.h"

using namespace J9;

#ifdef _J9_USE_PTHREAD
const char*								PMutex::pMutexName = "Mutex";
static stdext::hash_map<S32, PMutex*>*	pMutexPool = NULL;
static U32								iMutexCount = 0;
pthread_mutex_t							cMonitorMutex = PTHREAD_MUTEX_INITIALIZER;

PMutex::PMutex()
	: iDepth_(0), iDue_(0)
{
	pName_ = pMutexName;
	cId_ = pthread_t();
	pthread_mutex_init(&cMutex_, 0);
	Register();
}

PMutex::PMutex(const char* p)
	: iDepth_(0), iDue_(0)
{
	pName_ = _strdup(p);
	cId_ = pthread_t();
	pthread_mutex_init(&cMutex_, 0);
	Register();
}

PMutex::~PMutex()
{
	Unregister();
	pthread_mutex_destroy(&cMutex_);
	if (pName_ != pMutexName) free((void*) pName_);
}

void
PMutex::Register()
{
	pthread_mutex_lock(&cMonitorMutex);
	if (pMutexPool == NULL) pMutexPool = new stdext::hash_map<S32, PMutex*>;
	++iMutexCount;
	Size i = _rc<Size>(this);
	VERIFY(pMutexPool->insert(stdext::hash_map<S32, PMutex*>::value_type(_sc<S32>(i), this)).second == true);
	pthread_mutex_unlock(&cMonitorMutex);
}

void
PMutex::Unregister()
{
	pthread_mutex_lock(&cMonitorMutex);
	Size i = _rc<Size>(this);
	pMutexPool->erase(_sc<S32>(i));
	--iMutexCount;
	if (pMutexPool->size() == 0) delete pMutexPool;	// 너무 구리다 -_-;;;
    pthread_mutex_unlock(&cMonitorMutex);
}

void
PMutex::Setup()
{
	J9::PThread::Setup();
	J9::PThread * pThread = new J9::PThread();
	pThread->Start(Monitor);
}

#define MONITOR_INTERVAL 5000
#define LOCK_DELAY 3
#define LOCK_DEAD_LOCK 5

void PMutex::_Monitor()
{
	U32 iCount = 0;
	while (!PThread::bShutdown)
	{
		PThread::Sleep(MONITOR_INTERVAL);

		if (pMutexPool == NULL) continue;

		stdext::hash_map<S32, PMutex*>::iterator cIter, cEnd = pMutexPool->end();
		for (cIter = pMutexPool->begin(); cIter != cEnd; ++cIter)
		{
			PMutex* pMutex = cIter->second;

			if (!pMutex->cId_.x || ++pMutex->iDue_ < LOCK_DELAY) continue;

			if (pMutex->iDue_ > LOCK_DEAD_LOCK)
			{
				LOG(_W(" ! PMutex::Monitor probable dead lock detected for [%S:%d]\r\n"), pMutex->pName_, pMutex->iDue_);
				RaiseException(0, 0, 0, NULL);
			}

			LOG(_W(" ! PMutex::Monitor lock delay detected for [%S:%d]\r\n"), pMutex->pName_, pMutex->iDue_);
		}

		if (iCount++ % 360 == 0)
		{
			LOG(_W(" ! PMutex::Monitor watching [%d] locks\r\n"), iMutexCount);
		}
	}
}

void*
PMutex::Monitor(void* pArg)
{	
	__TRY();
	PMutex::_Monitor();
	__EXCEPT();
		LOG(_W(" ! server shutdown monitor thread by exception, run time = %02dh:%02dm:%02ds\r\n"), BootMSec / 1000 / 3600, BootMSec / 1000 % 3600 / 60, BootMSec / 1000 % 3600 % 60);		
		exit(0);
	__END_TRY_EXCEPT();

	return pArg;
}

void
PMutex::Lock()
{
	pthread_t cId = PThread::GetThreadID();
	if (cId_ == cId) ++iDepth_;
    else
	{
		S32 iErr = pthread_mutex_lock(&cMutex_);
		if (!iErr)
		{
			cId_ = cId;
			iDepth_ = 1;
		}
		else
		{
			LOG(_W(" mutex lock error occurred\r\n"));
			abort();
		}
    }
}

void
PMutex::Unlock()
{
	if (iDepth_ == 0)
	{
		// warn that unlocking non-locked
		LOG(_W(" trying to unlock never locked mutex\r\n"));
		RaiseException(0, 0, 0, NULL);
	}

	if (cId_ != PThread::GetThreadID())
	{
		// warn that unlocking from different thread
		LOG(_W(" trying to unlock in a different thread\r\n"));
		RaiseException(0, 0, 0, NULL);
	}

	if (iDepth_ > 1) --iDepth_;
    else
	{
		iDepth_ = 0;
		iDue_ = 0;
		cId_ = pthread_t();

		S32 iErr = pthread_mutex_unlock(&cMutex_);
		if (iErr)
		{
			// warn that unlocking error has occurred
			LOG(_W(" mutex unlock error occurred\r\n"));
			RaiseException(0, 0, 0, NULL);
		}
    }
}

///////////////////////////////////////////////////
//
// Conditional Mutex
//

ConditionalMutex::ConditionalMutex()
: PMutex()
{
    pthread_cond_init(&cCond_, NULL);
	ASSERT(cCond_);
}

ConditionalMutex::ConditionalMutex(const char* p)
: PMutex(p)
{
    pthread_cond_init(&cCond_, NULL);
	ASSERT(cCond_);
}

ConditionalMutex::~ConditionalMutex()
{
    pthread_cond_destroy(&cCond_);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		Conditional Signal 이 불릴때까지 무한정 Lock 안에서 기다린다.
///
/// \author		nevermind
/// \date		2005/06/23
////////////////////////////////////////////////////////////////////////////////
void
ConditionalMutex::Wait()
{
    iDepth_ = 0;
	cId_ = pthread_t();

	pthread_cond_wait(&cCond_, &cMutex_);

	iDepth_ = 1;
	iDue_ = 0;
	cId_ = PThread::GetThreadID();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		Wait 중인 Lock 에 Signal 을 주고 수행중인 함수를 계속 진행 시키도록 한다.
///
/// \author		nevermind
/// \date		2005/06/23
////////////////////////////////////////////////////////////////////////////////
void
ConditionalMutex::Signal()
{
    pthread_cond_signal(&cCond_);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		Wait중인 Lock들에게 모두 Signal 을 Broadcast 한다.
///
/// \author		nevermind
/// \date		2005/06/23
////////////////////////////////////////////////////////////////////////////////
void
ConditionalMutex::Broadcast()
{
	//LOGN4(_W(" ! mutex (%S) broadcast %08x, %08x\r\n"), pName_, &cCond_, &cMutex_);
    pthread_cond_broadcast(&cCond_);
}
#else//_J9_USE_PTHREAD
////////////////////////////////////////////////////////////////////////////////
// CriticalSection
////////////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&cCriticalSection_);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&cCriticalSection_);
}

void
CriticalSection::Lock()
{
	EnterCriticalSection(&cCriticalSection_);
}

void
CriticalSection::Unlock()
{
	LeaveCriticalSection(&cCriticalSection_);
}
#endif//_J9_USE_PTHREAD