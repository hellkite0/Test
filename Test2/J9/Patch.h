// $Id: Patch.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_PATCH_H__
#define __J9_PATCH_H__

namespace J9
{
	namespace Patch
	{
		bool	GeneratePatch(const WChar* pOldFileName, const WChar* pNewFileName, const WChar* pPatchFileName);
		bool	ApplyPatch(const WChar* pOldFileName, const WChar* pPatchFileName, const WChar* pNewFileName);
	}
}

#endif//__J9_PATCH_H__