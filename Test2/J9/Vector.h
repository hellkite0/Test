// $Id: Vector.h 7358 2010-09-15 02:06:06Z donghyuk.lee $

#ifndef __J9_VECTOR_H__
#define __J9_VECTOR_H__

#define VECTOR_ASSIGNMENT_OPERATOR(Operator, Operation) \
	__forceinline _Vector&	operator Operator(const _Vector& r) \
	{ \
		_VectorOperator<Numeric::Operation<Value>, _Vector>()(*this, r); \
		return *this; \
	} \

#define SCALAR_ASSIGNMENT_OPERATOR(Operator, Operation) \
	__forceinline _Vector&	operator Operator(ValueArg fV_) \
	{ \
		_VectorOperator<Numeric::Operation<Value>, _Vector>()(*this, fV_); \
		return *this; \
	} \

#define ASSIGNMENT_OPERATOR(Operator, Operation) \
	VECTOR_ASSIGNMENT_OPERATOR(Operator, Operation); \
	SCALAR_ASSIGNMENT_OPERATOR(Operator, Operation);

#define BINARY_OPERATOR_VECTOR_VECTOR(Operator, Operation) \
	__forceinline friend _Vector	operator Operator(const _Vector& lhs, const _Vector& rhs) \
	{ \
		_Vector	cV; \
		_VectorOperator<Numeric::Operation<Value>, _Vector>()(cV, lhs, rhs); \
		return cV; \
	} \

#define BINARY_OPERATOR_VECTOR_SCALAR(Operator, Operation) \
	__forceinline friend _Vector	operator Operator(const _Vector& lhs, ValueArg rhs) \
	{ \
		_Vector	cV; \
		_VectorOperator<Numeric::Operation<Value>, _Vector>()(cV, lhs, rhs); \
		return cV; \
	} \

#define BINARY_OPERATOR_SCALAR_VECTOR(Operator, Operation) \
	__forceinline friend _Vector	operator Operator(ValueArg lhs, const _Vector& rhs) \
	{ \
		_Vector	cV; \
		_VectorOperator<Numeric::Operation<Value>, _Vector>()(cV, lhs, rhs); \
		return cV; \
	} \

#define BINARY_OPERATOR(Operator, Operation) \
	BINARY_OPERATOR_VECTOR_VECTOR(Operator, Operation); \
	BINARY_OPERATOR_VECTOR_SCALAR(Operator, Operation); \
	BINARY_OPERATOR_SCALAR_VECTOR(Operator, Operation);

#define UNARY_OPERATOR(Operator, Operation) \
	__forceinline _Vector	operator Operator() const\
	{ \
		_Vector	cV; \
		_VectorOperator<Numeric::Operation<Value>, _Vector>()(*this, cV); \
		return cV; \
	} \

namespace J9
{
	template<typename Operation, typename Vector, Size iSize = Vector::eDimension>
	struct _VectorOperator : public Numeric::_Base<typename Vector::Value>
	{
		// binary operations
		__forceinline typename void operator()(Vector& r, const Vector& lhs, const Vector& rhs)
		{
			_VectorOperator<Operation, Vector, iSize - 1>()(r, lhs, rhs);
			r.v[iSize - 1] = Operation()(lhs.v[iSize - 1], rhs.v[iSize - 1]);
		}

		__forceinline typename void operator()(Vector& r, const Vector& lhs, ValueArg rhs)
		{
			_VectorOperator<Operation, Vector, iSize - 1>()(r, lhs, rhs);
			r.v[iSize - 1] = Operation()(lhs.v[iSize - 1], rhs);
		}

		__forceinline typename void operator()(Vector& r, ValueArg lhs, const Vector& rhs)
		{
			_VectorOperator<Operation, Vector, iSize - 1>()(r, lhs, rhs);
			r.v[iSize - 1] = Operation()(lhs, rhs.v[iSize - 1]);
		}

		// assignment operations
		__forceinline typename void operator()(Vector& r, const Vector& lhs)
		{
			_VectorOperator<Operation, Vector, iSize - 1>()(r, lhs);
			Operation()(r.v[iSize - 1], lhs.v[iSize - 1]);
		}

