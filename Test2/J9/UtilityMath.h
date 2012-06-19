// $Id$

#ifndef __J9_UTILITY_MATH_H__
#define __J9_UTILITY_MATH_H__

#include "Random.h"

namespace J9
{
	template<typename T>
	T Max(T lhs, T rhs)
	{
		return (lhs > rhs) ? lhs: rhs;
	}

	template<typename T>
	T Min(T lhs, T rhs)
	{
		return (lhs < rhs) ? lhs: rhs;
	}

	inline F32 SqrtF(F32 f) { ASSERT(f >= 0.0f); return J9::Sqrt(f); }

	// Normal <-> Radian
	inline F32	NormalToRadian(F32 fNormal)
	{
		return F32_PI * 2.0f * fNormal;
	}

	inline F32	RadianToNormal(F32 fRadian)
	{
		return 0.5f * F32_1_PI * fRadian;
	}

	// Normal <-> Degree
	inline F32	NormalToDeg(F32 fNormal)
	{
		return fNormal * 360.0f;
	}

	inline F32	DegToNormal(F32 fDeg)
	{
		return fDeg / 360.0f;
	}

	// Radian <-> Degree
	inline F32	ToDeg(F32 fRad)
	{
		return fRad * 180.0f * F32_1_PI;
	}

	inline F32	ToRad(F32 fDeg)
	{
		return fDeg * F32_PI / 180.0f;
	}

	inline Vector2	ToDeg(const Vector2& cRad)
	{
		return cRad * 180.0f * F32_1_PI;
	}

	inline Vector2	ToRad(const Vector2& cDeg)
	{
		return cDeg * F32_PI / 180.0f;
	}

	inline Vector3	ToDeg(const Vector3& cRad)
	{
		return cRad * 180.0f * F32_1_PI;
	}

	inline Vector3	ToRad(const Vector3& cDeg)
	{
		return cDeg * F32_PI / 180.0f;
	}

	inline Vector4	ToDeg(const Vector4& cRad)
	{
		return cRad * 180.0f * F32_1_PI;
	}

	inline Vector4	ToRad(const Vector4& cDeg)
	{
		return cDeg * F32_PI / 180.0f;
	}

	inline F32  CosNormal(F32 fAngle)
	{
		return cosf(NormalToRadian(fAngle));
	}

	inline F32  SinNormal(F32 fAngle)
	{
		return sinf(NormalToRadian(fAngle));
	}

	// x, z좌표를 반시계방향으로 rad만큼 회전시킨다
	Vector3	Rotate(const Vector3& r, F32 fRad);

	// x, z좌표를 라디안 값으로 변환해준다
	F32		DirToRad(F32 fX, F32 fZ);

	inline F32		DirToNormal(F32 fX, F32 fZ)
	{
		return RadianToNormal(DirToRad(fX, fZ));
	}

	// 벡터의 방향으로 yaw, pitch를 구한다, x
	J9::Vector2	DirToRad(const J9::Vector3& r);
	inline J9::Vector2	DirToNormal(const J9::Vector3& r)
	{
		J9::Vector2	vNormal = DirToRad(r) * 0.5f * F32_1_PI;
		return vNormal;

	}

	// remap given angle to [0, 1] value
	inline F32		NormalizeAngle(F32 fAngle)
	{
		F32	fRet = ::modff(fAngle);
		if (fRet < 0.0f)
			fRet += 1.0f;

		return fRet;
	}

	inline F32		NormalizeRad(F32 fRad)
	{
		// implement this
		J9::Break();
		return fRad;
	}

	inline F32		NormalizeDeg(F32 fDeg)
	{
		// implement this
		J9::Break();
		return fDeg;
	}

	////////////////////////////////////////////////////////////////////////////////
	// interpolating functions
	// 선형 보간 매핑의 템플릿 버전.
	template<typename T>
	T Map(typename J9::ArgType<T>::Type cInputFrom,
		typename J9::ArgType<T>::Type cInputTo,
		typename J9::ArgType<T>::Type cOutputFrom,
		typename J9::ArgType<T>::Type cOutputTo,
		typename J9::ArgType<T>::Type cInput)
	{
		if (!((cInputFrom <= cInput && cInput <= cInputTo) || (cInputTo <= cInput && cInput <= cInputFrom))) return cOutputFrom;
		//TODO 데모용 일단 안죽게 하는거니 나중에 다시 위의 ASSERT 수정
		//ASSERT((cInputFrom <= cInput && cInput <= cInputTo) || (cInputTo <= cInput && cInput <= cInputFrom));

		return cOutputFrom + (cOutputTo - cOutputFrom) * (cInput - cInputFrom) / (cInputTo - cInputFrom);
	}

	template<>
	inline F32 Map<F32>(F32 fInputFrom, F32 fInputTo, F32 fOutputFrom, F32 fOutputTo, F32 fInput)
	{
		//ASSERT((fInputFrom <= fInput && fInput <= fInputTo) || (fInputTo <= fInpu하t && fInput <= fInputFrom));
		//ASSERT(F32_EPSILON < fabsf(fInputTo - fInputFrom));
		//TODO 데모용 일단 안죽게 하는거니 나중에 다시 위의 ASSERT 수정
		if (!(fInputFrom <= fInput && fInput <= fInputTo) || (fInputTo <= fInput && fInput <= fInputFrom))	return fOutputFrom;
		if (!(F32_EPSILON < fabsf(fInputTo - fInputFrom)))													return fOutputFrom;

		return fOutputFrom + (fOutputTo - fOutputFrom) * (fInput - fInputFrom) / (fInputTo - fInputFrom);
	}

