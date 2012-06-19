// $Id: UtilityWin32.cpp 4461 2009-11-17 06:36:28Z younghwan.oh $

#include "stdafx.h"
#include "UtilityWin32.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shlobj.h>
#pragma comment(lib, "version.lib")
#include <errno.h>

#include "Application.h"
#include "UtilityFile.h"
#include "UtilityString.h"

using namespace J9;

#ifdef J9_WIN32

U32
GetVersionInfo(const J9::StringW& rFileName, VS_FIXEDFILEINFO& rInfo)
{
	DWORD	iDummy;
	U32	iInfoSize = ::GetFileVersionInfoSize(rFileName.c_str(), &iDummy);
	if (iInfoSize == 0) return GetLastError();

	std::vector<U8>	cBuffer(iInfoSize);
	if (!::GetFileVersionInfo(rFileName.c_str(), 0, iInfoSize, &cBuffer[0]))
	{
		return GetLastError();
	}

	void*	pBlock;
	UINT	iBlockSize;

	if (!::VerQueryValue(&cBuffer[0], _W("\\"), &pBlock, &iBlockSize))
		return 1;

	if (iBlockSize != sizeof(VS_FIXEDFILEINFO))
		return 2;

	rInfo = *_rc<VS_FIXEDFILEINFO*>(pBlock);
	return 0;
}

U32
Win32::FileUtil::GetFileVersion(const J9::StringW& rFileName, Version& rOutVersion)
{
	VS_FIXEDFILEINFO cInfo;
	U32 iResult = GetVersionInfo(rFileName, cInfo);
	if (iResult) return iResult;

	rOutVersion.SetMajorVersion(_sc<U16>((cInfo.dwFileVersionMS & 0xffff0000) >> 16));
	rOutVersion.SetMinorVersion(_sc<U16>(cInfo.dwFileVersionMS & 0x0000ffff));
	rOutVersion.SetProductVersion(_sc<U16>((cInfo.dwFileVersionLS & 0xffff0000) >> 16));
	rOutVersion.SetBulidVersion(_sc<U16>(cInfo.dwFileVersionLS & 0x0000ffff));
	return 0;
}

U32
Win32::FileUtil::GetProductVersion(const J9::StringW& rFileName, Version& rOutVersion)
{
	VS_FIXEDFILEINFO cInfo;
	U32 iResult = GetVersionInfo(rFileName, cInfo);
	if (iResult) return iResult;

	rOutVersion.SetMajorVersion(_sc<U16>((cInfo.dwProductVersionMS & 0xffff0000) >> 16));
	rOutVersion.SetMinorVersion(_sc<U16>(cInfo.dwProductVersionMS & 0x0000ffff));
	rOutVersion.SetProductVersion(_sc<U16>((cInfo.dwProductVersionLS & 0xffff0000) >> 16));
	rOutVersion.SetBulidVersion(_sc<U16>(cInfo.dwProductVersionLS & 0x0000ffff));
	return 0;
}

void
Win32::LogError(U32 iError, WChar* p)
{
	WChar	cBuf[BIG_NUMBER];
	_wcserror_s(cBuf, iError);
	LOG(p, cBuf);
}

void
Win32::GetErrorMessage(U32 iError, StringW& r)
{
	r.clear();

	WChar	pBuf[BIG_NUMBER];
	if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, iError, 0, pBuf, 4096, NULL))
	{
		r = pBuf;
	}
}

void
Win32::LoadResourceString(HINSTANCE hInstance, UINT iID, StringW& rString)
{
	rString.clear();

	enum
	{
		BufferLength = BIG_NUMBER,
	};
	WChar	pBuf[BufferLength];

	S32	iActualLength = LoadStringW(hInstance, iID, pBuf, BufferLength);

	rString.assign(pBuf, iActualLength);
}

extern int errno;

bool
Win32::CreateDirectory(const StringW& rFullPath)
{
	if (J9::FileUtil::CreateDirectory(rFullPath)) return true;
	if (EEXIST == errno) return true;
	return false;
}