		__forceinline typename void operator()(Vector& r, ValueArg lhs)
		{
			_VectorOperator<Operation, Vector, iSize - 1>()(r, lhs);
			Operation()(r.v[iSize - 1], lhs);
		}

		// unary operation
		__forceinline typename void operator()(const Vector& lhs, Vector& r)
		{
			_VectorOperator<Operation, Vector, iSize - 1>()(lhs, r);
			r.v[iSize - 1] = Operation()(lhs.v[iSize - 1]);
		}
	};

	template<typename Operation, typename Vector>
	struct _VectorOperator<typename Operation, typename Vector, 0> : public Numeric::_Base<typename Vector::Value>
	{
		__forceinline typename void operator()(Vector&, const Vector&, const Vector&)
		{
		}

		__forceinline typename void operator()(Vector& , const Vector&, ValueArg)
		{
		}

		__forceinline typename void operator()(Vector&, ValueArg, const Vector&)
		{
		}

		__forceinline typename void operator()(Vector&, const Vector&)
		{
		}

		__forceinline typename void operator()(Vector&, ValueArg)
		{
		}

		__forceinline typename void operator()(const Vector&, Vector&)
		{
		}
	};

	template<typename Vector, Size iSize = Vector::eDimension>
	struct _Sum : public Numeric::_Base<typename Vector::Value>
	{
		__forceinline typename Value operator()(const Vector& r)
		{
			return r.v[iSize - 1] + _Sum<Vector, iSize - 1>()(r);
		}
	};

	template<typename Vector>
	struct _Sum<Vector, 0> : public Numeric::_Base<typename Vector::Value>
	{
		__forceinline typename Value	operator()(const Vector&)
		{
			return 0;
		}
	};

	template<typename Vector, Size iSize = Vector::eDimension>
	struct _Equal : public Numeric::_Base<typename Vector::Value>
	{
		__forceinline bool operator()(const Vector& lhs, const Vector& rhs)
		{
			return (lhs.v[iSize - 1] == rhs.v[iSize - 1]) && _Equal<Vector, iSize - 1>()(lhs, rhs);
		}
	};

