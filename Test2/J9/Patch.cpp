// $Id: Patch.cpp 4439 2009-11-16 03:56:27Z hyojin.lee $

#include "Stdafx.h"
#include "Patch.h"

#include "UtilityString.h"
#include "UtilityMisc.h"
#include "UtilityMath.h"
#include "UtilityFile.h"

using namespace J9;

class PatchGenerator
{
public:
	enum
	{
		// iState
		kBeforeStart = 0,
		kProcessOld,
		kCalcDelta,
		kDone
	};

	struct ProgressInfo
	{
		Size	iState;

		Size	iCurValue;
		Size	iMaxValue;

		union
		{
			struct
			{
				Size	iCurBytes;
				Size	iTotalBytes;
			}
			cProcessOld;

			struct
			{
				Size	iCurBytes;
				Size	iTotalBytes;

				Size	iReusedBytes;
				Size	iNotReusedBytes;
			}
			cCalcDelta;
		};
	};


protected:
	enum
	{
		eHashListSize = 3145739,
		eMaxBlockSize = 67108864,
		eBlockLength = 16,	// must be evenly divisible by 4
	};

	struct Node
	{
		Node*	pNext_;
	};

	struct HashNode
	{
		Node*	pNext_;
	};

public:
	PatchGenerator(const WChar* pOldFileName, const WChar* pNewFileName, const WChar* pPatchFileName);
	~PatchGenerator(void);

	void			CreatePatchFile(void);
	void			GetProgressInfo(ProgressInfo* pInfo);

private:
	void			FillListsFromOldFile(void);
	U32				CalcNextSum(U32 iSum, U8 iOut, U8 iIn);

	void			FindMatch(int iNewPos, int* opStart, int* opLength);
	int				GetSameBytesSize(int iNewPos, int iOldPos);

	void			EmitAdd(int iStart, int iLength);
	void			EmitCopy(int iStart, int iLength);

	Size			GetBlockOffset(Size i);
	static U32		CalcHash(const U8* p);


private:
	MappedFile		cOldFile_;
	const U8*		pOld_;
	S32				iOldFileSize_;

	MappedFile		cNewFile_;
	const U8*		pNew_;
	S32				iNewFileSize_;

	File			cPatchFile_;
	StringW			sPatchFileName_;

	HashNode		cHashes_[eHashListSize];

	S32				iOldBlockSize_;
	S32				iLinkListSize_;
	HANDLE			hLinkListFile_;
	HANDLE			hLinkListMapping_;
	Node*			pLink_;

	ProgressInfo	cProgressInfo_;
};

