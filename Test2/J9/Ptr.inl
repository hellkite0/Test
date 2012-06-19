// $Id: Ptr.inl 3869 2009-09-21 09:15:33Z hyojin.lee $

#ifndef __J9_PTR_INL__
#define __J9_PTR_INL__

#include "Assert.h"

#if (_J9_PTR_DIAGNOSTIC)
	#define VALIDATE_PTR(p)	ASSERT((p).IsValid())
#else
	#define VALIDATE_PTR(p)
#endif

namespace J9
{
	template<typename T>
	__forceinline
	Ptr<T>::Ptr() : p_(NULL)
	{
	}

	template<typename T>
	__forceinline
	Ptr<T>::Ptr(const Ptr& r) : p_(NULL)
	{
		*this = r;
	}

	template<typename T>
	__forceinline
	Ptr<T>::Ptr(T* p) : p_(NULL)
	{
		*this = p;
	}

	template<typename T>
	template<typename T2>
	__forceinline
	Ptr<T>::Ptr(const Ptr<T2>& r) : p_(NULL)
	{
		*this = r;
	}

	template<typename T>
	Ptr<T>::Ptr(const PtrAllocHint&) : p_(NULL)
	{
		New();
	}

	template<typename T>
	template<TARG1>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG1) : p_(NULL)
	{
		New(PARG1);
	}

	template<typename T>
	template<TARG2>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG2) : p_(NULL)
	{
		New(PARG2);
	}

	template<typename T>
	template<TARG3>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG3) : p_(NULL)
	{
		New(PARG3);
	}

	template<typename T>
	template<TARG4>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG4) : p_(NULL)
	{
		New(PARG4);
	}

	template<typename T>
	template<TARG5>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG5) : p_(NULL)
	{
		New(PARG5);
	}

	template<typename T>
	template<TARG6>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG6) : p_(NULL)
	{
		New(PARG6);
	}

	template<typename T>
	template<TARG7>
	Ptr<T>::Ptr(const PtrAllocHint&, FARG7) : p_(NULL)
	{
		New(PARG7);
	}

	template<typename T>
	__forceinline
	Ptr<T>::~Ptr()
	{
		VALIDATE_PTR(*this);
		Release();
	}

	template<typename T>
	Ptr<T>&
	Ptr<T>::New()
	{
		Release();
		Alloc();
		new (p_) T;

		return *this;
	}

	template<typename T>
	template<TARG1>
	Ptr<T>&
	Ptr<T>::New(FARG1)
	{
		Release();
		Alloc();
		new (p_) T(PARG1);

		return *this;
	}

	template<typename T>
	template<TARG2>
	Ptr<T>&
	Ptr<T>::New(FARG2)
	{
		Release();
		Alloc();
		new (p_) T(PARG2);

		return *this;
	}

	template<typename T>
	template<TARG3>
	Ptr<T>&
	Ptr<T>::New(FARG3)
	{
		Release();
		Alloc();
		new (p_) T(PARG3);

		return *this;
	}

	template<typename T>
	template<TARG4>
	Ptr<T>&
	Ptr<T>::New(FARG4)
	{
		Release();
		Alloc();
		new (p_) T(PARG4);

		return *this;
	}

	template<typename T>
	template<TARG5>
	Ptr<T>&
	Ptr<T>::New(FARG5)
	{
		Release();
		Alloc();
		new (p_) T(PARG5);

		return *this;
	}

	template<typename T>
	template<TARG6>
	Ptr<T>&
	Ptr<T>::New(FARG6)
	{
		Release();
		Alloc();
		new (p_) T(PARG6);

		return *this;
	}

	template<typename T>
	template<TARG7>
	Ptr<T>&
	Ptr<T>::New(FARG7)
	{
		Release();
		Alloc();
		new (p_) T(PARG7);

		return *this;
	}

	template<typename T>
	__forceinline bool
	Ptr<T>::IsNull() const
	{
		return p_ == NULL;
	}

	template<typename T>
	__forceinline
	Ptr<T>::operator const T*() const
	{
		VALIDATE_PTR(*this);
		return p_;
	}

	template<typename T>
	__forceinline
	Ptr<T>::operator T*()
	{
		VALIDATE_PTR(*this);
		return p_;
	}

	template<typename T>
	__forceinline const T*
	Ptr<T>::operator->() const
	{
		VALIDATE_PTR(*this);
		return p_;
	}

	template<typename T>
	__forceinline T*
	Ptr<T>::operator->()
	{
		VALIDATE_PTR(*this);
		return p_;
	}

	template<typename T>
	__forceinline const T&
	Ptr<T>::operator*() const
	{
		VALIDATE_PTR(*this);
		return *operator->();
	}

	template<typename T>
	__forceinline T&
	Ptr<T>::operator*()
	{
		VALIDATE_PTR(*this);
		return *operator->();
	}

	template<typename T>
	__forceinline bool
	Ptr<T>::operator==(const Ptr<T>& r) const
	{
		return p_ == r.p_;
	}

	template<typename T>
	__forceinline void
	Ptr<T>::Release()
	{
		VALIDATE_PTR(*this);
		if (p_ != NULL)
		{
			if (ReleaseRef() == 0)
				Free();

			p_ = NULL;
		}
	}

	template<typename T>
	__forceinline Ptr<T>&
	Ptr<T>::operator=(const Ptr& r)
	{
		VALIDATE_PTR(r);
		VALIDATE_PTR(*this);

		if (p_ != r.p_)
		{
			Release();
			p_ = r.p_;
			if (p_ != NULL)
				AddRef();
		}

		return *this;
	}

	template<typename T>
	__forceinline Ptr<T>&
	Ptr<T>::operator=(T* p)
	{
		VALIDATE_PTR(*this);

		if (p_ != p)
		{
			Release();
			if (CheckSig(p))
			{
				p_ = p;
				AddRef();
			}
			else
			{
				if (p != NULL)
					CHECK(0, eCheckAlways, _W("Ptr에서 생성 되지 않은 포인터는 Ptr에 넣지 마세요."));
			}
		}

		return *this;
	}

	template<typename T>
	__forceinline LONG
	Ptr<T>::GetRefCount() const
	{
		return *_rc<const LONG*>((_rc<const U8*>(p_) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iRefCount_)));
	}

	template<typename T>
	void
	Ptr<T>::Alloc()
	{
		//++iTotalAlloc_;
		//++iCurAlloc_;

		//if ((iCurAlloc_ % 1000) == 0 || (iTotalAlloc_ % 1000) == 0)
		//{
		//	Char	sBuf[1024];
		//	::_stprintf_s(sBuf, _T("Ptr alloc : total %u, cur %u, %s\n"), iTotalAlloc_, iCurAlloc_, _T(__FUNCTION__));
		//	::OutputDebugString(sBuf);
		//}

		Block*	pBlock = _rc<Block*>(::malloc(sizeof(Block)));

		if (pBlock == NULL)		Throw<SystemError>(E_OUTOFMEMORY);

		pBlock->iHead_ = 'head';
#if (_J9_PTR_DIAGNOSTIC)
		pBlock->iObjSize_ = sizeof(T);
#endif
		pBlock->iRefCount_ = 1;
#if (_J9_PTR_DIAGNOSTIC)
		pBlock->iTail_ = 'tail';
#endif

		p_ = _rc<T*>(pBlock->cT_);
	}

	template<typename T>
	void
	Ptr<T>::Free()
	{
		//--iCurAlloc_;

		//::OutputDebugString(_T("Ptr free : "));
		//::OutputDebugString(_T(__FUNCTION__));
		//::OutputDebugString(_T("\n"));

		p_->~T();
		::free(_rc<U8*>(p_) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iHead_));
	}

	template<typename T>
	__forceinline LONG
	Ptr<T>::AddRef()
	{
		VALIDATE_PTR(*this);
		//LOG(J9::String().Format(_T("Add ref : cur ref count %s, %u\r\n"), _T(__FUNCTION__), GetRefCount() + 1));
		return ::InterlockedIncrement(_rc<LONG*>(_rc<U8*>(p_) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iRefCount_)));
		//return ++(*_rc<Size*>(_rc<U8*>(p_) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iRefCount_)));
	}

	template<typename T>
	__forceinline LONG
	Ptr<T>::ReleaseRef()
	{
		VALIDATE_PTR(*this);
		//LOG(J9::String().Format(_T("Release ref : cur ref count %s, %u\r\n"), _T(__FUNCTION__), GetRefCount() - 1));
		return ::InterlockedDecrement(_rc<LONG*>(_rc<U8*>(p_) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iRefCount_)));
		//return --(*_rc<Size*>(_rc<U8*>(p_) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iRefCount_)));
	}

	template<typename T>
	__forceinline bool
	Ptr<T>::IsValid() const
	{
		return (p_ == NULL) || (GetHead(p_) == 'head' && GetTail(p_) == 'tail' && (GetRefCount() > 0));
	}

	template<typename T>
	__forceinline bool
	Ptr<T>::CheckSig(const T* p)
	{
		return p != NULL && GetHead(p) == 'head' && GetTail(p) == 'tail';
	}

	template<typename T>
	__forceinline U32
	Ptr<T>::GetHead(const T* p)
	{
		return *_rc<const U32*>(_rc<const U8*>(p) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iHead_));
	}

	template<typename T>
	__forceinline Size
	Ptr<T>::GetObjSize(const T* p)
	{
		return *_rc<const Size*>((_rc<const U8*>(p) - OFFSET_OF(Block, cT_) + OFFSET_OF(Block, iObjSize_)));
	}

	template<typename T>
	__forceinline U32
	Ptr<T>::GetTail(const T* p)
	{
#if (_J9_PTR_DIAGNOSTIC)
		return *_rc<const U32*>(_rc<const U8*>(p) + GetObjSize(p));
#else
		UNUSED(p);
		return 'tail';
#endif
	}
};

#endif//__J9_PTR_INL__