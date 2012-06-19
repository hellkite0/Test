// $Id: Package.cpp 8959 2011-07-13 08:49:53Z hyojin.lee $

#include "Stdafx.h"
#include "Package.h"

#include <io.h>
#include <time.h>

#include "Crypt.h"
#include "File.h"
#include "UtilityFile.h"
#include "UtilityString.h"
#include "Any.h"
#include "Log.h"
#include "errno.h"
using namespace J9;

void
FileInfo::Debug(const J9::StringW& r)
{
	LOG(_W(" %s -> path(%s), size(%u), offset(%u), crc32(%u), cyrpt offset(%u), compressed(%u), crypted(%u)\r\n"),
		r.c_str(),
		sPath_.c_str(),
		iSize_, iOffset_,
		iCRC32_, iCryptOffset_,
		iCompressed_,
		bCrypted_);
}

char* pPassword = "just9nine co. ltd";

Package::Package()
: pCreateContents_(NULL), iContentsSize_(0), iRawContentsSize_(0), iHeaderSize_(0), bVerbose_(false)
{

}

Package::Package(const J9::StringW& rPath)
: pCreateContents_(NULL), iContentsSize_(0), sPath_(rPath), iRawContentsSize_(0), iHeaderSize_(0), bVerbose_(false)
{

}

void
Package::Reset()
{
	if (cFile_.IsOpened()) cFile_.Close();
	if (pCreateContents_)
	{
		free(pCreateContents_);
		pCreateContents_ = NULL;
	}
	iRawContentsSize_ = 0;
	//pContents_ = 0;
	iContentsSize_ = 0;
	sName_ = _W("");
	sPath_ = _W("");
	bVerbose_ = false;
	cFileList_.clear();
	cRawList_.clear();
}

Package::~Package()
{
	if (cFile_.IsOpened())
		cFile_.Close();
	if (pCreateContents_)
	{
		free(pCreateContents_);
		pCreateContents_ = NULL;
	}
}

void*
Package::ReadFile(const J9::StringW& rPath, U32* iSize, bool bAlign)
{
	J9::File cFile;
	if (!cFile.Open(rPath.c_str(), FileOpenRead))
		Throw<J9Error>(J9ECFileOpen);

	Size iFileSize = cFile.GetSize();
	Size iConvertedSize = iFileSize + (bAlign ? (8 - iFileSize % 8) : 0);

	void* pContent = calloc(1, iConvertedSize);

	Size iReadSize;
	if (iReadSize = iReadSize = cFile.Read(iConvertedSize, pContent), iReadSize != iFileSize)
	{
		LOG(_W(" ! Package::Read file [%s] failed to read\r\n"), rPath.c_str());
		free(pContent);
		return NULL;
	}

	*iSize = (U32) iFileSize;

	return pContent;
}

bool
Package::Write(const J9::StringW& rPath, void* pContent, U32 iSize)
{
	if (pContent == NULL)
	{
		LOG(_W(" ! Package::Write file [%s] nothing to write\r\n"), rPath.c_str());
		ASSERT(0);
		return false;
	}

	J9::File cFile;
	if (!cFile.Open(rPath.c_str(), FileOpenWrite | FileOpenCreate))
	{
		FileUtil::CreateDirectory(FileUtil::StripFileName(rPath));
		if (!cFile.Open(rPath.c_str(), FileOpenWrite | FileOpenCreate))
		{
			LOG(_W(" ! Package::Write file [%s] failed to open\r\n"), rPath.c_str());
			Throw<J9Error>(J9ECFileOpen);
			return false;
		}
	}

	if (cFile.Write(iSize, pContent) <= 0)
	{
		LOG(_W(" ! Package::Write file [%s] failed to write\r\n"), rPath.c_str());
		return false;
	}

	return true;
}

void
Package::Decrypt(void* pContent, U32 iSize)
{
	J9::Blowfish cBlowfish;
	cBlowfish.SetPassword(pPassword);
	cBlowfish.Decrypt(pContent, iSize);
}

