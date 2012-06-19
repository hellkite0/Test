// $Id: Physics.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_PHYSICS_H__
#define __J9_PHYSICS_H__

#include "Vector.h"

namespace J9
{
	struct Physic
	{
		F32	fFrictionConstant_;
		F32	fAirFrictionConstant_;
		F32	fTorqueConstant_;
		F32 fRepulsionConstant_;

		F32		GetFlyingDistance(F32 fVel, F32 fAcc, F32 fTime) const;
		void	AddFlyingDistance(const J9::Vector3& cVel, const J9::Vector3& cAcc, F32 fTime, J9::Vector3& cOutPos) const;

		F32		GetAirFriction(F32 fVel, F32 fTime) const;
		void	ApplyAirFriction(F32 fTime, J9::Vector3& cOutVel) const;

		F32		ApplyLandFriction(F32 fVel, F32 fTime) const;

		void	GetTorque(const Vector3& rRot, const Vector3& rVel, Vector3& rTorque) const;

		void	GetRepulsion(const J9::Vector3& rNormal, const J9::Vector3& rVel, J9::Vector3& rOutVec) const;
	};
};

#endif//__J9_PHYSICS_H__