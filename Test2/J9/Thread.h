// $Id: Thread.h 7208 2010-08-25 04:55:41Z kyuhun.lee $

#ifndef __J9_THREAD_H__
#define __J9_THREAD_H__

#include "Mutex.h"

namespace J9
{
#ifdef _J9_USE_PTHREAD
	////////////////////////////////////////////////////////////////////////////////
	// PTW32_Init
	////////////////////////////////////////////////////////////////////////////////
	#ifdef PTW32_STATIC_LIB
		class PTW32_Init
		{
			STATIC_SINGLETON(PTW32_Init);

		public:
			PTW32_Init();
			~PTW32_Init();
		};
	#endif

	////////////////////////////////////////////////////////////////////////////////
	// PThread
	////////////////////////////////////////////////////////////////////////////////
	inline bool operator==(const pthread_t& lhs, const pthread_t& rhs)
	{
		return (lhs.p == rhs.p && lhs.x == rhs.x);
	}

	inline bool operator!=(const pthread_t& lhs, const pthread_t& rhs)
	{
		return (lhs.p != rhs.p || lhs.x != rhs.x);
	}

	class PThread
	{
	public:
		typedef PMutex	Lock;

	private:
		typedef void* (*Function)(void*);

	public:
		PThread(bool bScope = true, bool bDetached = false);
		virtual ~PThread();

		static void				Sleep(U32 iTime);
		static void				Setup();
		static void				SignalShutdown();
		static void				Shutdown();

		//static void*	Do(void* pArg);

		inline static void		Kill(pthread_t cId)		{ pthread_kill(cId, 0); }
		inline static void		Exit()					{ pthread_exit(0); }
		inline static pthread_t	GetThreadID()			{ return pthread_self(); }
		inline static void		Detach(pthread_t cId)	{ pthread_detach(cId); }
		inline static void		Join(pthread_t cId)		{ pthread_join(cId, 0); }
		inline static void		Cancel(pthread_t cId)	{ pthread_cancel(cId); }

		pthread_t GetID()					{ return cId_; }

		void	Start(Function pFunc, void* pArg = 0);
		//void	Begin(Function pFunc, void* pArg = 0);

	public:
		static stdext::hash_map<S32, PThread*>	cThreadsPool;
		static bool		bShutdown;
		static bool		bDoneShuttingdown;

	private:
		static Lock			cLock;
		pthread_t			cId_;
		pthread_attr_t		cAttr_;

		Function			pFunc_;
		void*				pArg_;
	};
#else//_J9_USE_PTHREAD
	class Win32Thread
	{
	public:
		typedef CriticalSection	Lock;

	private:
		typedef	unsigned int ThreadId;
		typedef unsigned int (__stdcall *Function)(void*);

	public:
		Win32Thread();
		virtual ~Win32Thread();

		static void		Sleep(U32 iTime);
		static void		Setup();
		static void		SignalShutdown();
		static void		Shutdown();

		static void		Kill(ThreadId cId, U32 iExitCode);
		static void		Exit();
		static ThreadId	GetThreadID();

		ThreadId		GetID();
		void			Start(Function pFunc, void* pArg = NULL);
		bool			Kill(U32 iExitCode = 0);

	public:
		static stdext::hash_map<S32, Win32Thread*>	cThreadsPool;
		static Lock									cLock;
		static bool									bShutdown;

	protected:
		ThreadId		iId_;
		uintptr_t		iHandle_;
		Function		pFunc_;
		void*			pArg_;
	};

	// �������� ������ �� ���ϰ� �ϱ� ���ؼ� ������� Ŭ����
	// ����ڴ� ������ �Լ��� �����ϴ� ���� �ƴ϶�
	// �� Ŭ������ ��ӹ޾� Initialzie(), Run(), Finalize() �Լ��� �����ؾ� �Ѵ�
	class Win32ThreadEx : public Win32Thread
	{
	public:
		Win32ThreadEx();
		virtual ~Win32ThreadEx();

		static UINT __stdcall	ThreadFunc(void* pArg);

		void			Start();
		// ���� ���Ḧ �õ��ϰ�, �������ᰡ ���� ���� �� ������ ���̴� �Լ�
		bool			Exit(U32 iExitCode = 0, U32 iCheckCount = 10, U32 iCheckInterval = 100);
		// �������Ḧ �õ��ϴ� �Լ�, ��������Ǹ� true, �ƴϸ� false�� ����
		bool			Terminate(U32 iCheckCount = 10, U32 iCheckInterval = 100);
		// �����带 ���� ���� ��Ű�� �Լ�
		bool			Kill(U32 iExitCode = 0);
		bool			IsRunning() const;

	protected:
		virtual UINT	Initialize();
		virtual UINT	RunLoop();
		virtual UINT	RunOnce();
		virtual UINT	Finalize();

		bool			IsTerminating() const;

	protected:
		int				iSleepTime_;
		volatile bool	bTerminate_;
		volatile bool	bRunning_;
	};

#endif//_J9_USE_PTHREAD

#ifdef _J9_USE_PTHREAD
	typedef PThread			Thread;
	typedef PThread			ThreadEx;
	#define	J9_THREAD_FUNC(FuncName, ArgName)	void* FuncName(void* ArgName)
	#define J9_THREAD_RETURN(value)	return (void*)(value)
#else//_J9_USE_PTHREAD
	typedef Win32Thread		Thread;
	typedef Win32ThreadEx	ThreadEx;
	#define	J9_THREAD_FUNC(FuncName, ArgName)	unsigned int __stdcall FuncName(void* ArgName)
	#define J9_THREAD_RETURN(value)	return (unsigned int)(value)
#endif//_J9_USE_PTHREAD
};

#endif//__J9_THREAD_H__
