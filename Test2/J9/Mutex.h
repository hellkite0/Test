// $Id: Mutex.h 7174 2010-08-24 13:26:03Z junghyun.bae $

#ifndef __J9_MUTEX_H__
#define __J9_MUTEX_H__

namespace J9
{
#ifdef _J9_USE_PTHREAD
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Pthread Library �� �̿��� Mutex(Lock) Class Implementation
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
	/// \brief		Pthread Library �� �̿��� Conditional Mutex(Lock) Class Implementation
	///
	/// Mutex Class ���� Wait, Signal �׸��� Broadcast �Լ��� �߰��ȴ�.
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
	/// Lock �� ��� Return �ϱ� ���� Unlock �� �ؾ��Ѵ�.
	/// Lock Scope �ȿ� Return �� �ؾ��ϴ� ��찡 ���� ����µ�, Code �� ������������.
	/// ScopeLock ������ �̿��Ͽ�, �����ڿ��� Lock ��, �Ҹ��ڿ��� Unlock �� �ҷ�,
	/// ���ŷο� unlock -> return sequence �� ����ȭ �Ѵ�.
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
	/// �Ϲ������� ���ҽ��� lock�� ��� ��� return �ϱ� ���� �ش� ���ҽ��� unlock�ؾ��Ѵ�.
	/// �̸� �ڵ�ȭ �ϴ� Ŭ�����̴�.
	/// �����ڿ��� lock�ϰ� �Ҹ��ڿ��� unlock�Ѵ�.
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
