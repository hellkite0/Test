// $Id: PreConfig.h 7676 2011-01-17 11:36:45Z junghyun.bae $

////////////////////////////////////////////////////////////////////////////////
/// \file PreConfig.h
///	\brief	이 파일은 Just9/Config.h가 include되기 전에 먼저 include해야한다
///
/// 이 파일에서는 Just9 library의 설정에 필요한 상수들을 정의한다.
/// 이 파일을 include하고 Config.h를 include하기전에 사용자의 설정들을 정의할 수 있다.
/// 각 설정에 대한 정보는 Config.h를 보면 알 수 있다.
///	\author	hyojin.lee
///	\date	2005-06-13
////////////////////////////////////////////////////////////////////////////////
#ifndef __J9_PRE_CONFIG_H__
#define __J9_PRE_CONFIG_H__

#define _J9_ASSERT_OUTPUT_MESSAGE_BOX		0x00000001
#define _J9_ASSERT_OUTPUT_DEBUGOUT			0x00000002
#define _J9_ASSERT_OUTPUT_STDERR			0x00000004
#define _J9_ASSERT_OUTPUT_LOG				0x00000008
#define _J9_ASSERT_OUTPUT_KILL				0x00000010
#define _J9_ASSERT_OUTPUT_DUMP				0x00000020
#define _J9_ASSERT_OUTPUT_MAIL				0x00000040

#define _J9_ASSERT_DECO_FILE_LINE			0x00000001
#define _J9_ASSERT_DECO_EXPRESSION			0x00000002
#define _J9_ASSERT_DECO_FUNC_NAME			0x00000004
#define _J9_ASSERT_DECO_CALL_STACK			0x00000008

#define _J9_LOG_OUTPUT_STDOUT				0x00000001
#define _J9_LOG_OUTPUT_DEBUGOUT				0x00000002
#define _J9_LOG_OUTPUT_FILE					0x00000004
#define _J9_LOG_OUTPUT_DB					0x00000008
#define _J9_LOG_OUTPUT_MESSAGE_BOX			0x00000010
#define _J9_LOG_OUTPUT_DIALOG_WINDOW		0x00000020

#define _J9_LOG_DECO_TIME_STAMP				0x00000001

#define _J9_LOG_SPLIT_NONE					0x00000001
#define _J9_LOG_SPLIT_MONTHLY				0x00000002
#define _J9_LOG_SPLIT_DAILY					0x00000003
#define _J9_LOG_SPLIT_HOURLY				0x00000004

#define _J9_LOG_LEVEL_NOTHING				0x00000000
#define _J9_LOG_LEVEL_INFO					0x00000001
#define _J9_LOG_LEVEL_WARNING				0x00000002
#define _J9_LOG_LEVEL_ALERT					0x00000003
#define _J9_LOG_LEVEL_CRITICAL				0x00000004
#define _J9_LOG_LEVEL_EVERYTHING			(_J9_LOG_LEVEL_INFO | _J9_LOG_LEVEL_WARNING | _J9_LOG_LEVEL_ALERT | _J9_LOG_LEVEL_CRITICAL)

#endif//__J9_PRE_CONFIG_H__
