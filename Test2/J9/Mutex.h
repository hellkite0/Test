// $Id: Mutex.h 7174 2010-08-24 13:26:03Z junghyun.bae $

#ifndef __J9_MUTEX_H__
#define __J9_MUTEX_H__

namespace J9
{
#ifdef _J9_USE_PTHREAD
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Pthread Library 를 이용한 Mutex(Lock) Class Implementation
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
	class PMutex
	{
	public:
		static void		Setup();
		static void*	Monitor(void* pArg);
		static void		_Monitor();

		PMutex();
		PMutex(const char* p);
		virtual ~PMutex();

		void Lock();
		void Unlock();

		const char* GetName() { return pName_; }

	private:
		void		Register();
		void		Unregister();

	public:
		static const char* pMutexName;

	protected:
		U8 iDepth_;
		U8 iDue_;
		const char* pName_;

		pthread_t cId_;
		pthread_mutex_t	cMutex_;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Pthread Library 를 이용한 Conditional Mutex(Lock) Class Implementation
	///
	/// Mutex Class 에서 Wait, Signal 그리고 Broadcast 함수가 추가된다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
	class ConditionalMutex : public PMutex
	{
	public:
		ConditionalMutex();
		ConditionalMutex(const char* p);
		virtual ~ConditionalMutex();

		void		Wait();
		void		Signal();
		void		Broadcast();

	protected:
		pthread_cond_t	cCond_;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Scope Lock Class Implementation
	///
	/// Lock 을 잡고 Return 하기 전에 Unlock 을 해야한다.
	/// Lock Scope 안에 Return 을 해야하는 경우가 많이 생기는데, Code 가 지저분해진다.
	/// ScopeLock 변수를 이용하여, 생성자에서 Lock 을, 소멸자에서 Unlock 을 불러,
	/// 번거로운 unlock -> return sequence 를 간소화 한다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
	class ScopeLock
	{
	public:
		ScopeLock(PMutex* pMutex) : pMutex_(pMutex)	{ pMutex_->Lock(); }
		~ScopeLock()	{ pMutex_->Unlock(); }

	private:
		PMutex*		pMutex_;
	};

#else//_J9_USE_PTHREAD
	class CriticalSection
	{
	public:
		CriticalSection();
		virtual ~CriticalSection();

		static void			Setup();
		static void*		Monitor(void* pArg);

		void				Lock();
		bool				TryLock();
		void				Unlock();

	private:
		CRITICAL_SECTION	cCriticalSection_;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Scope Lock Class Implementation
	///
	/// 일반적으로 리소스를 lock한 경우 경우 return 하기 전에 해당 리소스를 unlock해야한다.
	/// 이를 자동화 하는 클래스이다.
	/// 생성자에서 lock하고 소멸자에서 unlock한다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
	class ScopeLock
	{
	public:
		ScopeLock(CriticalSection* pCriticalSection) : pCriticalSection_(pCriticalSection)	{ pCriticalSection_->Lock(); }
		virtual ~ScopeLock()	{ pCriticalSection_->Unlock(); }

	private:
		CriticalSection*		pCriticalSection_;
	};

#endif//_J9_USE_PTHREAD

#ifdef _J9_USE_PTHREAD
	typedef PMutex			Lock;
#else//_J9_USE_PTHREAD
	typedef CriticalSection	Lock;
#endif//_J9_USE_PTHREAD

}

#ifdef _MT
#define LOCK_SCOPE(x) J9::ScopeLock _lock(x);
#else
#define LOCK_SCOPE(x)
#endif

#endif // __J9_MUTEX_H__
