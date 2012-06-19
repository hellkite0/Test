// $Id: Physics.cpp 3959 2009-09-25 09:50:04Z hyojin.lee $

#include "Stdafx.h"
#include "Physics.h"

using namespace J9;

F32
Physic::GetFlyingDistance(F32 fVel, F32 fAcc, F32 fTime) const
{
	// d = vt * 0.5at^2
	return fVel * fTime + .5f * fAcc * fTime * fTime;
}

void
Physic::AddFlyingDistance(const Vector3& cVel, const Vector3& cAcc, F32 fTime, Vector3& cOutPos) const
{
	cOutPos.x += GetFlyingDistance(cVel.x, cAcc.x, fTime);
	cOutPos.y += GetFlyingDistance(cVel.y, cAcc.y, fTime);
	cOutPos.z += GetFlyingDistance(cVel.z, cAcc.z, fTime);
}

F32
Physic::GetAirFriction(F32 fVel, F32 fTime) const
{
    return (fAirFrictionConstant_ * fVel * fVel * fTime) * (fVel > 0 ? -1 : 1);
}

void
Physic::ApplyAirFriction(F32 fTime, J9::Vector3& cOutVel) const
{
	F32	fSpeed = cOutVel.Length();
	F32	fFriction = GetAirFriction(fSpeed, fTime);
	Vector3	cUnit = cOutVel;
	cUnit.Normalize();
	cUnit *= fFriction;
	cOutVel += cUnit;
}

F32
Physic::ApplyLandFriction(F32 fVel, F32 fTime) const
{
	if (abs(fVel) <= 0.1f) return 0.0f;

	return fVel - fVel * fFrictionConstant_ * fTime;
}

void
Physic::GetTorque(const Vector3& rRot, const Vector3& rVel, Vector3& rTorque) const
{
	F32 fSpeed = rVel.Length();
	rTorque = rRot * fSpeed * fTorqueConstant_;
}

void
Physic::GetRepulsion(const J9::Vector3& rNormal, const J9::Vector3& rVel, J9::Vector3& rOutVec) const
{

	Vector3	cNormal = rNormal;
	Vector3 cVel = rVel;
	cNormal.Normalize();
	F32	fDot = Dot(cNormal, -cVel);
	Vector3	cTemp = (1.0f + fRepulsionConstant_) * fDot * cNormal;
	rOutVec = cTemp + cVel;

	//일환 스파이 로그
	//LOG(_W("B:REPULC:%f\r\n"), fRepulsionConstant_);
	//LOG(_W("B:SPEEDVB:(%f, %f, %f)\r\n"), rVel.x, rVel.y, rVel.z);
	//LOG(_W("B:SPEEDVA:(%f, %f, %f)\r\n"), rOutVec.x, rOutVec.y, rOutVec.z);
	//LOG(_W("B:CRASHNV:(%f, %f, %f)\r\n"), cNormal.x, cNormal.y, cNormal.z);

	/*
	Vector3	cNormal = rNormal;
	Vector3 cVel = rVel;
	cNormal.Normalize();
	F32	fDot = Dot(cNormal, -cVel);
	Vector3	cTemp = fDot * cNormal;//(2.0f - fRepulsionConstant_) * fDot * cNormal;
	rOutVec = cTemp + cVel;
	*/
}