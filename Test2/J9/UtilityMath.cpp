// $Id$

#include "Stdafx.h"
#include "UtilityMath.h"

using namespace J9;

F32
J9::Map_Circle(F32 fInputFrom, F32 fInputTo, F32 fOutputFrom, F32 fOutputTo, F32 fInput)
{
	ASSERT((fInputFrom <= fInput && fInput <= fInputTo) || (fInputTo <= fInput && fInput <= fInputFrom));
	ASSERT(F32_EPSILON < fabsf(fInputTo - fInputFrom));

	F32	fX = (fInput - fInputFrom) / (fInputTo - fInputFrom) * 2.0f - 1.0f;

	bool	bMinus = fX < 0.0f;
	fX = ::fabsf(fX);

	F32	fY = SqrtF(1.0f - (fX - 1.0f) * (fX - 1.0f));

	fY = bMinus ? -fY : fY;

	fY = (fY + 1.0f) * 0.5f;

	return fOutputFrom + fY * (fOutputTo - fOutputFrom);
}

Vector3
J9::Rotate(const Vector3& r, F32 fRad)
{
	F32	fSin = ::sinf(fRad);
	F32	fCos = ::cosf(fRad);

	J9::Vector3	cRotated;

	cRotated.x = fCos * r.x - fSin * r.z;
	cRotated.y = r.y;
	cRotated.z = fSin * r.x + fCos * r.z;

	return cRotated;
}

F32
J9::DirToRad(F32 fX, F32 fZ)
{
	return J9::Atan2(-fX, -fZ);
}

J9::Vector2
J9::DirToRad(const Vector3& r)
{
	J9::Vector2		vRad;
	vRad.h = DirToRad(r.x, r.z);

	F32	fXZLen = J9::SqrtF(r.x * r.x + r.z * r.z);
	vRad.p = J9::Atan2(r.y, fXZLen);

	return vRad;
}

bool	IsAlmostZero(F32 f)
{
	static const F32	fAlmostZero = 0.00001f;
	if (-fAlmostZero <= f && f <= fAlmostZero)
		return true;

	return false;
}

bool
J9::IsSame(F32 lhs, F32 rhs)
{
	static const F32	fSimilarityCoef = 0.9999f;

	if (IsAlmostZero(lhs) && IsAlmostZero(rhs))
		return true;

	if (fabsf(rhs) < fabsf(lhs))
	{
		F32	fTemp = lhs;
		lhs = rhs;
		rhs = fTemp;
	}

	F32	fRatio = lhs / rhs;

	if (fSimilarityCoef < fRatio)
		return true;

	return false;
}
