// $Id: UtilityFile.h 8019 2011-02-09 06:44:41Z hyojin.lee $

#ifndef __J9_UTILITY_FILE_H__
#define __J9_UTILITY_FILE_H__

#include <Commdlg.h>
#include "StringTraits.h"

namespace J9
{
	template<typename _Type>
	class FileTraits
	{
	};

	template<>
	class FileTraits<AChar>
	{
	public:
		typedef AChar							Type;
		typedef std::basic_string<AChar>		String;
		typedef _finddata_t						FindData;

	public:
		static bool			IsDotDir(const String& rPath)	{ return IsDotDir(rPath.c_str()); }
		static bool			IsDotDir(const Type* pPath)		{ return StringTraits<Type>::Compare(pPath, ".") || StringTraits<Type>::Compare(pPath, ".."); }
		static bool			IsSVNDir(const String& rPath)	{ return IsSVNDir(rPath.c_str()); }
		static bool			IsSVNDir(const Type* pPath)		{ return StringTraits<Type>::Compare(pPath, ".svn") || StringTraits<Type>::Compare(pPath, "_svn"); }
		static intptr_t			FindFirst(const String& rPath, FindData& rData)	{ return ::_findfirst(rPath.c_str(), &rData); }
		static intptr_t		FindNext(intptr_t pHandle, FindData& rData)		{ return ::_findnext(pHandle, &rData); }
	};

	template<>
	class FileTraits<WChar>
	{
	public:
		typedef WChar							Type;
		typedef std::basic_string<WChar>		String;
		typedef _wfinddata_t					FindData;

	public:
		static bool			IsDotDir(const String& rPath)	{ return IsDotDir(rPath.c_str()); }
		static bool			IsDotDir(const Type* pPath)		{ return StringTraits<Type>::Compare(pPath, _W(".")) || StringTraits<Type>::Compare(pPath, _W("..")); }
		static bool			IsSVNDir(const String& rPath)	{ return IsSVNDir(rPath.c_str()); }
		static bool			IsSVNDir(const Type* pPath)		{ return StringTraits<Type>::Compare(pPath, _W(".svn")) || StringTraits<Type>::Compare(pPath, _W("_svn")); }
		static intptr_t			FindFirst(const String& rPath, FindData& rData)	{ return ::_wfindfirst(rPath.c_str(), &rData); }
		static intptr_t		FindNext(intptr_t pHandle, FindData& rData)		{ return ::_wfindnext(pHandle, &rData); }
	};

	namespace FileUtil
	{
		// FileUtil 로 옮기는게 맞는듯...
		inline U32 GetFileSize(const WChar* pFileName)
		{
			FILE *pFile = NULL;
			_wfopen_s(&pFile, pFileName, _W("rb"));
			ASSERT(pFile);
			fseek(pFile, 0, SEEK_END);
			U32 iSize = ftell(pFile);
			fclose(pFile);
			return iSize;
		}

		inline bool Rename(const WChar* pFilePath, const WChar* pNewFilePath)
		{
			return _wrename(pFilePath, pNewFilePath) == 0;
		}

		inline bool Rename(const AChar* pFilePath, const AChar* pNewFilePath)
		{
			return rename(pFilePath, pNewFilePath) == 0;
		}

		inline bool Access(const WChar* pFilePath)
		{
			return _waccess(pFilePath, 0) == 0;
		}		

		inline bool Access(const AChar* pFilePath)
		{
			return _access(pFilePath, 0) == 0;
		}

		inline bool Delete(const WChar* pFilePath)
		{
			if (_wunlink(pFilePath))
			{
				//LOG(_W(" file [%s] delete failed\r\n"), pFilePath);
				//DWORD i = GetLastError();
				//i = i;
				return false;
			}
			return true;
		}

		bool		CreateDirectory(const J9::StringW& rPath);
		bool		IsRelativeDir(const J9::StringW& rPath);

		J9::StringW	GetLastDir(const J9::StringW& rPath);
		J9::StringW	GetLastDir(const J9::StringW& rPath, const StringW& rDir);

