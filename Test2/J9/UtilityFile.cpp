// $Id: UtilityFile.cpp 8032 2011-02-10 06:08:21Z hyojin.lee $

#include "Stdafx.h"
#include "UtilityFile.h"
#include "UtilityString.h"

#include <io.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <direct.h>
#include "Package.h"

using namespace J9;

J9::StringW
FileUtil::StripFileName(const J9::StringW& rPath, bool bIncludeSlash)
{
	return rPath.substr(0, GetFileNamePos(rPath) - (bIncludeSlash ? 0 : 1));
}

J9::StringW
FileUtil::GetFileName(const J9::StringW& rPath)
{
	return rPath.substr(GetFileNamePos(rPath));
}

J9::StringW
FileUtil::GetFileNameWithoutExtension(const J9::StringW& rPath)
{
	Size iPos = GetFileNamePos(rPath);
	return rPath.substr(iPos, GetFileExtPos(rPath) - iPos - 1);
}

J9::StringW
FileUtil::StripExtension(const J9::StringW& rName)
{
	return rName.substr(0, GetFileExtPos(rName) - 1);
}

J9::StringW
FileUtil::GetExtension(const J9::StringW& rName, bool bIncludeDot)
{
	Size iPos = GetFileExtPos(rName);
	if (iPos == J9::StringW::npos)
		return J9::StringW();

	return rName.substr(GetFileExtPos(rName) - (bIncludeDot ? 1 : 0));
}

J9::StringA
FileUtil::GetExtension(const J9::StringA& rName, bool bIncludeDot)
{
	Size iPos = GetFileExtPos(rName);
	if (iPos == J9::StringA::npos)
		return J9::StringA();

	return rName.substr(GetFileExtPos(rName) - (bIncludeDot ? 1 : 0));
}

bool
FileUtil::CreateDirectory(const J9::StringW& rPath)
{
	ASSERT(!rPath.empty());
	Size iPos = _sc<Size>(-1), iLen = rPath.length();
	while (iPos = rPath.find(_T("\\"), iPos + 1), (iPos >= 0 && iPos <= iLen))
	{
		if (iPos != 0)
			_wmkdir(rPath.substr(0, iPos).c_str());
	}
	return _wmkdir(rPath.c_str()) == 0;
}

bool
FileUtil::IsRelativeDir(const J9::StringW& rPath)
{
	ASSERT(!rPath.empty());
	return ::PathIsRelativeW(rPath.c_str()) ? true : false;
}

J9::StringW
FileUtil::GetLastDir(const J9::StringW& rPath, const J9::StringW& rDir)
{
	// 절대경로 "c:\x1\x2\x3\rDir\file.txt" 에서 "rDir\file.txt" 를 return 한다.
	ASSERT(!rPath.empty() && !rDir.empty());
	WChar cBuf[DEFAULT_NUMBER];
	wsprintfW(cBuf, _W("\\%s\\"), rDir.c_str());
	Size iPos = rPath.rfind(cBuf);
	return rPath.substr(iPos + 1, rPath.length() - iPos - 1);
}

J9::StringW
FileUtil::GetLastDir(const J9::StringW& rPath)
{
	// rPath must not contain file name -0-!
	Size iPos = rPath.rfind(_T('\\'));
	return rPath.substr(iPos + 1, rPath.length() - iPos - 1);
}

Size
FileUtil::GetFileNamePos(const J9::StringW& rPathName)
{
	Size	iPos = rPathName.rfind(_T('\\'));
	if (iPos == J9::StringW::npos)
		iPos = rPathName.rfind(_T('/'));

	// 경로 부분을 찾을 수 없다면 스트링 전체가 파일 이름일 것이고
	// String::npos 가 0xFFFFFFFF이므로
	// 이에 1을 더하면 0이 된다
	CASSERT(J9::StringW::npos == SIZE_MAX, OOPS);
	return iPos + 1;
}

Size
FileUtil::GetFileExtPos(const J9::StringW& rPathName)
{
	Size	iPos = rPathName.rfind(_W('.'));

	// 찾을 수 없다면 확장자가 없는 것이다
	if (iPos == J9::StringW::npos)
	{
		return iPos;
	}

	return iPos + 1;
}

Size
FileUtil::GetFileExtPos(const J9::StringA& rPathName)
{
	Size	iPos = rPathName.rfind('.');

	// 찾을 수 없다면 확장자가 없는 것이다
	if (iPos == String::npos)
	{
		return iPos;
	}

	return iPos + 1;
}

bool
FileUtil::IsDotDir(const J9::StringW& rName)
{
	return IsDotDir(rName.c_str());
}


bool
FileUtil::IsDotDir(const WChar* pName)
{
	return ::wcscmp(pName, _W(".")) == 0 || ::wcscmp(pName, _W("..")) == 0;
}

bool
FileUtil::IsDotDir(const AChar* pName)
{
	return ::strcmp(pName, ".") == 0 || ::strcmp(pName, "..") == 0;
}

bool
J9::FileUtil::IsSVNDir(const WChar* pName)
{
	return ::_wcsicmp(pName, _W(".svn")) == 0 || ::_wcsicmp(pName, _W("_svn")) == 0;
}

bool
J9::FileUtil::IsSVNDir(const AChar* pName)
{
	return ::_stricmp(pName, ".svn") == 0 || ::_stricmp(pName, "_svn") == 0;
}