	template<typename Vector>
	struct _Equal<Vector, 0> : public Numeric::_Base<typename Vector::Value>
	{
		__forceinline bool	operator()(const Vector&, const Vector&)
		{
			return true;
		}
	};

#pragma pack(push, 1)
	////////////////////////////////////////////////////////////////////////////////
	/// \page	vector Vector Operations
	///	Vector는 다음과 같은 연산자들을 지원한다.\n
	///	Vector&	operator+=(const Vector&);
	///	Vector&	operator-=(const Vector&);
	///	Vector&	operator*=(const Vector&);
	///	Vector&	operator/=(const Vector&);
	///	Vector&	operator%=(const Vector&);
	///	Vector&	operator+=(Value);
	///	Vector&	operator-=(Value);
	///	Vector&	operator*=(Value);
	///	Vector&	operator/=(Value);
	///	Vector&	operator%=(Value);
	///	Vector	operator+(const Vector&, const Vector&);
	///	Vector	operator-(const Vector&, const Vector&);
	///	Vector	operator*(const Vector&, const Vector&);
	///	Vector	operator/(const Vector&, const Vector&);
	///	Vector	operator%(const Vector&, const Vector&);
	///	Vector	operator+(const Vector&, Value);
	///	Vector	operator-(const Vector&, Value);
	///	Vector	operator*(const Vector&, Value);
	///	Vector	operator/(const Vector&, Value);
	///	Vector	operator%(const Vector&, Value);
	///	Vector	operator+(Value, const Vector&);
	///	Vector	operator-(Value, const Vector&);
	///	Vector	operator*(Value, const Vector&);
	///	Vector	operator/(Value, const Vector&);
	///	Vector	operator%(Value, const Vector&);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	벡터를 나타내는 구조체
	///
	///	벡터의 차원에 따라 원소의 이름이 여러가지로 쓰이므로 있으므로 따로 정의한다.
	///	\see \ref vector "Vector Operations"
	////////////////////////////////////////////////////////////////////////////////
	template<typename T, Size i>
	struct __Vector;

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	2차원 벡터를 나타내는 구조체
	///
	///	2차원 벡터를 x, y나 tu, tv로 나타낸다
	///	워낙 자주 쓰이고 멤버의 뜻이 명확하므로 네이밍 컨벤션을 따르지 않는다.\n
	///	\see \ref vector "Vector Operations"
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct __Vector<T, 2>
	{
		enum	{ eDimension = 2, };
		typedef	T	Value;

		union
		{
			struct
			{
				Value x;
				Value y;
			};

			struct
			{
				Value tu;
				Value tv;
			};

			struct
			{
				Value min;
				Value max;
			};

			struct
			{
				Value h;
				Value p;
			};

			Value	v[eDimension];

			//union { Value x; Value tu; Value min; Value h; };
			//union { Value y; Value tv; Value max; Value p; };
		};
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	3차원 벡터를 나타내는 구조체
	///
	///	3차원 벡터를 x, y, z나 tu, tv, tw나 h, r, r로 나타낸다
	///	워낙 자주 쓰이고 멤버의 뜻이 명확하므로 네이밍 컨벤션을 따르지 않는다.
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct __Vector<T, 3>
	{
		enum	{ eDimension = 3, };
		typedef	T	Value;

		union
		{
			struct
			{
				Value x;
				Value y;
				Value z;
			};

			struct
			{
				Value tu;
				Value tv;
				Value tw;
			};

			struct
			{
				Value h;
				Value p;
				Value r;
			};
			Value	v[eDimension];
			//struct
			//{
			//	union { Value x; Value tu; Value h; };
			//	union { Value y; Value tv; Value p; };
			//	union { Value z; Value tw; Value r; };
			//};
		};
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief	4차원 벡터를 나타내는 구조체
	///
	///	4차원 벡터를 x, y, z, w나 tu, tv, tw, tq로 나타낸다
	///	워낙 자주 쓰이고 멤버의 뜻이 명확하므로 네이밍 컨벤션을 따르지 않는다.
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct __Vector<T, 4>
	{
		enum	{ eDimension = 4, };
		typedef	T	Value;

		union
		{
			struct
			{
				Value x;
				Value y;
				Value z;
				Value w;
			};

			struct
			{
				Value tu;
				Value tv;
				Value tw;
				Value tq;
			};

			struct
			{
				Value left;
				Value right;
				Value top;
				Value bottom;
			};

			struct
			{
				Value cx;
				Value cy;
				Value sx;
				Value sy;
			};
			Value	v[eDimension];
			//struct
			//{
			//	union { Value x; Value tu; Value cx; Value left; };
			//	union { Value y; Value tv; Value cy; Value right; };
			//	union { Value z; Value tw; Value sx; Value top; };
			//	union { Value w; Value tq; Value sy; Value bottom; };
			//};
		};
	};

	template<typename _Base>
	struct _Vector : public _Base
	{
		typedef typename _Base::Value			Value;
		typedef typename ArgType<Value>::Type	ValueArg;

		// contructors
		__forceinline explicit _Vector()
		{
			CASSERT(sizeof(_Vector) == sizeof(Value) * eDimension, PACK);
			_VectorOperator<Numeric::Assign<Value>, _Vector>()(*this, Numeric::Zero<Value>()());
		}

		__forceinline explicit _Vector(ValueArg v)
		{
			_VectorOperator<Numeric::Assign<Value>, _Vector>()(*this, v);
		}

		__forceinline explicit _Vector(ValueArg v0, ValueArg v1)
		{
			CASSERT(eDimension == 2, DIMENSION);
			v[0] = v0; v[1] = v1;
		}

		__forceinline explicit _Vector(ValueArg v0, ValueArg v1, ValueArg v2)
		{
			CASSERT(eDimension == 3, DIMENSION);
			v[0] = v0; v[1] = v1; v[2] = v2;
		}

		__forceinline explicit _Vector(ValueArg v0, ValueArg v1, ValueArg v2, ValueArg v3)
		{
			CASSERT(eDimension == 4, DIMENSION);
			v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
		}

		__forceinline explicit _Vector(const Value* p)
		{
			_VectorOperator<Numeric::Assign<Value>, _Vector>()(*this, *_rc<const _Vector*>(p));
		}

		// assignment operators
		ASSIGNMENT_OPERATOR(=, Assign);
		ASSIGNMENT_OPERATOR(+=, AddAssign);
		ASSIGNMENT_OPERATOR(-=, SubAssign);
		ASSIGNMENT_OPERATOR(*=, MulAssign);
		ASSIGNMENT_OPERATOR(/=, DivAssign);
		ASSIGNMENT_OPERATOR(%=, ModAssign);

		// special assignment operators
		_Vector& operator=(const StringW& rString);

		// binary operators
		BINARY_OPERATOR(+, Add);
		BINARY_OPERATOR(-, Sub);
		BINARY_OPERATOR(*, Mul);
		BINARY_OPERATOR(/, Div);
		BINARY_OPERATOR(%, Mod);

		UNARY_OPERATOR(-, Neg);

		// non member functions
		__forceinline friend bool	operator==(const _Vector& lhs, const _Vector& rhs)
		{
			return _Equal<_Vector>()(lhs, rhs);
		}

		__forceinline friend bool	operator!=(const _Vector& lhs, const _Vector& rhs)
		{
			return !_Equal<_Vector>()(lhs, rhs);
		}

		__forceinline bool	IsZero() const
		{
			return *this == _Vector();
		}

		__forceinline typename Value	Sum() const { return _Sum<_Vector>()(*this); }
		__forceinline typename Value	SquareSum() const { return _Sum<_Vector>()(*this * *this); }

		// 부동소수점 벡터만을 위한 연산들
		__forceinline void Normalize()
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			// 영벡터를 어떻게 노말라이즈해
			if (IsZero())		return;

			Value fAbs = Numeric::Sqrt<Value>()(SquareSum()); if (fAbs != 0) *this /= fAbs;
		}

		__forceinline bool IsValid() const
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			// 영벡터를 어떻게 노말라이즈해
			return *this == *this;
		}

		__forceinline bool IsNormalized() const
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			const Value	fDelta = _sc<Value>(2) * Numeric::Epsilon<Value>()();
			Value fLength = Length();
			return (Numeric::One<Value>()() - fDelta) <= fLength
				&& fLength <= (Numeric::One<Value>()() + fDelta);
		}

