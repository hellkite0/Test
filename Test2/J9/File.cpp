// $Id: File.cpp 8980 2011-07-15 05:18:21Z donghyuk.lee $

#include "stdafx.h"
#include "File.h"
#include "Package.h"

#include "UtilityString.h"
#include "UtilityFile.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// StandardIO
////////////////////////////////////////////////////////////////////////////////
bool StandardIO::Open(const WChar* pName, U32 iFlag)
{
	if (p_)		Close();

	bool	bRead = (iFlag & FileOpenRead) != 0;
	bool	bWrite = (iFlag & FileOpenWrite) != 0;
	bool	bCreate = (iFlag & FileOpenCreate) != 0;
	bool	bNoTrunc = (iFlag & FileOpenNoTrunc) != 0;

	StringW		sFlag;

	if (!bRead && !bWrite) return false;
	else if (bRead && !bWrite)
	{
		if (bCreate || bNoTrunc) return false;
		sFlag = _W("rb");
	}
	else if (!bRead && bWrite)
	{
		if (!bCreate) return false;
		if (bNoTrunc) sFlag = _W("ab");
		else sFlag = _W("wb");
	}
	else
	{
		if (!bCreate && !bNoTrunc) sFlag = _W("r+b");
		else if (bCreate && !bNoTrunc) sFlag = _W("w+b");
		else if (!bCreate && bNoTrunc) return false;
		else sFlag = _W("a+b");
	}
#if 0
	errno_t	iErr = ::_wfopen_s(&p_, pName, sFlag.c_str());
	return (iErr == 0) && (p_ != NULL);
#else
	int	iShareFlag = bWrite ? _SH_DENYWR : _SH_DENYRW;
	p_ = _wfsopen(pName, sFlag.c_str(), iShareFlag);
	return p_ != NULL;
#endif
}

bool StandardIO::Open(const AChar* pName, U32 iFlag)
{
	if (p_)		Close();

	bool	bRead = (iFlag & FileOpenRead) != 0;
	bool	bWrite = (iFlag & FileOpenWrite) != 0;
	bool	bCreate = (iFlag & FileOpenCreate) != 0;
	bool	bNoTrunc = (iFlag & FileOpenNoTrunc) != 0;

	StringA		sFlag;

	if (!bRead && !bWrite) return false;
	else if (bRead && !bWrite)
	{
		if (bCreate || bNoTrunc) return false;
		sFlag = "rb";
	}
	else if (!bRead && bWrite)
	{
		if (!bCreate) return false;
		if (bNoTrunc) sFlag = "ab";
		else sFlag = "wb";
	}
	else
	{
		if (!bCreate && !bNoTrunc) sFlag = "r+b";
		else if (bCreate && !bNoTrunc) sFlag = "w+b";
		else if (!bCreate && bNoTrunc) return false;
		else sFlag = "a+b";
	}
#if 0
	errno_t	iErr = ::fopen_s(&p_, pName, sFlag.c_str());
	return (iErr == 0) && (p_ != NULL);
#else
	int	iShareFlag = bWrite ? _SH_DENYWR : _SH_DENYRW;
	p_ = _fsopen(pName, sFlag.c_str(), iShareFlag);
	return p_ != NULL;
#endif
}

bool StandardIO::IsOpened() const
{
	return (p_ != NULL);
}

void
StandardIO::Close()
{
	if (IsOpened())
	{
		VERIFY(::fflush(p_) == 0);
		VERIFY(::fclose(p_) == 0);

		p_ = NULL;
	}
}

Size
StandardIO::Read(Size i, void* p)
{
	if (IsOpened())
		return ::fread(p, 1, i, p_);

	ASSERT(0);
	return 0;
}

Size
StandardIO::Write(Size i, const void* p)
{
	if (IsOpened())
		return ::fwrite(p, 1, i, p_);

	ASSERT(0);
	return 0;
}

void
StandardIO::Flush()
{
	if (!IsOpened())
		return;

	::fflush(p_);
}

void
StandardIO::Seek(FileSeekMethod iSeek, int iOffset)
{
	if (!IsOpened())
		return;

	S32	iOrigin;
	switch (iSeek)
	{
	case FileSeekBegin:
		iOrigin = SEEK_SET;
		break;

	case FileSeekCur:
		iOrigin = SEEK_CUR;
		break;

	case FileSeekEnd:
		iOrigin = SEEK_END;
		break;

	default:
		// default seek_cur
		ASSERT(0);
		iOrigin = SEEK_CUR;
		break;
	}

	VERIFY(::fseek(p_, _sc<long>(iOffset), iOrigin) == 0);
}