PatchGenerator::PatchGenerator(const WChar* pOldFileName, const WChar* pNewFileName, const WChar* pPatchFileName)
{
	ASSERT(pOldFileName != NULL);
	ASSERT(pNewFileName != NULL);
	ASSERT(pPatchFileName != NULL);

	// open files
	VERIFY(cOldFile_.Open(pOldFileName, FileOpenRead));
	pOld_ = cOldFile_.GetPointer();
	iOldFileSize_ = _sc<S32>(cOldFile_.GetSize());

	VERIFY(cNewFile_.Open(pNewFileName, FileOpenRead));
	pNew_ = cNewFile_.GetPointer();
	iNewFileSize_ = _sc<S32>(cNewFile_.GetSize());

	cPatchFile_.Open(pPatchFileName, FileOpenWrite | FileOpenCreate);
	sPatchFileName_ = pPatchFileName;

	// initialize hash list
	SetZero(cHashes_);

	// initialize link list
	iOldBlockSize_ = Max<S32>(0, iOldFileSize_ - eBlockLength + 1);
	iLinkListSize_ = Min<S32>(iOldBlockSize_, eMaxBlockSize);
	if (0 < iLinkListSize_)
	{
		WChar	sTempDir[MAX_PATH];
		WChar	sTempFileName[MAX_PATH];

		::GetTempPathW(MAX_PATH, sTempDir);
		::GetTempFileNameW(sTempDir, _W("Patch"), 0, sTempFileName);
		hLinkListFile_ = ::CreateFileW(sTempFileName,
			GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE, NULL);
		ASSERT(hLinkListFile_ != INVALID_HANDLE_VALUE);
		::SetFilePointer(hLinkListFile_, iLinkListSize_ * sizeof(Node), NULL, FILE_BEGIN);
		::SetEndOfFile(hLinkListFile_);
		hLinkListMapping_ = ::CreateFileMapping(hLinkListFile_, NULL, PAGE_READWRITE, 0, 0, NULL);
		ASSERT(hLinkListMapping_ != NULL);
		pLink_ = _rc<Node*>(::MapViewOfFile(hLinkListMapping_, FILE_MAP_ALL_ACCESS, 0, 0, 0));
		if (pLink_ == NULL)
		{
			DWORD	iError = ::GetLastError();
			iError = iError;
			ASSERT(0);
		}
		//ASSERT(pLink_ != NULL);
		::ZeroMemory(pLink_, sizeof(Node) * iLinkListSize_);
	}
	else
	{
		hLinkListFile_ = NULL;
		hLinkListMapping_ = NULL;
		pLink_ = NULL;
	}

	cProgressInfo_.iState = kBeforeStart;
	cProgressInfo_.iCurValue = 0;
	cProgressInfo_.iMaxValue = 0;
}


PatchGenerator::~PatchGenerator(void)
{
	if (pLink_ != NULL)
	{
		::UnmapViewOfFile((LPCVOID) pLink_);
		pLink_ = NULL;
	}

	if (hLinkListMapping_ != NULL)
	{
		::CloseHandle(hLinkListMapping_);
		hLinkListMapping_ = NULL;
	}

	if (hLinkListFile_ != NULL)
	{
		::CloseHandle(hLinkListFile_);
		hLinkListFile_ = INVALID_HANDLE_VALUE;
	}

	cPatchFile_.Close();
	if (cProgressInfo_.iState != kDone)
	{
		FileUtil::Delete(sPatchFileName_.c_str());
	}

	if (pNew_ != NULL)
		pNew_ = NULL;

	if (pOld_ != NULL)
		pOld_ = NULL;
}

U32
PatchGenerator::CalcHash(const U8* p)
{
	U32 iHash = 0x97072047;
	const U32* pSize = _rc<const U32*>(p);

	// 4 바이트씩 묶어서 계산
	for (Size i = 0; i < eBlockLength / sizeof(U32); ++i)
		iHash = _rotr(_sc<U32>(iHash ^ *pSize++), 5);

	// 4 바이트 미만의 나머지 부분
	const U8* pU8 = _rc<const U8*>(pSize);
	switch (eBlockLength & (sizeof(U32) - 1))
	{
	case 3: iHash ^= pU8[2] << 16 | pU8[1] << 8 | pU8[0]; break;
	case 2: iHash ^= pU8[1] << 8 | pU8[0]; break;
	case 1: iHash ^= pU8[0]; break;
	case 0: break;
	}

	return iHash % eHashListSize;
}