		__forceinline typename Value	Length() const
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			return J9::Sqrt(SquareSum());
		}

		__forceinline void ProjectTo(const _Vector& rTo)
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			ASSERT(rTo.IsNormalized());
			Value fDot = Dot(*this, rTo);
			*this = rTo * fDot;
		}

		__forceinline void OrthogonalizeTo(const _Vector& rTo)
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			_Vector	cProjected = *this;
			cProjected.ProjectTo(rTo);
			*this -= cProjected;
		}

		__forceinline void Fit(ValueArg v)
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			// 영벡터즐
			if (IsZero())		return;
			Value fAbs = Numeric::Sqrt<Value>()(SquareSum()); if (fAbs != 0) *this *= v / fAbs ;
		}

		// non member functions
		__forceinline friend _Vector Normalize(const _Vector& rVector)
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			_Vector	cVector(rVector);

			cVector.Normalize();
			return cVector;
		}

		__forceinline friend _Vector Project(const _Vector& rVector, const _Vector& rTo)
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			_Vector	cProjected = rVector;
			cProjected.ProjectTo(rTo);
			return cProjected;
		}

		__forceinline friend _Vector Orthogonalize(const _Vector& rVector, const _Vector& rTo)
		{
			CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
			_Vector	cOrthogonalized = rVector;
			cOrthogonalized.OrthogonalizeTo(rTo);
			return cOrthogonalized;
		}
	};