Size StandardIO::GetPos() const
{
	if (!IsOpened())
	{
		ASSERT(0);
		return 0;
	}

	fpos_t	iPos;

	VERIFY(::fgetpos(p_, &iPos) == 0);
	return _sc<Size>(iPos);
}

Size StandardIO::GetSize() const
{
	if (!IsOpened())
	{
		ASSERT(0);
		return 0;
	}

	Size	iSave, iSize;

	iSave = GetPos();

	_cc<StandardIO*>(this)->Seek(FileSeekEnd, 0);
	iSize = GetPos();

	_cc<StandardIO*>(this)->Seek(FileSeekBegin, iSave);

	return iSize;
}

bool
J9::StandardIO::IsDirectFile() const
{
	ASSERT(IsOpened());
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// MappedIO
////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
U32	MappedIO::iAllocationGranularity = U32_MAX;

MappedIO::MappedIO()
: hFile_(INVALID_HANDLE_VALUE),
hMapping_(INVALID_HANDLE_VALUE)
{
}

bool
MappedIO::OpenMapping(const WChar* pName, U32 iFlag)
{
	UNUSED(iFlag);

	CheckAllocationGranularity();

	// Create a memory mapped file for the master file
	hFile_ = ::CreateFileW(pName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if (hFile_ == NULL || hFile_ == (HANDLE) HFILE_ERROR)
	{
		Close();
		CHECK(false, J9::eCheckAlways, _W("패키지를 여는데 실패했습니다. 메모리가 부족할 수 있습니다. %s"), pName);
		return false;
	}

	hMapping_ = ::CreateFileMapping(hFile_, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMapping_ == NULL)
	{
		Close();
		CHECK(false, J9::eCheckAlways, _W("패키지를 여는데 실패했습니다. 메모리가 부족할 수 있습니다. %s"), pName);
		return false;
	}

	return true;
}

bool
MappedIO::OpenMapping(const AChar* pName, U32 iFlag)
{
	UNUSED(iFlag);

	CheckAllocationGranularity();

	// Create a memory mapped file for the master file
	hFile_ = ::CreateFileA(pName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if (hFile_ == NULL || hFile_ == (HANDLE) HFILE_ERROR)
	{
		Close();
		CHECK(false, J9::eCheckAlways, _W("패키지를 여는데 실패했습니다. 메모리가 부족할 수 있습니다. %S"), pName);
		return false;
	}

	hMapping_ = ::CreateFileMapping(hFile_, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMapping_ == NULL)
	{
		Close();
		CHECK(false, J9::eCheckAlways, _W("패키지를 여는데 실패했습니다. 메모리가 부족할 수 있습니다. %S"), pName);
		return false;
	}

	return true;
}


void
J9::MappedIO::CheckAllocationGranularity()
{
	if (iAllocationGranularity == U32_MAX)
	{
		// IsWow64Process API는 Windows Vista, Windows XP with SP2 이상에서 구현 돼 있다
		// XP SP2밑에선 동작하지 않는다

		BOOL	bWow64Process = FALSE;
		if (!IsWow64Process(GetCurrentProcess(), &bWow64Process))
		{
			CHECK(false, J9::eCheckAlways, _W("지원하지 않는 OS입니다. Windows XP SP2이상만 지원합니다."));
		}

		SYSTEM_INFO	cSystemInfo;
		if (bWow64Process)
		{
			GetNativeSystemInfo(&cSystemInfo);
		}
		else
		{
			GetSystemInfo(&cSystemInfo);
		}

		iAllocationGranularity = cSystemInfo.dwAllocationGranularity;
	}
}

static int	iMaxOpened = 0;

const U8*
J9::MappedIO::OpenPartial(U32& rOffset, U32 iSize)
{
	CheckAllocationGranularity();

	// Create a memory mapped file for the master file
	if (hFile_ == NULL)
	{
		// 부분적으로 열려면 이미 열어 놔야 한다
		ASSERT(0);
		return NULL;
	}

	if (hMapping_ == NULL)
	{
		// 부분적으로 열려면 이미 열어 놔야 한다
		ASSERT(0);
		return NULL;
	}

	U32	iBase = (rOffset / iAllocationGranularity) * iAllocationGranularity;
	U32	iSizeToMap = (rOffset - iBase) + iSize;
	const U8*	pView = _rc<const U8*>(::MapViewOfFile(hMapping_, FILE_MAP_READ, 0, iBase, iSizeToMap));
	if (pView == NULL)
	{
		Close();
		CHECK(false, J9::eCheckAlways, _W("패키지를 여는데 실패했습니다. 메모리가 부족할 수 있습니다."));
		return NULL;
	}
	rOffset -= iBase;
	pViews_.push_back(pView);
	if (iMaxOpened < _sc<int>(pViews_.size()))
	{
		iMaxOpened = pViews_.size();
		LOG(_W("%d map file view in single package.\r\n"), iMaxOpened);
	}
	return pView;
}

void
J9::MappedIO::ClosePartial(const U8* pView)
{
	FOR_ITER(std::vector<const U8*>, pViews_, cIter)
	{
		if (pView == *cIter)
		{
			::UnmapViewOfFile(*cIter);
			pViews_.erase(cIter);
			return;
		}
	}
}

bool
MappedIO::IsOpened() const
{
	return (hFile_ != INVALID_HANDLE_VALUE && hMapping_ != NULL) ? true : false;
}

Size
MappedIO::GetSize() const
{
	return ::GetFileSize(hFile_, NULL);
}

void
MappedIO::CloseMapping()
{
	if (hMapping_ != NULL && !pViews_.empty())
	{
		FOR_ITER(std::vector<const U8*>, pViews_, cIter)
		{
			::UnmapViewOfFile(*cIter);
		}
		pViews_.clear();
	}

	if (hMapping_ != NULL)
	{
		::CloseHandle(hMapping_);
		hMapping_ = NULL;
	}

	if (hFile_ != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile_);
		hFile_ = INVALID_HANDLE_VALUE;
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////
// PackageIO
////////////////////////////////////////////////////////////////////////////////

PackageIO::PackageIO()
: pFile_(NULL), pFileInfo_(NULL), pBase_(NULL), iPos_(0)
{
}

bool
PackageIO::Open(const WChar* pName, U32 iFlag)
{
	// 우선순위는 일단 파일에 둔다.
	if (OpenFile(pName, iFlag))
		return true;

	// 패키지 매니저가 없으면 더 동작할 필요가 없다
	if (!PackageManager::HasInstance())
		return false;

	pFileInfo_ = pPkgMgr->GetFile(pName);
	if (!pFileInfo_) return false;

	if (pFileInfo_->iCompressed_ > 0 || pFileInfo_->bCrypted_)
	{
		pBase_ = (U8*) ::malloc(pFileInfo_->iSize_ * (pFileInfo_->iCompressed_ > 0 ? pFileInfo_->iCompressed_ : 1));
		pPkgMgr->GetFile(pFileInfo_, pBase_, (U32*) &iSize_);
	}
	else
	{
		pBase_ = (U8*) pFileInfo_->pContent_;
		iSize_ = pFileInfo_->iSize_;
	}

	return true;
}

bool
PackageIO::Open(const AChar* pName, U32 iFlag)
{
	return Open(StringUtil::ToStringW(pName).c_str(), iFlag);
}

bool
PackageIO::IsOpened() const
{
	 return IsFileOpened() ? true : pBase_ != NULL;
}

void
PackageIO::Close()
{
	// direct file
	if (IsFileOpened())
		return CloseFile();

	// package file
	if (pFileInfo_)
	{
		if (pFileInfo_->iCompressed_ > 0 || pFileInfo_->bCrypted_)
			free(pBase_);

		pFileInfo_->OnClose();
	}

	pFileInfo_ = NULL;
	pBase_ = NULL;
}

Size
PackageIO::GetPos() const
{
	if (IsFileOpened()) return GetFilePos();
	return iPos_;
}

Size
PackageIO::GetSize() const
{
	if (IsFileOpened()) return GetFileSize();
	return iSize_;
}

Size
PackageIO::Read(Size i, void* p)
{
	if (IsFileOpened()) return ReadFile(i, p);

	if (pBase_ == NULL)
	{
		ASSERT(0);
		return false;
	}

	if (iPos_ >= GetSize())
	{
		ASSERT(0);
		return false;
	}

	// buf 크기 크면 size - iPos_ 만큼만 써준다...
	if (GetSize() < iPos_ + i)
	{
		//ASSERT(0);
		//return false;
		i = GetSize() - iPos_;
	}

	::memcpy(p, pBase_ + iPos_, i);
	iPos_ += i;

	return i;
}

void
PackageIO::Seek(FileSeekMethod iSeek, int iOffset)
{
	if (IsFileOpened()) return  SeekFile(iSeek, iOffset);
	else if (pBase_ == NULL)
	{
		ASSERT(0);
		return;
	}

	switch (iSeek)
	{
	case FileSeekBegin:
		break;

	case FileSeekCur:
		iOffset = iPos_ + iOffset;
		break;

	case FileSeekEnd:
		iOffset = GetSize() + iOffset;
		break;

	default:
		ASSERT(0);
		return;
	}

	iPos_ = iOffset;
}

bool
PackageIO::OpenFile(const WChar* pName, U32 iFlag)
{
	if (pFile_)	Close();

	bool	bRead = (iFlag & FileOpenRead) != 0;
	bool	bWrite = (iFlag & FileOpenWrite) != 0;
	bool	bCreate = (iFlag & FileOpenCreate) != 0;
	bool	bNoTrunc = (iFlag & FileOpenNoTrunc) != 0;

	WChar*	pFlag;

	if (!bRead && !bWrite) return false;
	else if (bRead && !bWrite)
	{
		if (bCreate || bNoTrunc) return false;
		pFlag = _W("rb");
	}
	else if (!bRead && bWrite)
	{
		if (!bCreate) return false;

		if (bNoTrunc) pFlag = _W("ab");
		else pFlag = _W("wb");
	}
	else
	{
		if (!bCreate && !bNoTrunc) pFlag = _W("r+b");
		else if (bCreate && !bNoTrunc) pFlag = _W("w+b");
		else if (!bCreate && bNoTrunc) return false;
		else pFlag = _W("a+b");
	}

	errno_t	iErr = ::_wfopen_s(&pFile_, pName, pFlag);
	return (iErr == 0) && (pFile_ != NULL);
}

bool
PackageIO::OpenFile(const AChar* pName, U32 iFlag)
{
	if (pFile_)	Close();

	bool	bRead = (iFlag & FileOpenRead) != 0;
	bool	bWrite = (iFlag & FileOpenWrite) != 0;
	bool	bCreate = (iFlag & FileOpenCreate) != 0;
	bool	bNoTrunc = (iFlag & FileOpenNoTrunc) != 0;

	AChar*	pFlag;

	if (!bRead && !bWrite) return false;
	else if (bRead && !bWrite)
	{
		if (bCreate || bNoTrunc) return false;
		pFlag = "rb";
	}
	else if (!bRead && bWrite)
	{
		if (!bCreate) return false;

		if (bNoTrunc) pFlag = "ab";
		else pFlag = "wb";
	}
	else
	{
		if (!bCreate && !bNoTrunc) pFlag = "r+b";
		else if (bCreate && !bNoTrunc) pFlag = "w+b";
		else if (!bCreate && bNoTrunc) return false;
		else pFlag = "a+b";
	}

	errno_t	iErr = ::fopen_s(&pFile_, pName, pFlag);
	return (iErr == 0) && (pFile_ != NULL);
}

const U8*
J9::PackageIO::GetPointer(Size iOffset) const
{
	if (IsFileOpened())
		return NULL;

	return pBase_ + iOffset;
}

bool
J9::PackageIO::IsDirectFile() const
{
	ASSERT(IsOpened());
	return IsFileOpened();
}

bool
FileOpenProcess_UnicodeText::DoPostProcess(_IOBase* pIO, U32 iFlag)
{
	static const U16	iSig = 0xfeff;
	//UTF-8 : EF BB BF 
	//UTF-16(little endian) : FE FF 
	//UTF-16(big endian) : FF FE
	if (pIO->GetSize() == 0 && iFlag & FileOpenWrite)
		pIO->Write(sizeof(iSig), &iSig);
	else if (iFlag & FileOpenRead)
	{
		U16	iRead;
		pIO->Read(sizeof(iRead), &iRead);
		// file is not unicode text file
		if (iRead != iSig)
		{
			pIO->Seek(FileSeekBegin, 0);
			CHECK(false, eCheckDebug, _W("Unicode Text 파일이 아닙니다."));
			return false;
		}
	}
	return true;
}