StringW
Win32::Registry::GetRegKey(const WChar* pSubKey)
{
	StringW	sRegKey = _W("Software\\");

	StringW	sPublisher = pApp()->GetPublisherName();
	if (!sPublisher.empty())
	{
		sRegKey += sPublisher;
		sRegKey += _W("\\");
	}
	StringW	sCompany = pApp()->GetCompanyName();
	ASSERT(!sCompany.empty());
	if (!sCompany.empty())
	{
		sRegKey += sCompany;
		sRegKey += _W("\\");
	}
	StringW	sProduct = pApp()->GetProductName();
	ASSERT(!sProduct.empty());
	if (!sProduct.empty())
	{
		sRegKey += sProduct;
		sRegKey += _W("\\");
	}

	if (pSubKey != NULL && 0 < ::wcslen(pSubKey))
	{
		sRegKey += pSubKey;
	}

	return sRegKey;
}

bool
Win32::Registry::CreateKey(Root iRoot, const WChar* pSubKey, HKEY& rKey)
{
	HKEY	hRoot;
	switch (iRoot)
	{
	case eRootLocalMachine:
		hRoot = HKEY_LOCAL_MACHINE;
		break;
	case eRootCurrentUser:
		hRoot = HKEY_CURRENT_USER;
		break;
	case eRootClasses:
		hRoot = HKEY_CLASSES_ROOT;
		break;
	default:
		return false;
	}

	StringW	sSubKey = GetRegKey(pSubKey);
	DWORD	iDisposition;

	LONG	iRet = RegCreateKeyEx(hRoot, sSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &rKey, &iDisposition);

	if (iRet != ERROR_SUCCESS)
		return false;

	return true;
}

bool
Win32::Registry::OpenKey(Root iRoot, const WChar* pSubKey, HKEY& rKey)
{
	HKEY	hRoot;
	switch (iRoot)
	{
	case eRootLocalMachine:
		hRoot = HKEY_LOCAL_MACHINE;
		break;
	case eRootCurrentUser:
		hRoot = HKEY_CURRENT_USER;
		break;
	case eRootClasses:
		hRoot = HKEY_CLASSES_ROOT;
		break;
	default:
		return false;
	}

	StringW	sSubKey = GetRegKey(pSubKey);

	LONG	iRet = RegOpenKeyEx(hRoot, sSubKey.c_str(), 0, KEY_QUERY_VALUE, &rKey);

	if (iRet != ERROR_SUCCESS)
		return false;

	return true;
}

bool
Win32::Registry::DeleteKey(Root iRoot, const WChar* pSubKey)
{
	HKEY	hRoot;
	switch (iRoot)
	{
	case eRootLocalMachine:
		hRoot = HKEY_LOCAL_MACHINE;
		break;
	case eRootCurrentUser:
		hRoot = HKEY_CURRENT_USER;
		break;
	case eRootClasses:
		hRoot = HKEY_CLASSES_ROOT;
		break;
	default:
		return false;
	}

	StringW	sSubKey = GetRegKey(pSubKey);
	LONG	iRet = ::SHDeleteKey(hRoot, sSubKey.c_str());

	if (iRet != ERROR_SUCCESS)
		return false;

	return true;
}

