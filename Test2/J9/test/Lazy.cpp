// $Id: Lazy.cpp 2378 2009-04-22 13:34:04Z hyojin.lee $

#include "Stdafx.h"
#include "Lazy.h"

#include <io.h>

void
Do(const WChar* pDir, bool bIncludeSubDir)
{
	UNUSED(bIncludeSubDir);

	intptr_t hFile;
	struct _wfinddata_t cFile;
	WChar cDir[DEFAULT_NUMBER];

	wsprintfW(cDir, _W("%s/*.gba"), pDir);
	if( (hFile = _wfindfirst((wchar_t*) cDir, &cFile )) == -1L )
	{
		LOG(_W( "No files in current (%s) directory!\n"), pDir);
		return;
	}

	do
	{
		if (wcscmp(cFile.name, _W(".")) == 0) continue;
		if (wcscmp(cFile.name, _W("..")) == 0) continue;

		WChar cPath[DEFAULT_NUMBER];
		wsprintfW(cPath, _W("alzip -a -d -m5 \"%s\" \"%s.zip\""), cFile.name);
	}
	while (_wfindnext(hFile, &cFile) == 0);
	_findclose(hFile);
}

S32
Lazy::DoMain(int argc, TCHAR* argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	const WChar*	pDir = _W("data/xml/meshes");
	bool			bIncludeSubDir = false;

	Do(pDir, bIncludeSubDir);

	return 0;
}