void
PatchGenerator::CreatePatchFile()
{
	int		iNewLength;
	int		newPos;
	int		addStart;
	int		copyStart;
	int		copyLength;

	// write new file size
	cPatchFile_ << iNewFileSize_;

	// preprocess new file
	cProgressInfo_.iState = kProcessOld;
	cProgressInfo_.iCurValue = 0;
	cProgressInfo_.iMaxValue = iOldFileSize_;
	cProgressInfo_.cProcessOld.iCurBytes = 0;
	cProgressInfo_.cProcessOld.iTotalBytes = iOldFileSize_;
	FillListsFromOldFile();

	// to the iStart
	cProgressInfo_.iState = kCalcDelta;
	cProgressInfo_.iCurValue = 0;
	cProgressInfo_.iMaxValue = iNewFileSize_;
	cProgressInfo_.cCalcDelta.iCurBytes = 0;
	cProgressInfo_.cCalcDelta.iTotalBytes = iNewFileSize_;
	cProgressInfo_.cCalcDelta.iReusedBytes = 0;
	cProgressInfo_.cCalcDelta.iNotReusedBytes = 0;

	iNewLength = iNewFileSize_;
	newPos = 0;
	addStart = 0;

	if (0 < iLinkListSize_)
	{
		while (newPos < iNewLength - eBlockLength)
		{
			FindMatch(newPos, &copyStart, &copyLength);
			if (eBlockLength <= copyLength)
			{
				// block move found
				if (addStart < newPos)
				{
					// emit pending add command
					EmitAdd(addStart, newPos - addStart);
					cProgressInfo_.iCurValue += (newPos - addStart);
					cProgressInfo_.cCalcDelta.iCurBytes += (newPos - addStart);
					cProgressInfo_.cCalcDelta.iNotReusedBytes += (newPos - addStart);
				}

				EmitCopy(copyStart, copyLength);
				cProgressInfo_.iCurValue += copyLength;
				cProgressInfo_.cCalcDelta.iCurBytes += copyLength;
				cProgressInfo_.cCalcDelta.iReusedBytes += copyLength;
				newPos += copyLength;
				addStart = newPos;
			}
			else
			{
				// mark for adding
				++newPos;
			}
		}
	}

	if (addStart < iNewLength)
	{
		EmitAdd(addStart, iNewLength - addStart);
		cProgressInfo_.iCurValue += (iNewLength - addStart);
		cProgressInfo_.cCalcDelta.iCurBytes += (iNewLength - addStart);
		cProgressInfo_.cCalcDelta.iNotReusedBytes += (iNewLength - addStart);
	}

	// write end iOpCode
	U8	iEndByte = 0;
	cPatchFile_ << iEndByte;

	cProgressInfo_.iState = kDone;
	cProgressInfo_.iCurValue = 0;
	cProgressInfo_.iMaxValue = 0;
}


void
PatchGenerator::GetProgressInfo(ProgressInfo* opInfo)
{
	ASSERT(opInfo != NULL);

	*opInfo = cProgressInfo_;
}


void
PatchGenerator::FillListsFromOldFile()
{
	Size	iMaxDepth = 0;
	Size	iSkipped = 0;
	Size	iBlock = 0;
	Size	iUsed = 0;
	if (0 < iLinkListSize_)
	{
		const U8*		pBlock;
		Size			iCount;
		Size			iIndex;
		U32				iHashValue;
		Node*			pCurNode;
		bool			bSkip;

		// read first block
		iIndex = 0;
		iCount = iLinkListSize_;
		pBlock = pOld_;
		iHashValue = CalcHash(pBlock);
		ASSERT(0 <= iHashValue && iHashValue < eHashListSize);
		do
		{
			Size	iDepth = 0;

			// bSkip if the same content already exists in the linked list
			pCurNode = cHashes_[iHashValue].pNext_;
			bSkip = false;
			while (pCurNode != NULL)
			{
				if (::memcmp(pBlock, pOld_ + GetBlockOffset(pCurNode - pLink_), eBlockLength) == 0)
				//if (::memcmp(pBlock, pOld_ + (pCurNode - pLink_), eBlockLength) == 0)
				{
					++iSkipped;

					bSkip = true;
					break;
				}
				++iDepth;
				pCurNode = pCurNode->pNext_;
			}

			if (iDepth > iMaxDepth)
			{
				iMaxDepth = iDepth;
				StringW	s = StringUtil::Format(_W("max depth : %d\r\n"), iDepth);
				::OutputDebugString(s.c_str());
			}

			// prepend to the list
			if (!bSkip)
			{
				pCurNode = &pLink_[iIndex];
				ASSERT(pCurNode->pNext_ == NULL);
				pCurNode->pNext_ = cHashes_[iHashValue].pNext_;
				cHashes_[iHashValue].pNext_ = pCurNode;

				if (pCurNode->pNext_ == NULL)
					++iUsed;

				++iBlock;
			}

			// calculate the next hash value
			//if (iIndex + 1 < iCount)
			//{
			//	iHashValue = CalcHash(pBlock + 1);
			//}

			// read next byte
			//iIndex++;
			//pBlock++;
			iIndex++;
			pBlock = pOld_ + GetBlockOffset(iIndex);
			if (iIndex < iCount)
				iHashValue = CalcHash(pBlock);

			cProgressInfo_.iCurValue = eBlockLength + iIndex;
			cProgressInfo_.cProcessOld.iCurBytes = eBlockLength + iIndex;

			if ((iIndex % 100000) == 0)
			{
				StringW	s = StringUtil::Format(_W("progressed %u, %u blocks, %u skipped %u used\r\n"), iIndex, iBlock, iSkipped, iUsed);
				::OutputDebugString(s.c_str());
			}
		}
		while (iIndex < iCount);
	}
}