#pragma pack(pop)

	template<typename Value>
	__forceinline
	Value	Dot(const _Vector<__Vector<Value, 2> >& lhs, const _Vector<__Vector<Value, 2> >& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	template<typename Value>
	__forceinline
	Value	Dot(const _Vector<__Vector<Value, 3> >& lhs, const _Vector<__Vector<Value, 3> >& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	template<typename Value>
	__forceinline
	Value	Dot(const _Vector<__Vector<Value, 4> >& lhs, const _Vector<__Vector<Value, 4> >& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}

	template<typename Value>
	__forceinline
	_Vector<__Vector<Value, 3> >	Cross(const _Vector<__Vector<Value, 3> >& lhs, const _Vector<__Vector<Value, 3> >& rhs)
	{
		_Vector<__Vector<Value, 3> >	cV;
		cV.v[0] = lhs.v[1] * rhs.v[2] - lhs.v[2] * rhs.v[1];
		cV.v[1] = lhs.v[2] * rhs.v[0] - lhs.v[0] * rhs.v[2];
		cV.v[2] = lhs.v[0] * rhs.v[1] - lhs.v[1] * rhs.v[0];

		return cV;
	}

	template<typename Value>
	__forceinline
	void	Cross(_Vector<__Vector<Value, 3> >& rOut, const _Vector<__Vector<Value, 3> >& lhs, const _Vector<__Vector<Value, 3> >& rhs)
	{
		rOut.v[0] = lhs.v[1] * rhs.v[2] - lhs.v[2] * rhs.v[1];
		rOut.v[1] = lhs.v[2] * rhs.v[0] - lhs.v[0] * rhs.v[2];
		rOut.v[2] = lhs.v[0] * rhs.v[1] - lhs.v[1] * rhs.v[0];
	}

	template<typename Value>
	__forceinline
	Value	Distance(const _Vector<__Vector<Value, 2> >& lhs, const _Vector<__Vector<Value, 2> >& rhs)
	{
		CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
		Value	tdx = lhs.x - rhs.x;
		Value	tdy = lhs.y - rhs.y;

		return ::sqrt(tdx * tdx + tdy * tdy);
	}

	template<typename Value>
	__forceinline
	Value	Distance(const _Vector<__Vector<Value, 3> >& lhs, const _Vector<__Vector<Value, 3> >& rhs)
	{
		CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
		Value	tdx = lhs.x - rhs.x;
		Value	tdy = lhs.y - rhs.y;
		Value	tdz = lhs.z - rhs.z;

		return ::sqrt(tdx * tdx + tdy * tdy + tdz * tdz);
	}

	template<typename Value>
	__forceinline
	Value	Distance(const _Vector<__Vector<Value, 4> >& lhs, const _Vector<__Vector<Value, 4> >& rhs)
	{
		CASSERT(TypeCategory<Value>::bFloatingType, ONLY_FOR_FLOATING_TYPE);
		Value	tdx = lhs.x - rhs.x;
		Value	tdy = lhs.y - rhs.y;
		Value	tdz = lhs.z - rhs.z;
		Value	tdw = lhs.w - rhs.w;

		return ::sqrt(tdx * tdx + tdy * tdy + tdz * tdz + tdw * tdw);
	}

	template<typename Value>
	__forceinline
	Value	DistanceSq(const _Vector<__Vector<Value, 2> >& lhs, const _Vector<__Vector<Value, 2> >& rhs)
	{
		Value	tdx = lhs.x - rhs.x;
		Value	tdy = lhs.y - rhs.y;

		return tdx * tdx + tdy * tdy;
	}

	template<typename Value>
	__forceinline
	Value	DistanceSq(const _Vector<__Vector<Value, 3> >& lhs, const _Vector<__Vector<Value, 3> >& rhs)
	{
		Value	tdx = lhs.x - rhs.x;
		Value	tdy = lhs.y - rhs.y;
		Value	tdz = lhs.z - rhs.z;

		return tdx * tdx + tdy * tdy + tdz * tdz;
	}

	template<typename Value>
	__forceinline
	Value	DistanceSq(const _Vector<__Vector<Value, 4> >& lhs, const _Vector<__Vector<Value, 4> >& rhs)
	{
		Value	tdx = lhs.x - rhs.x;
		Value	tdy = lhs.y - rhs.y;
		Value	tdz = lhs.z - rhs.z;
		Value	tdw = lhs.w - rhs.w;

		return tdx * tdx + tdy * tdy + tdz * tdz + tdw * tdw;
	}

	// predefine some useful vectors
	typedef _Vector<__Vector<F32, 2> >	F32Vector2;
	typedef _Vector<__Vector<F32, 3> >	F32Vector3;
	typedef _Vector<__Vector<F32, 4> >	F32Vector4;
	typedef _Vector<__Vector<S32, 2> >	S32Vector2;
	typedef _Vector<__Vector<S32, 3> >	S32Vector3;
	typedef _Vector<__Vector<S32, 4> >	S32Vector4;
	typedef _Vector<__Vector<S16, 2> >	S16Vector2;
	typedef _Vector<__Vector<S16, 3> >	S16Vector3;
	typedef _Vector<__Vector<S16, 4> >	S16Vector4;
	typedef _Vector<__Vector<U8, 2> >	U8Vector2;
	typedef _Vector<__Vector<U8, 3> >	U8Vector3;
	typedef _Vector<__Vector<U8, 4> >	U8Vector4;
	typedef _Vector<__Vector<S8, 2> >	S8Vector2;
	typedef _Vector<__Vector<S8, 3> >	S8Vector3;
	typedef _Vector<__Vector<S8, 4> >	S8Vector4;

	typedef _Vector<__Vector<U32, 2> >	U32Vector2;
	typedef _Vector<__Vector<U32, 3> >	U32Vector3;
	typedef _Vector<__Vector<U32, 4> >	U32Vector4;
	typedef _Vector<__Vector<U16, 2> >	U16Vector2;
	typedef _Vector<__Vector<U16, 3> >	U16Vector3;
	typedef _Vector<__Vector<U16, 4> >	U16Vector4;

	typedef _Vector<__Vector<Size, 2> >	SizeVector2;
	typedef _Vector<__Vector<Size, 3> >	SizeVector3;
	typedef _Vector<__Vector<Size, 4> >	SizeVector4;

	typedef F32Vector2	Vector2;
	typedef F32Vector3	Vector3;
	typedef F32Vector4	Vector4;

#ifdef J9_SOL_USE_DIRECTX
	inline void	TransformNormal(J9::Vector3& rVec, const D3DXMATRIXA16& rMat)
	{
		::D3DXVec3TransformNormal(_rc<D3DXVECTOR3*>(&rVec), _rc<D3DXVECTOR3*>(&rVec), &rMat);
	}

	inline void	TransformCoord(J9::Vector3& rVec, const D3DXMATRIXA16& rMat)
	{
		::D3DXVec3TransformCoord(_rc<D3DXVECTOR3*>(&rVec), _rc<D3DXVECTOR3*>(&rVec), &rMat);
	}

	inline void	Transform(J9::Vector3& rVec, const D3DXMATRIXA16& rMat)
	{
		TransformCoord(rVec, rMat);
	}
#endif
};

// optimized dirextx functions
#ifdef J9_SOL_USE_DIRECTX
__forceinline void
D3DXVec3Cross
(D3DXVECTOR3& rOut, const D3DXVECTOR3& rV1, const D3DXVECTOR3& rV2)
{
	rOut.x = rV1.y * rV2.z - rV1.z * rV2.y;
	rOut.y = rV1.z * rV2.x - rV1.x * rV2.z;
	rOut.z = rV1.x * rV2.y - rV1.y * rV2.x;
}

__forceinline F32
D3DXVec3Dot
(const D3DXVECTOR3& rV1, const D3DXVECTOR3& rV2)
{
	return rV1.x * rV2.x + rV1.y * rV2.y + rV1.z * rV2.z;
}
#endif//J9_SOL_USE_DIRECTX

USE_TYPE_TRAITS_DEFAULT(J9::Vector2, 8);
USE_TYPE_TRAITS_DEFAULT(J9::Vector3, 12);
USE_TYPE_TRAITS_DEFAULT(J9::Vector4, 16);

USE_TYPE_TRAITS_DEFAULT(J9::U32Vector2, 8);
USE_TYPE_TRAITS_DEFAULT(J9::U32Vector3, 12);
USE_TYPE_TRAITS_DEFAULT(J9::U32Vector4, 16);

#endif//__J9_VECTOR_H__