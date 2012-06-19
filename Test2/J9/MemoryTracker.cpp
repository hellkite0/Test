// $Id$

#include "stdafx.h"
#include "MemoryTracker.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// MemoryTracker
////////////////////////////////////////////////////////////////////////////////
MemoryTracker::MemoryTracker()
{
#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

MemoryTracker::~MemoryTracker()
{
}
