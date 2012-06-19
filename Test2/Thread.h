#pragma once
#include <stdio.h>

class Thread
{
public:
	typedef unsigned int (*Function)(void*);
	typedef void (*Func)(void*);

	Thread() {}
	~Thread() {}


	void Start(Function pFunc, void* pArgs) { pFunc_ = pFunc; pArgs_ = pArgs; }
	void Start2(Func pFunc2, void* pArgs){}

	void DoFunc() { pFunc_(pArgs_); }
private:
	Function	pFunc_;
	Func		pFunc2_;
	void*		pArgs_;
};