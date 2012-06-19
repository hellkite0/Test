// $Id: UtilityWin32.h 4461 2009-11-17 06:36:28Z younghwan.oh $

#ifndef __J9_UTILITY_WIN32_H__
#define __J9_UTILITY_WIN32_H__

#include "Version.h"

namespace J9
{
#ifdef J9_WIN32
	namespace Win32
	{
		void	LogError(U32 iError, WChar* p);
		void	GetErrorMessage(U32 iError, StringW& r);
		void	LoadResourceString(HINSTANCE hInstance, UINT iID, StringW& rString);
		bool	CreateDirectory(const StringW& rFullPath);

		namespace FileUtil
		{
			U32		GetFileVersion(const J9::StringW& rFileName, Version& rOutVersion);
			U32		GetProductVersion(const J9::StringW& rFileName, Version& rOutVersion);
		};

		namespace Registry
		{
			// registry
			enum Root
			{
				eRootLocalMachine,
				eRootCurrentUser,
				eRootClasses,
			};

			// Software/Publisher/Company/Product의 SubKey로 읽고 쓰는 함수들
			StringW	GetRegKey(const WChar* pSubKey);
			bool	CreateKey(Root iRoot, const WChar* pSubKey, HKEY& rKey);
			bool	OpenKey(Root iRoot, const WChar* pSubKey, HKEY& rKey);
			bool	DeleteKey(Root iRoot, const WChar* pSubKey);
			bool	ReadValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32& rValue);
			bool	ReadValue(Root iRoot, const WChar* pSubKey, const WChar* pName, StringW& rValue);
			bool	WriteValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32 iValue);
			bool	WriteValue(Root iRoot, const WChar* pSubKey, const WChar* pName, const StringW& rValue);
			////////////////////////////////////////////////////////////////////////////////

			// 임의의 SubKey로 읽고 쓰는 함수들, 특별한 경우가 아니라면 사용하지 않는다.
			bool	CreateArbitraryKey(Root iRoot, const WChar* pSubKey, HKEY& rKey);
			bool	OpenArbitraryKey(Root iRoot, const WChar* pSubKey, HKEY& rKey);
			bool	DeleteArbitraryKey(Root iRoot, const WChar* pSubKey);
			bool	ReadArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32& rValue);
			bool	ReadArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, StringW& rValue);
			bool	WriteArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32 iValue);
			bool	WriteArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, const StringW& rValue);
			////////////////////////////////////////////////////////////////////////////////
		};

		namespace SpecialFolder
		{
			enum Type
			{
				eCommonDesktopDirectory,
				eCommonPrograms,
				eCommonStartMenu,
				eCommonStartup,
				eCommonDocuments,
				eDesktopDirectory,
				ePrograms,
				eStartMenu,
				eStartup,
				ePersonal,	// my documents
			};

			StringW	Get(Type iType);
		};
	};
#endif
};

#endif//__J9_UTILITY_WIN32_H__