void
Package::Encrypt(void* pContent, U32 iSize)
{
	J9::Blowfish cBlowfish;
	cBlowfish.SetPassword(pPassword);
	cBlowfish.Encrypt(pContent, iSize);
}

void*
Package::Compress(void* pContent, U32* iSize)
{
	U32 iLen = Compression::GetCompressBound(*iSize);
	void* pBuffer = calloc(1, iLen);
	if (!J9::Compression::Compress((U8*) pBuffer, &iLen, (U8*) pContent, *iSize))
	{
		free(pBuffer);
		return NULL;
	}

	memcpy(pContent, pBuffer, iLen);
	free(pBuffer);
	*iSize = iLen;
	return pContent;
}

void*
Package::Decompress(void* pContent, U32* iSize, U8 iRatio)
{
	U32 iDecompressedLen = *iSize * iRatio;
	void* pBuffer = calloc(1, iDecompressedLen);

	if (!J9::Compression::Decompress((U8*) pBuffer, &iDecompressedLen, (U8*) pContent, *iSize))
	{
		free(pBuffer);
		return NULL;
	}

	memcpy(pContent, pBuffer, iDecompressedLen);
	free(pBuffer);
	*iSize = iDecompressedLen;
	return pContent;
}

bool
Package::Add(const WChar* pName, void* pContent, U32 iSize, bool bCompress, bool bCrypt)
{
	J9::StringW sPath(pName);

	U32 iConvertedSize = iSize;

	if (bCompress && (pContent = Compress(pContent, &iConvertedSize), !pContent))
		return false;

	U8 iCryptOffset = 0;
	if (bCrypt)
	{
		if (iConvertedSize % 8 != 0) // need padding
		{
			iCryptOffset = 8 - _sc<U8>(iConvertedSize % 8);
			iConvertedSize += iCryptOffset;
		}
		Encrypt(pContent, iConvertedSize);
	}

	J9::Ptr<J9::FileInfo> pInfo(ALLOC);
	ASSERT(J9::FileUtil::IsRelativeDir(sPath));
	pInfo->sPath_ = StringUtil::ToLower(sPath);
	pInfo->iCompressed_ = bCompress ? _sc<U8>((iSize) / iConvertedSize + 1) : 0;
	pInfo->bCrypted_ = bCrypt;
	pInfo->iOffset_ = iContentsSize_;
	pInfo->iSize_ = iConvertedSize;
	pInfo->iCryptOffset_ = iCryptOffset;
	pInfo->iCRC32_ = J9::Compression::GetCRC32(pContent, iConvertedSize);

	//pInfo->Debug("Package::Add");
	pCreateContents_ = realloc(pCreateContents_, iContentsSize_ + iConvertedSize);  // realloc 을 하지 않고 한꺼번에 가지고 있는것으로 고친다.
	memcpy((char*) pCreateContents_ + iContentsSize_, pContent, iConvertedSize);
	iContentsSize_ += iConvertedSize;

	cFileList_.insert(stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::value_type(sPath, pInfo));
	//LOG(_W(" ! Package::Add file [%s] CRC32 (%u) checksum size %d/%d\r\n"), sPath.c_str(), pInfo->iCRC32_, pInfo->iSize_, iContentsSize_);
	return true;
}

bool
Package::HasFile(const WChar* pName)
{
	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator	cIter = cFileList_.find(pName);
	return cIter != cFileList_.end();
}

void
Package::AddFile(const WChar* pPath, bool bCompress, bool bCrypt)
{
	U32 iSize = 0;
	void* pContent = ReadFile(pPath, &iSize, bCrypt);
	if (pContent == NULL) return;
	J9::StringW sPath(pPath);
	if (!J9::FileUtil::IsRelativeDir(pPath))
	{
		ASSERT(!sPath_.empty());
		sPath = J9::FileUtil::GetLastDir(sPath, J9::FileUtil::GetLastDir(sPath_));
	}

	Add(sPath.c_str(), pContent, iSize, bCompress, bCrypt);
	free(pContent);
	if (bVerbose_) LOG(_W(" - file [%s] adding complete\r\n"), sPath.c_str());
}

