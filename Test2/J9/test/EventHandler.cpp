// $id$

#include "Stdafx.h"
#include "EventHandler.h"

#if 0
class A;
class B;
class C;
class D;

A*	pA = NULL;
B*	pB = NULL;
C*	pC = NULL;
D*	pD = NULL;

class A
{
public:
	virtual bool	F() = 0;
	virtual bool	G() = 0;
};

class B : public A
{
public:
	virtual bool	F();
	virtual bool	G();
};

class C : public B
{
public:
	virtual bool	F();
	virtual bool	G();
};

class D
{
public:
	virtual bool	F();
	virtual bool	G();
	virtual bool	H();
};

bool
B::F()
{
	LOG(_W("called B::F\n"));
	J9::Register(1236, pC, &A::F);
	return false;
}

bool
B::G()
{
	LOG(_W("called B::G\n"));
	J9::Unregister(1235, this);
	return false;
}

bool
C::F()
{
	LOG(_W("called C::F\n"));
	return false;
}

bool
C::G()
{
	LOG(_W("called C::G\n"));
	return false;
}

bool
D::F()
{
	LOG(_W("called D::F\n"));
	return false;
}

bool
D::G()
{
	LOG(_W("called D::G\n"));
	return false;
}

bool
D::H()
{
	LOG(_W("called D::H\n"));
	return false;
}

S32
EH::DoMain(int argc, TCHAR* argv[])
{
	J9::PostMan::CreateInstance();

	UNUSED(argc);
	UNUSED(argv);

	pB = new B;
	pC = new C;
	pD = new D;

	J9::Register(1234, pB, &A::F);
	J9::Register(1234, pC, &A::F);
	J9::Register(1234, pD, &D::F);

	J9::Register(1235, pB, &A::G);
	J9::Register(1235, pD, &D::G);

	J9::Register16(0, pD, &D::H);

	J9::Dispatch(1234, pB);
	J9::Dispatch(1235);
	J9::Dispatch(1235);

	delete pD;
	delete pC;
	delete pB;

	return 0;
}
#endif