U32
FileUtil::GetFileNum(const StringW& rFullPath, bool bRecursive, bool bCountDir)
{
	WChar sBuffer[DEFAULT_NUMBER];
	if (!_wgetcwd(sBuffer, countof(sBuffer))) return U32_MAX;
	if (_wchdir(rFullPath.c_str()) != 0) return U32_MAX;

	_wfinddata_t cData;
	intptr_t pHandle = ::_wfindfirst(_W("*.*"), &cData);
	if (pHandle == -1)
	{
		_wchdir(sBuffer);
		return U32_MAX;
	}

	U32 iNum = 0;
	do
	{
		if (_A_SUBDIR & cData.attrib)
		{
			if (!FileTraits<WChar>::IsDotDir(cData.name))
			{
				if (bRecursive)
				{
					iNum += GetFileNum(rFullPath + _W("\\") + cData.name, bRecursive, bCountDir);
				}
				if (bCountDir)
				{
					++iNum;
				}
			}
		}
		else
		{
			++iNum;
		}
	} while (_wfindnext(pHandle, &cData) == 0);

	if (_findclose(pHandle) != 0)
	{
		_wchdir(sBuffer);
		return U32_MAX;
	}

	if (_wchdir(sBuffer) != 0) return U32_MAX;

	return iNum;
}

bool
FileUtil::GetFilePath(HWND hWnd, LPCWSTR pFilter, StringW& rPath, FileNameVectorW& rNames, bool bOpenStyle, const StringW& rInitPath)
{
	rNames.clear();

	OPENFILENAME	cOFN;
	WChar			sTemp[65536] = _W("");

	ZeroMemory(&cOFN, sizeof(OPENFILENAME));
	cOFN.lStructSize	= sizeof(OPENFILENAME);
	cOFN.hwndOwner		= hWnd;
	cOFN.lpstrFilter	= pFilter;
	cOFN.lpstrFile		= sTemp;
	cOFN.nMaxFile		= 65536;
	cOFN.Flags			= OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	cOFN.lpstrInitialDir= rInitPath.c_str();

	if (bOpenStyle)
	{
		if (GetOpenFileName(&cOFN) != 0)
		{
			WChar*	p = sTemp;
			WChar	sName[MAX_PATH];
			lstrcpyW(sName, p);

			if (*(p + lstrlenW(sName) + 1) == 0)
			{
				rPath = StringUtil::Format(_W("%s"), sTemp);
				rNames.push_back(rPath.substr(cOFN.nFileOffset));
				rPath = rPath.substr(0, cOFN.nFileOffset);
			}
			else
			{
				rPath = StringUtil::Format(_W("%s\\"), sTemp);
				while (true)
				{
					p = p + lstrlenW(sName) + 1;
					if (0 == *p)
						break;
					rNames.push_back(StringUtil::Format(_W("%s"), p));
				}
			}

			return true;
		}
	}
	else
	{
		cOFN.nFilterIndex = 1;

		if (GetSaveFileName(&cOFN) != 0)
		{
			rPath = StringUtil::Format(_W("%s"), sTemp);
			rNames.push_back(rPath.substr(cOFN.nFileOffset));
			rPath = rPath.substr(0, cOFN.nFileOffset);

			return true;
		}
	}

	return false;
}

Size
FileUtil::FindFilePackage(const WChar* pFileName, FileNameVectorW& rFound, bool bRecursive , bool bWithPath )
{
	J9::StringW sNameWithPath = pFileName;
	J9::StringUtil::ReplaceAll<WChar>(sNameWithPath, _W("/"), _W("\\"));

	Size iFound;
	iFound = FindFile(sNameWithPath.c_str(), rFound, bRecursive, bWithPath);
	
	if (J9::PackageManager::HasInstance())
	{
		iFound += pPkgMgr->FindFile(sNameWithPath, rFound, bRecursive, bWithPath);
	}
	return iFound;
}

Size
FileUtil::FindFilePackage(const AChar* pFileName, FileNameVectorA& rFound, bool bRecursive , bool bWithPath)
{
	J9::StringA	sNameWithPath = pFileName;
	J9::StringUtil::ReplaceAll<AChar>(sNameWithPath, "/", "\\");

	Size iFound;
	iFound = FindFile(sNameWithPath.c_str(), rFound, bRecursive, bWithPath);

	if (J9::PackageManager::HasInstance())
	{
		iFound += pPkgMgr->FindFile(sNameWithPath, rFound, bRecursive, bWithPath);
	}
	return iFound;
}

J9::Ptr<J9::Buffer>
J9::ReadFileToBuffer(_IOBase* pIO)
{
	if (!pIO->IsOpened())
	{
		// 일단 열려 있어야 읽는다
		ASSERT(0);
		return NULL;
	}

	Size	iFileSize = pIO->GetSize();
	if (iFileSize == 0)
	{
		// 크기가 0인 파일?
		ASSERT(0);
		return NULL;
	}

	J9::Ptr<J9::Buffer>	pBuffer(ALLOC, iFileSize + 8);
	//if (pIO->IsDirectFile())
	//{
	//	// 직접 읽을 수 있는 파일을 왜 버퍼로 가져오려고 할까?
	//	LOG(_W("reading direct file to buffer\r\n"));
	//}
	pIO->Read(iFileSize, pBuffer->GetWritePointer());
	return pBuffer;
}