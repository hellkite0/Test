// $Id: Rect.h 4313 2009-10-29 04:00:44Z hyojin.lee $

#ifndef __J9_RECT_H__
#define __J9_RECT_H__

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	///	\brief	RECT 구조체를 상속받아서 각종 연산을 멤버로 구현해 놓은 클래스.
	///
	///	RECT구조체를 상속받아서 각종 연산을 멤버 함수로 구현하고,
	///	몇몇 연산자들을 정의해 놓았다.
	///	사용상의 편의를 위해 제작되었고, 제작자의 귀차니즘으로 인해서
	///	각종 멤버들이 RECT관련 API를 호출하도록 만들어 져 있다.
	///	성능상의 문제가 생기거나, 성능이 의심될 경우 손으로 코드를 짜 넣으면 된다.
	///	또한 제작시 필요한 함수만 만들어 놓았으므로, 사용시 필요한 함수는 추가로 만들면 된다.
	///
	///	\author	hyojin.lee
	///	\date	2005-11-29
	////////////////////////////////////////////////////////////////////////////////
	class Rect : public RECT
	{
	public:
		__forceinline S32	Width() const						{ return right - left; }
		__forceinline S32	Height() const						{ return bottom - top; }
		__forceinline bool	IsEmpty() const						{ return !!::IsRectEmpty(this); }

		__forceinline void	Set(S32 l, S32 t, S32 r, S32 b)		{ ::SetRect(this, l, t, r, b); }
		__forceinline void	SetEmpty()							{ ::SetRectEmpty(this); }
		__forceinline void	Offset(S32 x, S32 y)				{ ::OffsetRect(this, x, y); }
	};

	__forceinline
	bool
	operator==(const Rect& lhs, const Rect& rhs)
	{ return !!::EqualRect(&lhs, &rhs); }

	__forceinline
	bool
	operator!=(const Rect& lhs, const Rect& rhs)
	{ return !::EqualRect(&lhs, &rhs); }

	template<typename T>
	class TRect
	{
	public:
		__forceinline T		Width() const						{ return right - left; }
		__forceinline T		Height() const						{ return bottom - top; }
		__forceinline bool	IsEmpty() const						{ return !!::IsRectEmpty(this); }

		__forceinline void	Set(T l, T t, T r, T b)				{ left = l; top = t; right = r; bottom = b; }
		__forceinline void	SetLT(T l, T t)						{ left = l; top = t; }
		__forceinline void	SetRB(T r, T b)						{ right = r; bottom = b; }
		__forceinline void	SetLT(const J9::_Vector<J9::__Vector<T, 2> >& r) { SetLT(r.x, r.y); }
		__forceinline void	SetRB(const J9::_Vector<J9::__Vector<T, 2> >& r) { SetRB(r.x, r.y); }

		// 정수, 실수값이 아닌 임의의 값에 0을 대입할 수 있도록 수정되어야 한다.
		__forceinline void	SetEmpty()							{ left = 0; top = 0; right = 0; bottom = 0; }
		__forceinline void	Offset(T x, T y)					{ left += x; top += y; right += x; bottom += y; }

		__forceinline bool	Contains(T x, T y) const			{ return left <= x && x < right && top <= y && y < bottom; }
		__forceinline bool	Contains(const J9::_Vector<J9::__Vector<T, 2> >& r) const { return Contains(r.x, r.y); }

		__forceinline J9::_Vector<J9::__Vector<T, 2> >	LT() const	{ return J9::_Vector<J9::__Vector<T, 2> >(left, top); }
		__forceinline J9::_Vector<J9::__Vector<T, 2> >	RT() const	{ return J9::_Vector<J9::__Vector<T, 2> >(right, top); }
		__forceinline J9::_Vector<J9::__Vector<T, 2> >	LB() const	{ return J9::_Vector<J9::__Vector<T, 2> >(left, bottom); }
		__forceinline J9::_Vector<J9::__Vector<T, 2> >	RB() const	{ return J9::_Vector<J9::__Vector<T, 2> >(right, bottom); }

	public:
		union
		{
			struct
			{
				T		left;
				T		top;
				T		right;
				T		bottom;
			};
			T			v[4];
		};
	};

	typedef TRect<F32>	F32Rect;
};

#endif//__J9_RECT_H__