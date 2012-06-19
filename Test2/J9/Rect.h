// $Id: Rect.h 4313 2009-10-29 04:00:44Z hyojin.lee $

#ifndef __J9_RECT_H__
#define __J9_RECT_H__

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	///	\brief	RECT ����ü�� ��ӹ޾Ƽ� ���� ������ ����� ������ ���� Ŭ����.
	///
	///	RECT����ü�� ��ӹ޾Ƽ� ���� ������ ��� �Լ��� �����ϰ�,
	///	��� �����ڵ��� ������ ���Ҵ�.
	///	������ ���Ǹ� ���� ���۵Ǿ���, �������� ������������ ���ؼ�
	///	���� ������� RECT���� API�� ȣ���ϵ��� ����� �� �ִ�.
	///	���ɻ��� ������ ����ų�, ������ �ǽɵ� ��� ������ �ڵ带 ¥ ������ �ȴ�.
	///	���� ���۽� �ʿ��� �Լ��� ����� �������Ƿ�, ���� �ʿ��� �Լ��� �߰��� ����� �ȴ�.
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

		// ����, �Ǽ����� �ƴ� ������ ���� 0�� ������ �� �ֵ��� �����Ǿ�� �Ѵ�.
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