J9::Ptr<Buffer>
Package::MakeHeader()
{
	J9::Ptr<J9::Buffer> pBuffer(ALLOC, GIGANTIC_NUMBER * 3);
	Size iSize = 0;

	U16 iNumFiles = _sc<U16>(cFileList_.size());
	ASSERT(iNumFiles <= U16_MAX);

	*pBuffer << iSize << iNumFiles;

	U16 iCount = 0;
	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator cIter, cEnd = cFileList_.end();
	for (cIter = cFileList_.begin(); cIter != cEnd; ++cIter)
	{
		FileInfo* p = cIter->second;
		*pBuffer << p->sPath_ << p->iSize_ << p->iOffset_ << p->iCRC32_ << p->iCryptOffset_ << p->iCompressed_ << p->bCrypted_;
		iCount++;
	}

	ASSERT(iCount == iNumFiles);

	U8 iCryptOffset = 8 - _sc<U8>((pBuffer->GetWriteOffset() - 4) % 8);	// 역시 blowfish 가 8byte 로 나누어 떨어져야해서 구려졌다
	for (U8 i = 0; i < iCryptOffset; ++i) pBuffer->Write(1, &iCryptOffset);
	iSize = pBuffer->GetWriteOffset();
	pBuffer->Write(sizeof(iSize), &iSize, 0);
	Encrypt((U8*) pBuffer->GetPointer(sizeof(iSize)), _sc<U32>(iSize - sizeof(iSize)));
	if (bVerbose_) LOG(_W(" - Package::MakeHeader size %u\r\n"), iSize);
	return pBuffer;
}

U32
Package::BreakHeader(U32 iSize)
{
	U32 iHeaderSize = 0;
	U32	iOffset = 0;
	const U8*	pBase = cFile_.OpenPartial(iOffset, sizeof(iHeaderSize));
	memcpy(&iHeaderSize, pBase + iOffset, sizeof(iHeaderSize));
	cFile_.ClosePartial(pBase);

	ASSERT((iHeaderSize - sizeof(iHeaderSize)) % 8 == 0);

	iContentsSize_ = iSize - iHeaderSize;

	iOffset = 0;
	pBase = cFile_.OpenPartial(iOffset, iHeaderSize);
	J9::Buffer cBuffer((U8*) pBase + iOffset, iHeaderSize);
	cFile_.ClosePartial(pBase);

	Decrypt((U8*) cBuffer.GetPointer(sizeof(iHeaderSize)), iHeaderSize - sizeof(iHeaderSize));
	U16 iNumFiles = 0;
	cBuffer >> iHeaderSize >> iNumFiles;

	U32 iContentsSize = 0;
	for (U16 i = 0; i < iNumFiles; ++i)
	{
		J9::Ptr<J9::FileInfo> p(ALLOC);
		cBuffer >> p->sPath_ >> p->iSize_ >> p->iOffset_ >> p->iCRC32_ >> p->iCryptOffset_ >> p->iCompressed_ >> p->bCrypted_;
		iContentsSize += p->iSize_;
		p->pPackage_ = this;
		cFileList_.insert(stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::value_type(p->sPath_, p));
		//LOG(_W(" - (%d) file %s:%u:%u:%u:%u:%u:%u in package [%s]\r\n"),
		//	i, p->sPath_.c_str(), p->iSize_, p->iOffset_, p->iCRC32_, p->iCryptOffset_, p->iCompressed_, p->bCrypted_, sName_.c_str());
	}

	ASSERT(iContentsSize == iContentsSize_);
	if (bVerbose_) LOG(_W(" - Package::BreakHeader size %u\r\n"), iHeaderSize);
	return iHeaderSize;
}

void
Package::Save(const WChar* pPath)
{
	J9::Ptr<Buffer> pHeader = MakeHeader();

	// pPackage = pHeader + pContents
	void* pPackage = malloc(iContentsSize_ + pHeader->GetWriteOffset());
	memcpy(pPackage, pHeader->GetPointer(0), pHeader->GetWriteOffset());
	memcpy((U8*) pPackage + pHeader->GetWriteOffset(), pCreateContents_, iContentsSize_);
	Write(pPath, pPackage, iContentsSize_ + pHeader->GetWriteOffset());
	free(pPackage);
}

