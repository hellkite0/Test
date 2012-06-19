#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <crtdbg.h>
#include <Windows.h>
#include <process.h>
#include <MMSystem.h>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <dos.h>
#include <limits>
#include <cfloat>

#include "Thread.h"

#define _CRTDBG_MAPALLOC
#define MAX_LENGTH 10
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

char *tzstr = "TZ=PST8PDT";

#pragma comment(lib, "winmm.lib")

#define MAX 10000000
#define MAX_THREAD 4

DWORD MT_TotalCount = 0;
HANDLE hMutex;

static int a = 0;
static int b = 0;
bool bThreadDone[MAX_THREAD] = {0, };

#ifdef _WIN64
#define MAXT 100
#else
#define MAXT 10
#endif

enum eTestEnum
{
	eOne	= 1 << 0,
	eTwo	= 1 << 1,
	eThree	= 1 << 2,
	eFour	= 1 << 3,
	eFive	= 1 << 4,
	eSix	= 1 << 5,
	eAll	= eOne | eTwo | eThree | eFour | eFive | eSix,
};

void IncreaseCount()
{
	WaitForSingleObject(hMutex, INFINITE);
	MT_TotalCount++;
	ReleaseMutex(hMutex);
}

unsigned int __stdcall
ThreadFunc(void* lpParams)
{
	unsigned int i = (DWORD)lpParams;
	unsigned int iCount = 0;
	printf("Thread%d Start.\n", i);
	while(iCount < MAX)
	{
		//printf("Thread %d: %d\n", i, iCount);
		//b++;
		WaitForSingleObject(hMutex, INFINITE);
		a++;
		ReleaseMutex(hMutex);
		iCount++;
		//Sleep(10);
	}

	printf("Thread%d End.\n", i);
	
	//_endthreadex(0);
	bThreadDone[i] = true;
	return 1;
}

class AA
{
public:
	typedef void (*Function)(void*);
	AA() {}
	~AA() {}

	void SetFunc(Function pFunc) { pFunc_ = pFunc; }
	void DoFunc() { pFunc_(NULL); }

	static unsigned int Func(void* pArgs) { printf("Function called\n"); return 0; }

	Function pFunc_;
};

void
Func(void* pArgs)
{
	printf("Function called\n");
}

void
ThreadTest()
{
	unsigned int iStart = ::timeGetTime();
	DWORD cntOfThread = 0;
	DWORD dwThreadID[MAX_THREAD];
	HANDLE hThread[MAX_THREAD];

	hMutex = CreateMutex(NULL, false, NULL);
	if (!hMutex)
		printf("Failed to create mutex\n");

	for (unsigned int i = 0; i < MAX_THREAD; ++i)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (LPVOID)i, 0, (unsigned*)&dwThreadID[i]);
	}


	WaitForMultipleObjects(MAX_THREAD, hThread, true, INFINITE);

	int cntOfEndThread = 0;
	while(cntOfEndThread < MAX_THREAD)
	{
		//Sleep(10);
		cntOfEndThread = 0;
		for (int i = 0; i < MAX_THREAD; ++i)
		{
			if (bThreadDone[i] == true)
				cntOfEndThread++;
		}
	}

	printf("All Thread complete.\n");

	for (unsigned int i = 0; i < MAX_THREAD; ++i)
	{
		CloseHandle(hThread[i]);
	}

	CloseHandle(hMutex);

	printf("%d, With %d thread:%u ms\n", a, MAX_THREAD, ::timeGetTime() - iStart);
}

void
NormalTest()
{
	unsigned int iStart = ::timeGetTime();

	unsigned int c = 0;
	unsigned int iCount = 0;
	for (int i = 0; i < MAX_THREAD; ++i)
	{
		while(iCount < MAX)
		{
			c++;
			iCount++;
		}
		iCount = 0;
	}
	
	printf("%d, With single thread:%u ms\n", c, MAX_THREAD, ::timeGetTime() - iStart);
}


class AT
{
public:
	AT() {}

	void Print() { iValue_++; printf("%d\n", iValue_); }
	static int iValue_;
};
int AT::iValue_ = 0;

bool
myComp(AT* pi, AT* pj) { return (pi->iValue_ > pj->iValue_); }

void
Parse()
{
	static int a = 0;

	a++;

	printf("%d \n", a);
}

class TT
{
public:
	TT(int a):a_(a) {}

	int a_;
};

bool
Checer()
{
	return 1 < 2;
}

union TestData
{
	struct 
	{
		char a;
		char b;
		char c;
		char d;
	} Data;

	int all;
};

bool
Checker()
{
	std::string sStr = "123";
	return (!sStr.empty());
}

class cData
{
public:
	cData() {}
	~cData() {}
private:
	float fValue_;
};

class cBase
{
public:
	cBase() { printf("cBase Allocated.\n"); }
	~cBase() { printf("cBase Deallocated.\n"); }
	void ValueUp() { ++iValue_; }
	void Print() { printf("%d \n", iValue_); }
private:
	int iValue_;
};

class cDerieved : protected cBase
{
public:
	cDerieved() { iValue2_ = 10; /*cDataClass_ = new cData();*/ printf("cDerieved Allocated.\n"); }
	~cDerieved() { /*delete cDataClass_;*/ printf("cDerieved Deallocated.\n"); }
private:
	int iValue2_;
	//cData* cDataClass_;
};

std::vector<cDerieved> cVec;

void
Dsafaf()
{
	cDerieved cD;


	cVec.push_back(cD);
}

void
CreateInstance(cBase** pBase)
{
	*pBase = new cBase();
}

bool
Check(int iValue, int iMask)
{
	return (iValue & iMask);
}

void
main(void)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//std::vector<AT*> cVector;

	//AT* pA = NULL;
	//for (int i = 0; i < 30; ++i)
	//{
	//	pA = new AT();
	//	pA->iValue_ = rand()%100;
	//	cVector.push_back(pA);
	//}

	//{
	//	std::vector<AT*>::iterator cIter = cVector.begin();
	//	for (; cIter != cVector.end(); ++cIter)
	//	{
	//		printf("%d ", (*cIter)->iValue_);
	//	}
	//	printf("\n");
	//}
	//

	//std::sort(cVector.begin(), cVector.end(), myComp);

	//{
	//	std::vector<AT*>::iterator cIter = cVector.begin();
	//	for (; cIter != cVector.end(); ++cIter)
	//	{
	//		printf("%d ", (*cIter)->iValue_);
	//	}
	//	printf("\n");
	//}

	//cBase* pBase = (cBase*)new cDerieved();

	//delete pBase;


	int a = 1;

	int day = 19;

	if (day % a == 0)
	{
		printf("test \n");
	}


	//printf("%s\n", cHostName);

	//Thread* pThread = new Thread();
	//pThread->Start(&A::Func, NULL);
	//pThread->DoFunc();
	//delete pThread;

	//ThreadTest();
	//NormalTest();
}