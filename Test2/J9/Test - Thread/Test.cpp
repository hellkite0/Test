// $Id: Test.cpp 108 2008-01-24 02:34:36Z hyojin.lee $

#include "../Just9.h"

J9_THREAD_FUNC(Func, pArg)
{
	while(true)
	{
		::Sleep(1);
		::_tprintf(_rc<const Char*>(pArg));
	}

	J9_THREAD_RETURN(0);
}

int
_tmain(int argc, Char* argv[], Char* envp[])
{
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(envp);

	J9::Thread	cThread1;
	J9::Thread	cThread2;

	cThread1.Start(Func, _T("thread 1\n"));
	cThread2.Start(Func, _T("thread 2\n"));
	while(true)
	{
	}
}