void
PatchGenerator::FindMatch(int iNewPos, int* opStart, int* opLength)
{
	U32		iHashValue;
	S32		iStart;
	Node*	pCurNode;
	S32		iLength;

	ASSERT(opStart != NULL && opLength != NULL && 0 <= iNewPos && iNewPos < iNewFileSize_);

	*opStart = 0;
	*opLength = 0;

	iHashValue = CalcHash(pNew_ + iNewPos);
	pCurNode = cHashes_[iHashValue].pNext_;

	if (pCurNode == NULL)	return;

	iStart = _sc<S32>(GetBlockOffset(pCurNode - pLink_));
	//iStart = _sc<S32>(pCurNode - pLink_);

	while (pCurNode != NULL)
	{
		// determine if blocks are identical
		iLength = GetSameBytesSize(iNewPos, iStart);
		if (*opLength < iLength)
		{
			*opLength = iLength;
			*opStart = iStart;
			break;
		}

		pCurNode = pCurNode->pNext_;
		if (pCurNode != NULL)
		{
			iStart = _sc<S32>(GetBlockOffset(pCurNode - pLink_));
			//iStart = _sc<S32>(pCurNode - pLink_);
		}
	}
}


int
PatchGenerator::GetSameBytesSize(int iNewPos, int iOldPos)
{
	int	value;
	int	oldSize;
	int	newSize;
	int	oldPos;
	int	newPos;

	value = 0;
	oldSize = iOldFileSize_;
	newSize = iNewFileSize_;
	for (oldPos = iOldPos, newPos = iNewPos; oldPos < oldSize && newPos < newSize; oldPos++, newPos++)
	{
		if (pOld_[oldPos] == pNew_[newPos])
			value++;
		else
			break;
	}

	return value;
}


// 0ttnnnnn + additional bytes for iLength + bytes to add
// additional bytes are of iLength 0, 1, 2, 3 for tt 00, 01, 10, 11
void
PatchGenerator::EmitAdd(int iStart, int iLength)
{
	int		addByteCount;
	int		iIndex;
	static const int	k_aLen[] = { 0, 0x20, 0x2000, 0x200000, 0x20000000 };

	for (addByteCount = 0; addByteCount < 4; addByteCount++)
	{
		if (k_aLen[addByteCount] <= iLength && iLength < k_aLen[addByteCount + 1])
		{
			break;
		}
	}
	ASSERT(addByteCount < 4);

	U8 iCurByte;
	iCurByte = _sc<U8>(((addByteCount & 0x03) << 5) | (iLength & 0x0000001F));
	cPatchFile_ << iCurByte;
	for (iIndex = 0; iIndex < addByteCount; iIndex++)
	{
		iCurByte = _sc<U8>((((iLength >> 5) >> (iIndex * 8)) & 0x000000FF));
		cPatchFile_ << iCurByte;
	}

	cPatchFile_.Write(iLength, pNew_ + iStart);
}