bool
Package::_LoadHeader(const WChar* pPath)
{
	if (!cFile_.OpenMapping(pPath, 0)) return false;

	sPath_ = FileUtil::StripFileName(pPath); // path 필요없음...
	sName_ = J9::FileUtil::GetFileNameWithoutExtension(pPath);
	sPathName_ = pPath;

	Size iSize = cFile_.GetSize();
	iHeaderSize_ = BreakHeader(_sc<U32>(iSize));
	iContentsSize_ = _sc<U32>(iSize) - iHeaderSize_;

	LOG(_W(" - package [%s] _loading complete\r\n"), GetName().c_str());
	return true;
}

bool
Package::_Add(const WChar* pName, void* pContent, U32 iSize, bool bCompress, bool bCrypt)
{
	J9::StringW sPath(pName);

	U32 iConvertedSize = iSize;

	if (bCompress && (pContent = Compress(pContent, &iConvertedSize), !pContent))
		return false;

	U8 iCryptOffset = 0;
	if (bCrypt)
	{
		if (iConvertedSize % 8 != 0) // need padding
		{
			iCryptOffset = 8 - _sc<U8>(iConvertedSize % 8);
			iConvertedSize += iCryptOffset;
		}
		Encrypt(pContent, iConvertedSize);
	}

	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator	cIter = cFileList_.find(pName);
	J9::FileInfo*	pInfo = cIter == cFileList_.end() ? NULL : cIter->second;
	ASSERT(pInfo);

	pInfo->iCompressed_ = bCompress ? _sc<U8>((iSize) / iConvertedSize + 1) : 0;
	pInfo->iOffset_ = iContentsSize_;
	pInfo->iSize_ = iConvertedSize;
	pInfo->iCryptOffset_ = iCryptOffset;
	pInfo->iCRC32_ = J9::Compression::GetCRC32(pContent, iConvertedSize);

	ASSERT(iRawContentsSize_ >= iContentsSize_ + iConvertedSize);
	memcpy((char*) pCreateContents_ + iContentsSize_, pContent, iConvertedSize);
	iContentsSize_ += iConvertedSize;

	return true;
}

void
Package::PackList(const WChar* pDir, bool bCompress, bool bCrypt, bool bIncludeSubDir, bool bList)
{
	cRawList_.clear();
	_Pack(pDir, bCompress, bCrypt, bIncludeSubDir, bList);
	PackList();
}

void
Package::PackList()
{
	pCreateContents_ = ::realloc(pCreateContents_, iRawContentsSize_);
	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator cIter, cEnd = cRawList_.end();
	for (cIter = cRawList_.begin(); cIter != cEnd; ++cIter)
	{
		FileInfo* p = cIter->second;
		U32 iSize = 0;
		void* pContent = ReadFile(p->sAbsPath_.c_str(), &iSize, p->bCrypted_);
		if (pContent == NULL) return;
		J9::StringW sPath = p->sPath_;
		if (!J9::FileUtil::IsRelativeDir(p->sPath_.c_str()))
		{
			ASSERT(!sPath_.empty());
			sPath = J9::FileUtil::GetLastDir(sPath, J9::FileUtil::GetLastDir(sPath_));
		}
		_Add(sPath.c_str(), pContent, iSize, p->iCompressed_ > 0, p->bCrypted_);
		free(pContent);
		if (bVerbose_) LOG(_W(" - file [%s] adding complete\r\n"), sPath.c_str());
	}
	if (iRawContentsSize_ > iContentsSize_) pCreateContents_ = ::realloc(pCreateContents_, iContentsSize_);
}

