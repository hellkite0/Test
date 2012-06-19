// $Id: UtilityMisc.cpp 8019 2011-02-09 06:44:41Z hyojin.lee $

#include "Stdafx.h"
#include "UtilityMisc.h"

#include "UtilityString.h"

using namespace J9;

StringW
J9::MakeStringFromDebugInfo(const DebugInfo& rWhere)
{
	return StringUtil::Format(_W("file : %s, line : %d, function %s"), rWhere.pFileName_, rWhere.iLine_, rWhere.pFuncName_);
}

int
J9::RoundUpPowerOf2(int  iNumber)
{
	iNumber--;
	iNumber |= iNumber >> 1;
	iNumber |= iNumber >> 2;
	iNumber |= iNumber >> 4;
	iNumber |= iNumber >> 8;
	iNumber |= iNumber >> 16;
	iNumber++;
	iNumber += (iNumber == 0);
	return iNumber;
}

void
J9::MemSet4(void* pVoid, int iSet, Size iSize)
{
	ASSERT(iSize % sizeof(int) == 0);
	iSize = iSize >> 2;
	int*	pDest = _rc<int*>(pVoid);
	while(iSize-->0)
	{
		*pDest++ = iSet;
	}
}

bool
J9::MemCmpCpy(void* pDest, const void* pSrc, Size iSize)
{
	if (::memcmp(pDest, pSrc, iSize) == 0)
		return false;

	::memcpy(pDest, pSrc, iSize);
	return true;
}