		Size		GetFileNamePos(const J9::StringW& rPathName);
		Size		GetFileExtPos(const J9::StringW& rPathName);
		Size		GetFileExtPos(const J9::StringA& rPathName);
		U32			GetFileNum(const J9::StringW& rPath, bool bRecursive = false, bool bCountDir = false);
		J9::StringW	GetFileName(const J9::StringW& rPath);
		J9::StringW	GetFileNameWithoutExtension(const J9::StringW& rPath);
		J9::StringW	GetExtension(const J9::StringW& rName, bool bIncludeDot = false);
		J9::StringA	GetExtension(const J9::StringA& rName, bool bIncludeDot = false);
		J9::StringW	StripFileName(const J9::StringW& rPath, bool bIncludeSlash = true);
		J9::StringW	StripExtension(const J9::StringW& rName);

		bool		IsDotDir(const WChar* pName);
		bool		IsDotDir(const AChar* pName);
		bool		IsDotDir(const J9::StringW& rName);
		bool		IsSVNDir(const WChar* pName);
		bool		IsSVNDir(const AChar* pName);

		typedef std::vector<J9::StringW> FileNameVectorW;
		typedef std::vector<J9::StringA> FileNameVectorA;
		bool	GetFilePath(HWND hWnd, LPCWSTR pFilter, J9::StringW& rPath, FileNameVectorW& rNames, bool bOpenStyle, const J9::StringW& rInitPath);
		Size	FindFilePackage(const WChar* pFileName, FileNameVectorW& rFound, bool bRecursive = true, bool bWithPath = false);
		Size	FindFilePackage(const AChar* pFileName, FileNameVectorA& rFound, bool bRecursive = true, bool bWithPath = false);
		template<typename T>
		Size	FindFile(const T* pFileName, std::vector<typename FileTraits<T>::String>& rFound, bool bRecursive = true, bool bWithPath = false);
	};

	template<typename T>
	Size
	FileUtil::FindFile(const T* pFileName, std::vector<typename FileTraits<T>::String>& rFound, bool bRecursive, bool bWithPath)
	{
		Size	iFound = 0;

		StringTraits<T>::String	sNameWithPath = pFileName;
		J9::StringUtil::ReplaceAll<T>(sNameWithPath, StringTraits<T>::BackSlash(), StringTraits<T>::Slash());

		Size iPos = sNameWithPath.rfind(StringTraits<T>::Slash());
		StringTraits<T>::String	sPath(sNameWithPath, 0, iPos + 1);
		StringTraits<T>::String	sName(sNameWithPath, iPos + 1);

		StringTraits<T>::String	sToFind = sPath + StringTraits<T>::Multiply();
		FileTraits<T>::FindData	cData;
		// 주어진 파일 이름으로 찾으면 디렉토리이름이 주어진 이름과 맞지 않을 경우
		// 디렉토리를 찾지 못하고 recursive하게 찾을 수 없게 된다
		intptr_t pHandle = FileTraits<T>::FindFirst(sToFind, cData);

		if (pHandle == -1)
			return iFound;

		do
		{
			if (_A_SUBDIR & cData.attrib)
			{
				if (!FileTraits<T>::IsDotDir(cData.name) && !FileTraits<T>::IsSVNDir(cData.name))
				{
					if (bRecursive)
					{
						StringTraits<T>::String sSubPathAndName = sPath + cData.name + StringTraits<T>::Slash() + sName;
						iFound += FindFile(sSubPathAndName.c_str(), rFound, bRecursive, bWithPath);
					}
				}
			}
			else
			{
				if (StringUtil::CompareWildcardedString(sName.c_str(), cData.name))
				{
					StringTraits<T>::String	sFoundName;
					if (bWithPath)
					{
						sFoundName = sPath;
					}
					sFoundName += cData.name;

					rFound.push_back(sFoundName);
					++iFound;
				}
			}
		}
		while (FileTraits<T>::FindNext(pHandle, cData) == 0);

		_findclose(pHandle);
		return iFound;
	}

	J9::Ptr<J9::Buffer>	ReadFileToBuffer(_IOBase* pIO);
}

#endif//__J9_UTILITY_FILE_H__