void
Package::AddFileList(const WChar* pPath, bool bCompress, bool bCrypt)
{
	J9::File cFile;
	if (!cFile.Open(pPath, FileOpenRead))
		Throw<J9Error>(J9ECFileOpen);
	U32 iSize = _sc<U32>(cFile.GetSize());
	cFile.Close();

	J9::StringW sPath(pPath);
	J9::Ptr<J9::FileInfo> pInfo(ALLOC);

	pInfo->sAbsPath_ = pPath;
	pInfo->sAbsPath_ = StringUtil::ToLower(pInfo->sAbsPath_);

	if (!J9::FileUtil::IsRelativeDir(pPath))
	{
		ASSERT(!sPath_.empty());
		sPath = J9::FileUtil::GetLastDir(sPath, J9::FileUtil::GetLastDir(sPath_));
	}

	ASSERT(J9::FileUtil::IsRelativeDir(sPath));
	sPath = StringUtil::ToLower(sPath);
	pInfo->sPath_ = sPath;

	pInfo->iCompressed_ = bCompress ? 1 : 0;
	pInfo->bCrypted_ = bCrypt;
	pInfo->iOffset_ = 0;
	pInfo->iSize_ = iSize;
	pInfo->iCryptOffset_ = 0;
	pInfo->iCRC32_ = 0;

	cFileList_.insert(stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::value_type(sPath, pInfo));
	cRawList_.insert(stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::value_type(sPath, pInfo));

	iRawContentsSize_ += iSize;

	if (bCrypt)
	{
		U8 iCryptOffset = 0;
		if (iSize % 8 != 0) // need padding
		{
			iCryptOffset = 8 - _sc<U8>(iSize % 8);
		}
		iRawContentsSize_ += iCryptOffset;
	}

	//if (bVerbose_) LOG(_W(" - __file [%s : %d/%d] adding complete\r\n"), sPath.c_str(), iSize, iRawContentsSize_);
}

void
Package::_Pack(const WChar* pDir, bool bCompress, bool bCrypt, bool bIncludeSubDir, bool bList)
{
	intptr_t hFile;
	struct _wfinddata_t cFile;
	WChar cDir[DEFAULT_NUMBER];

	wsprintfW(cDir, _W("%s\\*.*"), pDir);
	if ((hFile = _wfindfirst((wchar_t*) cDir, &cFile )) == -1L )
	{
		switch (errno)
		{

		case ENOENT:
			LOG(_W("Not Found (%s) File\r\n"), pDir);
			break;
		case EINVAL:
			LOG(_W("invaild Path (%s) \r\n"), pDir);
			break;
		case ENOMEM:
			LOG(_W("Not enough Memory too long (%s) \r\n"), pDir);
			break;
		default:
			LOG(_W( "No files in current (%s) directory!\r\n"), pDir);
		}		
		return;
	}

	do {
		if (wcscmp(cFile.name, _W(".")) == 0) continue;
		if (wcscmp(cFile.name, _W("..")) == 0) continue;
		if (wcscmp(cFile.name, _W(".svn")) == 0) continue;

		if (cFile.attrib & _A_SUBDIR)
		{
			wsprintfW(cDir, _W("%s\\%s"), pDir, cFile.name);
			if (bIncludeSubDir)
			{
				if (bList) LOG(_W("%d,%d,%d,d,%s\r\n"), bCompress, bCrypt, !bIncludeSubDir, cDir);
				_Pack(cDir, bCompress, bCrypt, bIncludeSubDir, bList);
			}
		}
		else
		{			
			WChar cPath[DEFAULT_NUMBER];
			J9::StringW sKeyExtension = FileUtil::GetExtension(cFile.name, false).c_str();
			
			// jpg,png,bmp,tga확장명을 갖고 있을 경우 dds파일 존재 여부 확인 후 존재하면 처리하지 않는다.
			if (sKeyExtension.compare(_W("jpg")) == 0 || sKeyExtension.compare(_W("png")) == 0
				|| sKeyExtension.compare(_W("bmp")) == 0 || sKeyExtension.compare(_W("tga")) == 0)
			{
				J9::StringW sStripExtension = FileUtil::StripExtension(cFile.name);
				WChar cFullFilePath[DEFAULT_NUMBER];
				wsprintfW(cFullFilePath, _W("%s\\%s.dds"), pDir, sStripExtension.c_str());
				if (FileUtil::Access(cFullFilePath))
					continue;
			}
			wsprintfW(cPath, _W("%s\\%s"), pDir, cFile.name);
			if (!bList) AddFileList(cPath, bCompress, bCrypt);
			else LOG(_W("%d,%d,%d,f,%s\r\n"), bCompress, bCrypt, 0, cPath);
		}
	} while (_wfindnext(hFile, &cFile) == 0);
	_findclose(hFile);
}

