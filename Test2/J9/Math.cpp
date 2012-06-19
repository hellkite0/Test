// $Id: Math.cpp 5282 2010-01-21 04:36:25Z hyojin.lee $

#include "Stdafx.h"
#include "Math.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace J9;

F32
J9::Sqrt(F32 f)
{
	if (!IsValid(f))
	{
		Break();
		return 0.0f;
	}
	else if (f < 0.0f)
	{
		Break();
		return 0.0f;
	}

	return ::sqrtf(f);
}

F32
J9::Sin(F32 f)
{
	return ::sinf(f);
}

F32
J9::Cos(F32 f)
{
	return ::cosf(f);
}

F32
J9::Tan(F32 f)
{
	return ::tanf(f);
}

F32
J9::Asin(F32 f)
{
	if (!IsValid(f))
	{
		Break();
		return 0.0f;
	}
	else if (!IsInRange<F32>(f, -1.0f, 1.0f))
	{
		Break();
		Fit(-1.0f, 1.0f, f);
	}

	return ::asinf(f);
}

F32
J9::Acos(F32 f)
{
	if (!IsValid(f))
	{
		Break();
		return 0.0f;
	}
	else if (!IsInRange<F32>(f, -1.0f, 1.0f))
	{
		Break();
		Fit(-1.0f, 1.0f, f);
	}

	return ::acosf(f);
}

F32
J9::Atan(F32 f)
{
	if (!IsValid(f))
	{
		Break();
		return 0.0f;
	}

	return ::atanf(f);
}

F32
J9::Atan2(F32 y, F32 x)
{
	if (!IsValid(y) || !IsValid(x))
	{
		Break();
		return 0.0f;
	}

	return ::atan2f(y, x);
}

// F64 versions
F64
J9::Sqrt(F64 f)
{
	if (f < 0.0)
	{
		Break();
		return 0.0;
	}

	return ::sqrt(f);
}

void
J9::SinCos(F32 fRad, F32& rSin, F32& rCos)
{
	rCos = ::cosf(fRad);
	F32	fCycle = fRad * (0.5f * F32_1_PI);
	F32	fDummy;
	fCycle = ::modff(fCycle, &fDummy);
	if (fCycle < 0.0f)
		fCycle += 1.0f;
	F32	fCycleSq = fCycle * fCycle;

	rSin = ::sqrtf(1.0f - rCos * rCos);
	if (0.25f < fCycleSq)
		rSin *= -1.0f;
}