bool
Win32::Registry::ReadValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32& rValue)
{
	HKEY	hKey;

	if (!OpenKey(iRoot, pSubKey, hKey))
		return false;

	DWORD	iType;
	DWORD	iRead;
	DWORD	iNumRead = sizeof(iRead);
	LONG	iRet = RegQueryValueEx(hKey, pName, 0, &iType, _rc<BYTE*>(&iRead), &iNumRead);

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iType != REG_DWORD)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iNumRead != sizeof(iRead))
	{
		::RegCloseKey(hKey);
		return false;
	}

	rValue = iRead;
	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::ReadValue(Root iRoot, const WChar* pSubKey, const WChar* pName, StringW& rValue)
{
	HKEY	hKey;

	if (!OpenKey(iRoot, pSubKey, hKey))
		return false;

	DWORD	iType;
	DWORD	iNeed = 0;
	LONG	iRet = RegQueryValueEx(hKey, pName, 0, &iType, NULL, &iNeed);

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iType != REG_SZ)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iNeed == 0)
	{
		// nothing to read
		rValue.clear();
		::RegCloseKey(hKey);
		return true;
	}

	std::vector<U8>		cBuffer(iNeed);
	DWORD	iNumRead = _sc<DWORD>(cBuffer.size());
	iRet = RegQueryValueEx(hKey, pName, 0, &iType, &cBuffer[0], &iNumRead);

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iType != REG_SZ)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iNumRead != iNeed)
	{
		::RegCloseKey(hKey);
		return false;
	}

	rValue = _rc<const WChar*>(&cBuffer[0]);
	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::WriteValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32 iValue)
{
	HKEY	hKey;

	if (!CreateKey(iRoot, pSubKey, hKey))
		return false;

	LONG	iRet = RegSetValueEx(hKey, pName, 0, REG_DWORD, _rc<const BYTE*>(&iValue), sizeof(iValue));

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::WriteValue(Root iRoot, const WChar* pSubKey, const WChar* pName, const StringW& rValue)
{
	HKEY	hKey;

	if (!CreateKey(iRoot, pSubKey, hKey))
		return false;

	LONG	iRet = RegSetValueEx(hKey, pName, 0, REG_SZ, _rc<const BYTE*>(rValue.c_str()), _sc<DWORD>(GetBufferSize(rValue)));

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::CreateArbitraryKey(Root iRoot, const WChar* pSubKey, HKEY& rKey)
{
	HKEY	hRoot;
	switch (iRoot)
	{
	case eRootLocalMachine:
		hRoot = HKEY_LOCAL_MACHINE;
		break;
	case eRootCurrentUser:
		hRoot = HKEY_CURRENT_USER;
		break;
	case eRootClasses:
		hRoot = HKEY_CLASSES_ROOT;
		break;
	default:
		return false;
	}

	DWORD	iDisposition;

	LONG	iRet = RegCreateKeyEx(hRoot, pSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &rKey, &iDisposition);

	if (iRet != ERROR_SUCCESS)
		return false;

	return true;
}

bool
Win32::Registry::OpenArbitraryKey(Root iRoot, const WChar* pSubKey, HKEY& rKey)
{
	HKEY	hRoot;
	switch (iRoot)
	{
	case eRootLocalMachine:
		hRoot = HKEY_LOCAL_MACHINE;
		break;
	case eRootCurrentUser:
		hRoot = HKEY_CURRENT_USER;
		break;
	case eRootClasses:
		hRoot = HKEY_CLASSES_ROOT;
		break;
	default:
		return false;
	}

	LONG	iRet = RegOpenKeyEx(hRoot, pSubKey, 0, KEY_QUERY_VALUE, &rKey);

	if (iRet != ERROR_SUCCESS)
		return false;

	return true;
}

bool
Win32::Registry::DeleteArbitraryKey(Root iRoot, const WChar* pSubKey)
{
	HKEY	hRoot;
	switch (iRoot)
	{
	case eRootLocalMachine:
		hRoot = HKEY_LOCAL_MACHINE;
		break;
	case eRootCurrentUser:
		hRoot = HKEY_CURRENT_USER;
		break;
	case eRootClasses:
		hRoot = HKEY_CLASSES_ROOT;
		break;
	default:
		return false;
	}

	// 하위 디렉토리까지 지워버리는 위험한 코드이므로최대한 방어를 한다.
	if (pSubKey == NULL)							return false;
	if (::wcscmp(pSubKey, _W("")) == 0)				return false;
	if (::_wcsicmp(pSubKey, _W("software")) == 0)	return false;
	if (::_wcsicmp(pSubKey, _W("software\\")) == 0)	return false;

	LONG	iRet = ::SHDeleteKey(hRoot, pSubKey);

	if (iRet != ERROR_SUCCESS)
		return false;

	return true;
}

bool
Win32::Registry::ReadArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32& rValue)
{
	HKEY	hKey;

	if (!OpenArbitraryKey(iRoot, pSubKey, hKey))
		return false;

	DWORD	iType;
	DWORD	iRead;
	DWORD	iNumRead = sizeof(iRead);
	LONG	iRet = RegQueryValueEx(hKey, pName, 0, &iType, _rc<BYTE*>(&iRead), &iNumRead);

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iType != REG_DWORD)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iNumRead != sizeof(iRead))
	{
		::RegCloseKey(hKey);
		return false;
	}

	rValue = iRead;
	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::ReadArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, StringW& rValue)
{
	HKEY	hKey;

	if (!OpenArbitraryKey(iRoot, pSubKey, hKey))
		return false;

	DWORD	iType;
	DWORD	iNeed = 0;
	LONG	iRet = RegQueryValueEx(hKey, pName, 0, &iType, NULL, &iNeed);

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iType != REG_SZ)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iNeed == 0)
	{
		// nothing to read
		rValue.clear();
		::RegCloseKey(hKey);
		return true;
	}

	std::vector<U8>		cBuffer(iNeed);
	DWORD	iNumRead = _sc<DWORD>(cBuffer.size());
	iRet = RegQueryValueEx(hKey, pName, 0, &iType, &cBuffer[0], &iNumRead);

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iType != REG_SZ)
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (iNumRead != iNeed)
	{
		::RegCloseKey(hKey);
		return false;
	}

	rValue = _rc<const WChar*>(&cBuffer[0]);
	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::WriteArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, U32 iValue)
{
	HKEY	hKey;

	if (!CreateArbitraryKey(iRoot, pSubKey, hKey))
		return false;

	LONG	iRet = RegSetValueEx(hKey, pName, 0, REG_DWORD, _rc<const BYTE*>(&iValue), sizeof(iValue));

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	::RegCloseKey(hKey);
	return true;
}