StringW
Package::GetName() const
{
	return StringUtil::ToLower(sName_);
}

///////////////////////////////////////////////////////////////////
//
// Package Manager Class Implementation
//
//

DYNAMIC_SINGLETON_IMPL(PackageManager);

PackageManager::PackageManager()
{

}

PackageManager::~PackageManager()
{

}

void
PackageManager::Add(const J9::StringW& rName, J9::Ptr<Package> pPackage)
{
	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator cIter, cEnd = pPackage->cFileList_.end();
	for (cIter = pPackage->cFileList_.begin(); cIter != cEnd; ++cIter)
	{
		cFiles_.insert(stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::value_type(cIter->first, cIter->second));
		//LOG(_W(" ! PackageManager::Add File %s complete\r\n"), cIter->first.c_str());
	}

	cPackages_.insert(stdext::hash_map<StringW, J9::Ptr<J9::Package> >::value_type(rName, pPackage));
}

void
PackageManager::LoadPackages(const J9::StringW& rPath)
{
	#define PackageExtension _W(".pkg")
	intptr_t hFile;
	struct _wfinddata_t cFile;
	WChar cDir[DEFAULT_NUMBER];

	wsprintfW(cDir, _W("%s\\*%s"), rPath.c_str(), PackageExtension);
	if ((hFile = _wfindfirst((wchar_t*) cDir, &cFile )) == -1L )
	{
		LOG(_W( "No Package files in current (%s) directory!\r\n"), rPath.c_str());
		return;
	}

	do
	{
		WChar cPath[DEFAULT_NUMBER];
		wsprintfW(cPath, _W("%s\\%s"), rPath.c_str(), cFile.name);
		J9::Ptr<J9::Package> pPackage(ALLOC);

		if (pPackage->_LoadHeader(cPath))
		{
			Add(pPackage->GetName(), pPackage);
		}
	} while (_wfindnext(hFile, &cFile) == 0);
	_findclose(hFile);
}

J9::StringW
PackageManager::GetPackageName(const J9::StringW& rPath)
{
	// rPath must be replaced using GetReplacedName before this methos is invoked
	Size iPos = rPath.find(_W("\\"));
	if (iPos > 0) return rPath.substr(0, iPos);
	return rPath;
}

J9::StringW
PackageManager::GetReplacedName(const WChar* pPath)
{
	static J9::StringW sSlash(_W("/"));
	static J9::StringW sBackSlash(_W("\\"));

	J9::StringW sPath(pPath);
	StringUtil::ReplaceAll(sPath, sSlash, sBackSlash);
	return StringUtil::ToLower(sPath);
}

bool
PackageManager::HasFile(const WChar* pName)
{
	return cFiles_.find(GetReplacedName(pName)) != cFiles_.end();
}

FileInfo*
PackageManager::GetFile(const WChar* pName)
{
	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator	cIter = cFiles_.find(GetReplacedName(pName));
	FileInfo*	pInfo = cIter == cFiles_.end() ? NULL : cIter->second;

	if (!pInfo)
	{
		//LOG(_W(" ! PackageManager::GetFile failed to find [%s]\r\n"), pName);
		return NULL;
	}
	pInfo->OnOpen();
	return pInfo;
}

bool
PackageManager::GetFile(FileInfo* pInfo, void* pFile, U32* pSize)
{
	if (!pInfo)
	{
		LOG(_W(" ! Package::GetFile failed to find [%s]\r\n"), pInfo);
		return false;
	}
	memcpy(pFile, pInfo->pContent_, pInfo->iSize_);
	U32 iContentSize = pInfo->iSize_;

	if (pInfo->bCrypted_)
	{
		Package::Decrypt(pFile, iContentSize);
		iContentSize -= pInfo->iCryptOffset_;
	}

	if (pInfo->iCompressed_ > 0 && (pFile = Package::Decompress(pFile, &iContentSize, pInfo->iCompressed_), !pFile))
		return false;

	*pSize = iContentSize;

	return true;
}

