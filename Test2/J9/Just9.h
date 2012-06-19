// $Id: Just9.h 7182 2010-08-24 16:22:02Z kyuhun.lee $

#ifndef __J9_JUST9_H__
#define __J9_JUST9_H__
////////////////////////////////////////////////////////////////////////////////
/// \file	Just9.h
/// \brief	외부 사용자에게 라이브러리를 제공하기 위한 헤더 파일
///
/// 외부 사용자에게 라이브러리를 제공하기 위한 헤더파일이다.
/// 외부 사용자는 이 일을 포함함으로서 Just9 Library의 모든 것을 사용할 수 있다
/// 개개의 파일만 포함하는 방법은 권장하지 않는다.
/// \note	이 파일은 라이브러리 내부에서 포함하지 않는다.
////////////////////////////////////////////////////////////////////////////////

#include "PreConfig.h"
#include "Config.h"

#include "Externals.h"

#include "CAssert.h"
#include "TypeDefs.h"
#include "Constants.h"
#include "CommonMacros.h"
#include "TypeList.h"
#include "TypeHelper.h"
#include "Exception.h"
#include "Assert.h"
#include "TypeTraits.h"
#include "UtilityLevel0.h"
#include "MemoryTracker.h"
#include "File.h"
#include "Buffer.h"
#include "HashFunc.h"
#include "String.h"
#include "UtilityString.h"
#include "Log.h"
#include "Ptr.h"
#include "Math.h"
#include "Numeric.h"
#include "Vector.h"
#include "Containers.h"
#include "Rect.h"

#include "Event.h"
#include "Object.h"

#include "Property.h"

#include "Random.h"
#include "PerfomanceCounter.h"
#include "UtilityMisc.h"
#include "UtilityMath.h"
#include "UtilityWin32.h"
#include "UtilityFile.h"
#include "UtilityRandom.h"
#include "UtilityType.h"

#include "Socket.h"
#include "UDPSocket.h"
#include "Thread.h"
#include "StringKey.h"
#include "Tokens.h"
#include "Mail.h"
#include "Time.h"

#include "XML.h"
#include "UtilityXML.h"
#include "Application.h"
#include "Language.h"

#include "Physics.h"
#include "Wave.h"

#include "Any.h"
#include "Option.h"
#include "Crypt.h"
#include "Package.h"
#include "Patch.h"
#include "GetOpt.h"

#include "BooleanExpression.h"

#include "FileWatcher.h"
#include "Hardware.h"
#include "SecureValue.h"

#include "J9BugTrapper.h"

#ifdef _DEBUG
	#if defined(_JUST9_MFC)
		#pragma comment(lib, "Just9MFCD.lib")
	#elif defined(_SERVER)
		#pragma comment(lib, "Just9SD.lib")
	#elif defined(FAST_DEBUG)
		#pragma comment(lib, "Just9FD.lib")
	#elif defined(SLOW_RELEASE)
		#pragma comment(lib, "Just9SR.lib")
	#else
		#pragma comment(lib, "Just9D.lib")
	#endif
#else
	#if defined (_JUST9_MFC)
		#pragma comment(lib, "Just9MFCR.lib")
	#elif defined (_SERVER)
		#pragma comment(lib, "Just9SR.lib")
	#elif defined(FAST_DEBUG)
		#pragma comment(lib, "Just9FD.lib")
	#elif defined(SLOW_RELEASE)
		#pragma comment(lib, "Just9SR.lib")
	#else
		#pragma comment(lib, "Just9.lib")
	#endif
#endif

#endif//__J9_JUST9_H__