bool
Win32::Registry::WriteArbitraryValue(Root iRoot, const WChar* pSubKey, const WChar* pName, const StringW& rValue)
{
	HKEY	hKey;

	if (!CreateArbitraryKey(iRoot, pSubKey, hKey))
		return false;

	LONG	iRet = RegSetValueEx(hKey, pName, 0, REG_SZ, _rc<const BYTE*>(rValue.c_str()), _sc<DWORD>(GetBufferSize(rValue)));

	if (iRet != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return false;
	}

	::RegCloseKey(hKey);
	return true;
}

StringW
Win32::SpecialFolder::Get(Type iType)
{
	int			iFolder;
	bool		bValid = false;

	switch (iType)
	{
	case eCommonDesktopDirectory:
		iFolder = CSIDL_COMMON_DESKTOPDIRECTORY;
		bValid = true;
		break;
	case eCommonPrograms:
		iFolder = CSIDL_COMMON_PROGRAMS;
		bValid = true;
		break;
	case eCommonStartMenu:
		iFolder = CSIDL_COMMON_STARTMENU;
		bValid = true;
		break;
	case eCommonStartup:
		iFolder = CSIDL_COMMON_STARTUP;
		bValid = true;
		break;
	case eCommonDocuments:
		iFolder = CSIDL_COMMON_DOCUMENTS;
		bValid = true;
		break;
	case eDesktopDirectory:
		iFolder = CSIDL_DESKTOPDIRECTORY;
		bValid = true;
		break;
	case ePrograms:
		iFolder = CSIDL_PROGRAMS;
		bValid = true;
		break;
	case eStartMenu:
		iFolder = CSIDL_STARTMENU;
		bValid = true;
		break;
	case eStartup:
		iFolder = CSIDL_STARTUP;
		bValid = true;
		break;
	case ePersonal:
		iFolder = CSIDL_PERSONAL;
		bValid = true;
		break;
	default:
		ASSERT(0);
		iFolder = -1;
		bValid = false;
		break;
	}

	if (!bValid)
		return _W("");

	WChar	sPath[MAX_PATH];
	if (!::SHGetSpecialFolderPath(NULL, sPath, iFolder, FALSE))
		return _W("");

	StringW	sRet = sPath;
	return sRet;
}
#endif J9_WIN32