Size
PackageManager::FindFile(const J9::StringW& rFileName, std::vector<J9::StringW>& rFound, bool bRecursive /* = false */, bool bWithPath)
{
	Size iFound = 0;
	stdext::hash_map<StringW, J9::Ptr<J9::FileInfo> >::iterator cIter, cEndIter = cFiles_.end();
	cIter = cFiles_.begin();

	J9::StringW sFileName = J9::StringUtil::ToLower(rFileName);
	Size iPos = sFileName.rfind(_W("\\"));
	J9::StringW sFindName = sFileName.substr(iPos + 1, sFileName.length() - iPos);
	J9::StringW sFindPath = sFileName.substr(0, iPos);

	bool bFound;
	while (cIter != cEndIter)
	{
		J9::StringW sTargetFullPath = cIter->first;
		Size iTargetPos = sTargetFullPath.rfind(_W("\\"));
		J9::StringW sTargetPath = sTargetFullPath.substr(0, iTargetPos);
		J9::StringW sTargetName = sTargetFullPath.substr(iTargetPos + 1, sTargetFullPath.length() - iTargetPos);
		bFound = false;
		if (bRecursive)
		{
			if (StringUtil::CompareWildcardedString(rFileName.c_str(), sTargetFullPath.c_str()))
				bFound = true;
		}
		else
		{
			if (sTargetPath.compare(sFindPath) == 0
				&& StringUtil::CompareWildcardedString(sFindName.c_str(), sTargetName.c_str()))
			{
				bFound = true;
			}
		}

		if (bFound)
		{
			J9::StringW sFoundFIle;
			if (bWithPath)
			{
				sFoundFIle = sTargetPath + _W("\\") + sTargetName;
			}
			else
			{
				sFoundFIle = sTargetName;				
			}

			std::vector<J9::StringW>::iterator cListIter = rFound.begin();
			std::vector<J9::StringW>::iterator cListEndIter = rFound.end();
			for (; cListIter != cListEndIter; ++cListIter)
			{
				if (sFoundFIle.compare(J9::StringUtil::ToLower(*cListIter)) == 0)
					break;
			}
			if (cListIter == cListEndIter)
				rFound.push_back(sFoundFIle);
		}
		++cIter;
	}
	return iFound;
}

Size
PackageManager::FindFile(const J9::StringA &rFileName, std::vector<J9::StringA> &rFound, bool bRecursive, bool bWithPath)
{
	std::vector<J9::StringW> cFindList;
	Size iSize = FindFile(J9::StringUtil::ToStringW(rFileName), cFindList, bRecursive, bWithPath);
	std::vector<J9::StringW>::iterator cIter = cFindList.begin();
	std::vector<J9::StringW>::iterator cEndIter = cFindList.end();
	
	for (;cIter != cEndIter; ++cIter)
	{
		J9::StringA sTemp = J9::StringUtil::ToStringA(*cIter);;

		std::vector<J9::StringA>::iterator cFoundIter = rFound.begin();
		std::vector<J9::StringA>::iterator cFoundEndIter = rFound.end();

		for (;cFoundIter != cFoundEndIter; ++cFoundIter)
		{
			if (sTemp.compare(J9::StringUtil::ToLower(*cFoundIter)) == 0)
				break;
		}
		if (cFoundIter == cFoundEndIter && !sTemp.empty())
			rFound.push_back(sTemp);
			
	}
	return iSize;

}

void
J9::FileInfo::OnOpen()
{
	iOffsetInView_ = pPackage_->iHeaderSize_ + iOffset_;
	const U8*	pBase = pPackage_->cFile_.OpenPartial(iOffsetInView_, iSize_);
	pContent_ = pBase + iOffsetInView_;
}

void
J9::FileInfo::OnClose()
{
	pPackage_->cFile_.ClosePartial(pContent_ - iOffsetInView_);
	pContent_ = NULL;
}