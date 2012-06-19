// $Id: Config.h 8959 2011-07-13 08:49:53Z hyojin.lee $

#ifndef __J9_CONFIG_H__
#define __J9_CONFIG_H__

#include <solconfig.h>

#ifndef __J9_PRE_CONFIG_H__
	#error You must include "PreConfig.h" and do your customizing setting before include this file.
#endif

////////////////////////////////////////////////////////////////////////////////
// visual studio version
////////////////////////////////////////////////////////////////////////////////
#if _MSC_VER != 1500
	#error : this lib is for VS2008
#endif

////////////////////////////////////////////////////////////////////////////////
// user config
////////////////////////////////////////////////////////////////////////////////
//#define _J9_LOG_DEFAULT_NAME	_W("Take9Client")

////////////////////////////////////////////////////////////////////////////////
// default config
////////////////////////////////////////////////////////////////////////////////
#ifndef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
		#define DETECT_MEMORY_LEAK 1
	#endif
#endif

////////////////////////////////////////////////////////////////////////////////
// win32
////////////////////////////////////////////////////////////////////////////////
#ifndef WINVER
	// windows 98 or windows 2000
	#define WINVER			0x500
#endif

#ifndef _WIN32_WINDOWS
	// windows 98
	#define _WIN32_WINDOWS	0x410
#endif

#ifndef _WIN32_WINNT
	// windows xp
	#define _WIN32_WINNT	0x0501
#endif

#ifdef _WIN32
	#define J9_WIN32
#endif

////////////////////////////////////////////////////////////////////////////////
// endian
////////////////////////////////////////////////////////////////////////////////
#define __LITTLE_ENDIAN__
//#define __BIG_ENDIAN__
#if defined(__BIG_ENDIAN__) && defined(__LITTLE_ENDIAN__)
#pragma error
#endif
#if !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#pragma error
#endif

////////////////////////////////////////////////////////////////////////////////
// 64bit
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN64
	#define _J9_64BIT
#else
	#define _J9_32BIT
#endif

#define _J9_CASSERT

// debug 모드인 경우 ASSERT를 활성화 한다
#if defined(_DEBUG) || defined(FAST_DEBUG)
	#define _J9_ASSERT
#endif

// debug 모드인 경우 log를 사용한다
#if defined(_DEBUG) || defined(FAST_DEBUG)
	#define _J9_LOG
#endif

#if !defined(_J9_PTR_DIAGNOSTIC)
	#ifdef _DEBUG
		//#define _J9_PTR_DIAGNOSTIC 1
	#endif
#endif

#define _PACKAGE

#ifdef _SERVER
	#ifndef HANDLE_EXCEPTION
		#define HANDLE_EXCEPTION
	#endif

	#undef _PACKAGE
	#ifndef _J9ASSERT
		#define _J9_ASSERT
	#endif
	#ifndef _J9_LOG
		#define _J9_LOG
	#endif
#endif

#if !defined(_J9_EVENT_DIAGNOSTIC)
	#ifdef _DEBUG
		#define _J9_EVENT_DIAGNOSTIC 1
	#endif
#endif

// disable some warning for convenience
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4127) // conditional expression if constant

#ifdef _JUST9_MFC
#pragma warning(disable:4100) // 'identifier' : unreferenced formal parameter
#endif

#include "svn_revision.h"

namespace J9
{

};

#endif//__J9_CONFIG_H__