// 1ttnnnnn + additional bytes for iLength and iStart
// nnnnn is the five lowest bits of the iLength
// tt = 00 -> two bytes for iStart offset follow
// tt = 01 -> two bytes for iStart offset and one more byte for iLength follow
// tt = 10 -> three bytes for iStart offset and one more byte for iLength follow
// tt = 11 -> four bytes for iStart offset and three more bytes for iLength follow
void
PatchGenerator::EmitCopy(int iStart, int iLength)
{
	int		type1;
	int		type2;
	int		iType;
	U8		iCurByte;
	static const DWORD	k_aStart[] = { 0, 0x00010000, 0x00010000, 0x01000000, 0xFFFFFFFF };
	static const DWORD	k_aLen[] = { 0, 0x20, 0x2000, 0x2000, 0x20000000 };

	for (type1 = 0; type1 < 4; type1++)
	{
		if (k_aStart[type1] <= (DWORD) iStart && (DWORD) iStart < k_aStart[type1 + 1])
		{
			break;
		}
	}
	ASSERT(type1 < 4);

	for (type2 = 0; type2 < 4; type2++)
	{
		if (k_aLen[type2] <= (DWORD) iLength && (DWORD) iLength < k_aLen[type2 + 1])
		{
			break;
		}
	}
	ASSERT(type2 < 4);

	// iOpCode
	iType = __max(type1, type2);
	iCurByte = _sc<U8>(0x80 | ((iType & 0x03) << 5) | (iLength & 0x0000001F));
	cPatchFile_ << iCurByte;

	// iStart offset
	iCurByte = _sc<U8>((iStart & 0x000000FF));
	cPatchFile_ << iCurByte;
	iCurByte = _sc<U8>(((iStart & 0x0000FF00) >> 8));
	cPatchFile_ << iCurByte;
	if (2 <= iType)
	{
		iCurByte = _sc<U8>(((iStart & 0x00FF0000) >> 16));
		cPatchFile_ << iCurByte;
	}
	if (3 <= iType)
	{
		iCurByte = _sc<U8>(((iStart & 0xFF000000) >> 24));
		cPatchFile_ << iCurByte;
	}

	// iLength
	if (1 <= iType)
	{
		iCurByte = _sc<U8>(((iLength >> 5) & 0x000000FF));
		cPatchFile_ << iCurByte;
	}
	if (3 <= iType)
	{
		iCurByte = _sc<U8>((((iLength >> 5) >> 8) & 0x000000FF));
		cPatchFile_ << iCurByte;
		iCurByte = _sc<U8>((((iLength >> 5) >> 16) & 0x000000FF));
		cPatchFile_ << iCurByte;
	}
}

Size
PatchGenerator::GetBlockOffset(Size i)
{
	ASSERT(iLinkListSize_ <= iOldBlockSize_);

	if (iLinkListSize_ == iOldBlockSize_)
		return i;

	U64	iReturn = _sc<U64>(iOldBlockSize_) * _sc<U64>(i) / _sc<U64>(iLinkListSize_);
	ASSERT(iReturn <= iOldBlockSize_);
	return _sc<Size>(iReturn);
}

bool
J9::Patch::GeneratePatch(const WChar* pOldFileName, const WChar* pNewFileName, const WChar* pPatchFileName)
{
	Ptr<PatchGenerator>	pPatchGenerator(ALLOC, pOldFileName, pNewFileName, pPatchFileName);
	pPatchGenerator->CreatePatchFile();

	return true;
}

struct ProgressInfo
{
	StringW	sDesc_;
	U32		iPercent_;
	StringW	sDetailedDesc_;
	U32		iDownloaded_;
};

