// $Id: Just9Init.cpp 8979 2011-07-15 03:14:11Z hyojin.lee $

#include "Stdafx.h"
#include "Just9.h"

////////////////////////////////////////////////////////////////////////////////
/// \file	Just9Init.cpp
/// \brief	전역 객체를 초기화 하는 파일
///
/// Just9 Library에서 사용 및 제공하는 객체들을 초기화 하는 파일이다.
////////////////////////////////////////////////////////////////////////////////

#pragma warning(push)
#pragma warning(disable:4073)	// initializers put in library initialization area
#pragma init_seg(lib)

using namespace J9;

STATIC_SINGLETON_IMPL(MemoryTracker);

#ifdef _J9_USE_PTHREAD
	#ifdef PTW32_STATIC_LIB
	STATIC_SINGLETON_IMPL(PTW32_Init);
	#endif
#endif
Thread::Lock			Thread::cLock;

_KeyTable<WChar>				_KeyTable<WChar>::cTable;
Lock							_KeyTable<WChar>::cLock;
_KeyTable<WChar>::Strings		_KeyTable<WChar>::cStrings;
const std::basic_string<WChar>*	_KeyTable<WChar>::pEmpty = _KeyTable<WChar>::Find(_W(""));

_KeyTable<AChar>				_KeyTable<AChar>::cTable;
Lock							_KeyTable<AChar>::cLock;
_KeyTable<AChar>::Strings		_KeyTable<AChar>::cStrings;
const std::basic_string<AChar>*	_KeyTable<AChar>::pEmpty = _KeyTable<AChar>::Find("");

STATIC_SINGLETON_IMPL(COM);

#ifdef _J9_LOG
STATIC_SINGLETON_IMPL(_Log);
#endif

// random
U32					RandomFast::iSeed0;
RandomFast			RandomFast::cGlobal;
#ifndef J9_SOL_ENCRYPT_SOCKET_KEY
U32*				J9::pSocketKey;
#endif
#ifdef _SERVER
J9::NamedPool<J9::RecycledObjectQue> J9::RecycleObject::cRecycleBins;
#endif

DYNAMIC_SINGLETON_IMPL(Language);

DYNAMIC_SINGLETON_IMPL(Application);

#pragma warning(pop)