	// 입력 범위안의 값을 출력범위로 매핑한다. 원형으로 매핑함
	F32 Map_Circle(F32 fInputFrom, F32 fInputTo, F32 fOutputFrom, F32 fOutputTo, F32 fInput);

	template<typename Value>
	Value Lerp(typename ArgType<Value>::Type lhs, typename ArgType<Value>::Type rhs, F32 fWeight)
	{
		return _sc<Value>(lhs + fWeight * (rhs - lhs));
	}

	template<>
	inline U8 Lerp(U8 lhs, U8 rhs, F32 fWeight)
	{
		return _sc<U8>(lhs + fWeight * (rhs - lhs) + .5f);
	}

	// 비정규화 된 가중치를 사용한 선형 보간
	inline F32 Lerp(F32 lhs, F32 rhs, F32 fWeight1, F32 fWeight2)
	{
		return (lhs * fWeight1 + rhs * fWeight2) / (fWeight1 + fWeight2);
	}

	inline F32 LerpNormalAngle(F32 lhs, F32 rhs, F32 fWeight)
	{
		// fTmp 는 rhs 와 lhs 의 각도의 차이중 작은 방향의 각도 입니다.
		// rhs 와 lhs 는 Invalid 가 아니란 것을 전제로 합니다.
		F32 fTmp = ::modff(rhs - lhs);
		if (fTmp < -0.5f)
			fTmp += 1.0f;
		else if (0.5f < fTmp)
			fTmp -= 1.0f;

		// 이제 fTmp 는 lhs 기준으로 0.5(180도) 미만인 사이각을 fWeight 만큼 보간한 값입니다.
		fTmp = ::modff(lhs + fTmp * fWeight);
		if (fTmp < 0.0f)
			fTmp += 1.0f;

		return fTmp;
	}

	inline F32 LerpNormalAngle(F32 lhs, F32 rhs, F32 fLWeight, F32 fRWeight)
	{
		// fTmp 는 rhs 와 lhs 의 각도의 차이중 작은 방향의 각도 입니다.
		// rhs 와 lhs 는 Invalid 가 아니란 것을 전제로 합니다.
		F32 fTmp = ::modff(rhs - lhs);
		if (fTmp < -0.5f)
			fTmp += 1.0f;
		else if (0.5f < fTmp)
			fTmp -= 1.0f;

		// 이제 fTmp 는 lhs 기준으로 0.5(180도) 미만인 사이각을 RWeight 만큼 보간한 값입니다.
		fTmp = ::modff(lhs + fTmp * (fRWeight / (fLWeight + fRWeight)));
		if (fTmp < 0.0f)
			fTmp += 1.0f;

		return fTmp;
	}
	////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	inline T GetRandom(const J9::_Vector<J9::__Vector<T, 2> >& rMinMax)
	{
		if (J9::TypeCategory<T>::bFloatingType)
			return _sc<T>(J9::Random::cGlobal.GetF32(_sc<F32>(rMinMax.x), _sc<F32>(rMinMax.y)));
		else if (J9::TypeCategory<T>::bUnsignedType)
			return _sc<T>(J9::Random::cGlobal.GetU32(_sc<U32>(rMinMax.x), _sc<U32>(rMinMax.y)));
		else if (J9::TypeCategory<T>::bSignedType)
			return _sc<T>(J9::Random::cGlobal.GetS32(_sc<S32>(rMinMax.x), _sc<S32>(rMinMax.y)));
	}

	template <>
	inline F32 GetRandom(const J9::F32Vector2& rMinMax)
	{
		return J9::Random::cGlobal.GetF32(rMinMax.x, rMinMax.y);
	}

	template <>
	inline S16 GetRandom(const J9::S16Vector2& rMinMax)
	{
		return _sc<S16>(J9::Random::cGlobal.GetS32(rMinMax.x, rMinMax.y));
	}

	inline void GetPlaneNormalByPoint(J9::Vector3& rOut, const J9::F32Vector3& rPoint1, const J9::F32Vector3& rPoint2, const J9::F32Vector3& rPoint3)
	{
		Vector3 vU = rPoint2 - rPoint1;
		Vector3 vV = rPoint3 - rPoint1;
		rOut = Cross(vU, vV);
		rOut.Normalize();
	}

	inline void GetSlidingVector(J9::F32Vector3& rOut, const J9::F32Vector3& rDir, const J9::F32Vector3& rPoint1, const J9::F32Vector3& rPoint2, const J9::F32Vector3& rPoint3)
	{
		//Vector3 vNormal;
		//GetPlaneNormalByPoint(vNormal, rPoint1, rPoint2, rPoint3);
		//rOut = rDir - (Cross(rDir, vNormal) * vNormal);
		GetPlaneNormalByPoint(rOut, rPoint1, rPoint2, rPoint3);
		rOut = rDir - (Dot(rDir, rOut) * rOut);
	}

	inline void GetReflectedVector(J9::F32Vector3& rOut, const J9::F32Vector3& rDir, const J9::F32Vector3& rPoint1, const J9::F32Vector3& rPoint2, const J9::F32Vector3& rPoint3)
	{
		GetPlaneNormalByPoint(rOut, rPoint1, rPoint2, rPoint3);
		rOut = rDir - (2.0f * Dot(rDir, rOut) * rOut);
	}

	bool	IsSame(F32 lhs, F32 rhs);
	template<Size iDimension>
	bool	IsSame(const _Vector<__Vector<F32, iDimension> >& lhs, const _Vector<__Vector<F32, iDimension> >& rhs)
	{
		for (int i = 0; i < iDimension; ++i)
		{
			if (!IsSame(lhs.v[i], rhs.v[i]))
				return false;
		}

		return true;
	}
};

#endif//__J9_UTILITY_MATH_H__