bool
J9::Patch::ApplyPatch(const WChar* pOldFileName, const WChar* pPatchFileName, const WChar* pNewFileName)
{
	ASSERT(pOldFileName);
	ASSERT(pPatchFileName);
	ASSERT(pNewFileName);

	ProgressInfo	cProgressInfo;

	bool	isGood = true;
	File	cOldFile;
	File	cPatchFile;
	File	cNewFile;
	U32		iNewFileSize;
	U8		iOpCode;
	U8		iType;
	U8		iTemp;
	U32		iStart;
	U32		iLength;
	U8		pSmallBuffer[HUGE_NUMBER];

	// open files
	if (!cOldFile.Open(pOldFileName, FileOpenRead))	return false;
	if (!cPatchFile.Open(pPatchFileName, FileOpenRead))	return false;
	if (!cNewFile.Open(pNewFileName, FileOpenWrite | FileOpenCreate))	return false;

	// get new file size
	cPatchFile >> iNewFileSize;

	cProgressInfo.iPercent_ = 0;

	// scanning through delta file,
	do
	{
		// read iOpCode
		cPatchFile >> iOpCode;

		if (iOpCode == 0)
		{
			ASSERT(cPatchFile.GetPos() == cPatchFile.GetSize());
			if (cPatchFile.GetPos() != cPatchFile.GetSize())
			{
				isGood = false;
			}
			break;
		}
		else if (iOpCode & 0x80)
		{
			// copy
			iType = (iOpCode & 0x60) >> 5;
			iLength = iOpCode & 0x1F;

			cPatchFile >> iTemp;
			iStart = iTemp;

			cPatchFile >> iTemp;
			iStart |= _sc<U32>(iTemp) << 8;

			switch (iType)
			{
			case 0:
				break;

			case 1:
				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 5);
				break;

			case 2:
				cPatchFile >> iTemp;
				iStart |= (_sc<U32>(iTemp) << 16);

				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 5);
				break;

			case 3:
				cPatchFile >> iTemp;
				iStart |= (_sc<U32>(iTemp) << 16);
				cPatchFile >> iTemp;
				iStart |= (_sc<U32>(iTemp) << 24);

				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 5);
				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 13);
				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 21);
				break;

			default:
				ASSERT(false);
				break;
			}

			U32 iLeft = iLength;
			cOldFile.Seek(FileSeekBegin, iStart);
			while (iLeft) //** 여기서 계속 돈다.
			{
				S32	iReadLen;

				if (iLeft > HUGE_NUMBER)
					iReadLen = _sc<S32>(cOldFile.Read(HUGE_NUMBER, pSmallBuffer));
				else
					iReadLen = _sc<S32>(cOldFile.Read(iLeft, pSmallBuffer));

				cNewFile.Write(iReadLen, pSmallBuffer);
				iLeft -= iReadLen;
			}
		}
		else
		{
			// add
			iType = (iOpCode & 0x60) >> 5;
			iLength = iOpCode & 0x1F;
			if (1 <= iType)
			{
				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 5);
			}
			if (2 <= iType)
			{
				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 13);
			}
			if (3 <= iType)
			{
				ASSERT(iType == 3);
				cPatchFile >> iTemp;
				iLength |= (_sc<U32>(iTemp) << 21);
			}

			U32	iLeft = iLength;

			while (iLeft)
			{
				S32	iReadLen;

				if (iLeft > HUGE_NUMBER)
					iReadLen = _sc<S32>(cPatchFile.Read(HUGE_NUMBER, pSmallBuffer));
				else
					iReadLen = _sc<S32>(cPatchFile.Read(iLeft, pSmallBuffer));

				cNewFile.Write(iReadLen, pSmallBuffer);
				iLeft -= iReadLen;
				if (iReadLen <= 0)
					break;
			}
		}

		cProgressInfo.iPercent_ = _sc<U32>(100.0f * cNewFile.GetPos() / iNewFileSize);
	}
	while (cPatchFile.GetPos() < cPatchFile.GetSize());

	return isGood;
}
