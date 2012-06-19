// $Id: UtilityLevel0.cpp 2842 2009-05-19 07:06:45Z hyojin.lee $

#include "Stdafx.h"
#include "UtilityLevel0.h"

#include <math.h>

F32 modff(F32 f)
{
	F32	fDummy;
	return ::modff(f, &fDummy);
}
