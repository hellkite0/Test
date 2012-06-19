// $Id: Any.cpp 8979 2011-07-15 03:14:11Z hyojin.lee $

#include "Stdafx.h"
#include "Any.h"

#include "UtilityString.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// AnyHelper
////////////////////////////////////////////////////////////////////////////////
namespace J9
{
	class AnyHelper
	{
		friend class Any;

	private:
		AnyHelper() {}

	private:
		static bool const&			ToBool(const Any* p);
		static U32 const&			ToUnsigned(const Any* p);
		static S32 const&			ToSigned(const Any* p);
		static F32 const&			ToFloat(const Any* p);
		static Ptr<StringW> const&	ToStringW(const Any* p);
		static Ptr<StringA> const&	ToStringA(const Any* p);
		static const void* const&	ToVoidPtr(const Any* p);

		static bool&				ToBool(Any* p);
		static U32&					ToUnsigned(Any* p);
		static S32&					ToSigned(Any* p);
		static F32&					ToFloat(Any* p);
		static Ptr<StringW>&		ToStringW(Any* p);
		static Ptr<StringA>&		ToStringA(Any* p);
		static const void*&			ToVoidPtr(Any* p);
	};
}

bool const&
AnyHelper::ToBool(const Any* p)
{
	ASSERT(p->iType_ == Any::eATBool);
	return *_rc<const bool*>(&p->iUnsigned_);
}

U32 const&
AnyHelper::ToUnsigned(const Any* p)
{
	ASSERT(p->iType_ == Any::eATUnsigned);
	return p->iUnsigned_;
}

S32 const&
AnyHelper::ToSigned(const Any* p)
{
	ASSERT(p->iType_ == Any::eATSigned);
	return p->iSigned_;
}

F32 const&
AnyHelper::ToFloat(const Any* p)
{
	ASSERT(p->iType_ == Any::eATFloat);
	return p->fFloat_;
}

Ptr<StringW> const&
AnyHelper::ToStringW(const Any* p)
{
	ASSERT(p->iType_ == Any::eATStringW);
	return *_rc<const Ptr<StringW>*>(&p->iUnsigned_);
}

Ptr<StringA> const&
AnyHelper::ToStringA(const Any* p)
{
	ASSERT(p->iType_ == Any::eATStringA);
	return *_rc<const Ptr<StringA>*>(&p->iUnsigned_);
}

const void* const&
AnyHelper::ToVoidPtr(const Any* p)
{
	ASSERT(p->iType_ == Any::eATVoidPtr);
	return p->pVoid_;
}

bool&
AnyHelper::ToBool(Any* p)
{
	ASSERT(p->iType_ == Any::eATBool);
	return *_rc<bool*>(&p->iUnsigned_);
}

U32&
AnyHelper::ToUnsigned(Any* p)
{
	ASSERT(p->iType_ == Any::eATUnsigned);
	return p->iUnsigned_;
}

S32&
AnyHelper::ToSigned(Any* p)
{
	ASSERT(p->iType_ == Any::eATSigned);
	return p->iSigned_;
}

F32&
AnyHelper::ToFloat(Any* p)
{
	ASSERT(p->iType_ == Any::eATFloat);
	return p->fFloat_;
}

Ptr<StringW>&
AnyHelper::ToStringW(Any* p)
{
	ASSERT(p->iType_ == Any::eATStringW);
	return *_rc<Ptr<StringW>*>(&p->iUnsigned_);
}

Ptr<StringA>&
AnyHelper::ToStringA(Any* p)
{
	ASSERT(p->iType_ == Any::eATStringA);
	return *_rc<Ptr<StringA>*>(&p->iUnsigned_);
}

const void*&
AnyHelper::ToVoidPtr(Any* p)
{
	ASSERT(p->iType_ == Any::eATVoidPtr);
	return p->pVoid_;
}

////////////////////////////////////////////////////////////////////////////////
// Any
////////////////////////////////////////////////////////////////////////////////
Any Any::cBool_True(_sc<bool>(true));
Any Any::cBool_False(_sc<bool>(false));
Any Any::cNull_Bool(_sc<bool>(false));
Any Any::cNull_Unsigned(_sc<U32>(0));
Any Any::cNull_Signed(_sc<S32>(0));
Any Any::cNull_Float(_sc<F32>(0.0f));
Any Any::cNull_StringW(_W(""));
Any Any::cNull_StringA("");
Any Any::cNull_VoidPtr(_sc<void*>(NULL));

Any::Any(const Any& r)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = r;
}

Any::Any(bool b)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = b;
}


Any::Any(const U32 i)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = i;
}

Any::Any(const S32 i)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = i;
}

Any::Any(const F32 f)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = f;
}

Any::Any(const WChar* p)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = p;
}

Any::Any(const StringW& r)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = r;
}

Any::Any(const AChar* p)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = p;
}

Any::Any(const StringA& r)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = r;
}

Any::Any(const void* p)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = p;
}

Any::Any(PathKeyW s)
: iType_(eATUnsigned), iUnsigned_(0)
{
	*this = _sc<const void*>(s.GetStringPointer());
}

void
Any::Clear()
{
	// 각각의 형에 따라 free한다.
	if (IsAllocatedType())	Free();

	iType_ = eATUnsigned;
	iUnsigned_ = 0;
}

Any&
Any::operator=(const Any& r)
{
	if (iType_ != r.iType_)
		ChangeType(_sc<AnyType>(r.iType_));

	switch (iType_)
	{
	case eATBool:
		AnyHelper::ToBool(this) = r;
		break;
	case eATUnsigned:
		AnyHelper::ToUnsigned(this) = r;
		break;
	case eATSigned:
		AnyHelper::ToSigned(this) = r;
		break;
	case eATFloat:
		AnyHelper::ToFloat(this) = r;
		break;
	case eATStringW:
		*AnyHelper::ToStringW(this) = r;
		break;
	case eATStringA:
		*AnyHelper::ToStringA(this) = r;
		break;
	case eATVoidPtr:
		AnyHelper::ToVoidPtr(this) = r;
		break;
	default:
		ASSERT(0);
		break;
	}

	return *this;
}

Any&
Any::operator=(const bool b)
{
	if (iType_ != eATBool)
		ChangeType(eATBool);

	AnyHelper::ToBool(this) = b;

	return *this;
}

Any&
Any::operator=(const U32 i)
{
	if (iType_ != eATUnsigned)
		ChangeType(eATUnsigned);

	AnyHelper::ToUnsigned(this) = i;

	return *this;
}

Any&
Any::operator=(const S32 i)
{
	if (iType_ != eATSigned)
		ChangeType(eATSigned);

	AnyHelper::ToSigned(this) = i;

	return *this;
}

Any&
Any::operator=(const F32 f)
{
	if (iType_ != eATFloat)
		ChangeType(eATFloat);

	AnyHelper::ToFloat(this) = f;

	return *this;
}

Any&
Any::operator=(const StringW& r)
{
	if (iType_ != eATStringW)
		ChangeType(eATStringW);

	*AnyHelper::ToStringW(this) = r;

	return *this;
}

Any&
Any::operator=(const StringA& r)
{
	if (iType_ != eATStringA)
		ChangeType(eATStringA);

	*AnyHelper::ToStringA(this) = r;

	return *this;
}

Any&
Any::operator=(const void* p)
{
	if (iType_ != eATVoidPtr)
		ChangeType(eATVoidPtr);

	AnyHelper::ToVoidPtr(this) = p;

	return *this;
}

Any::operator bool() const
{
	bool	b = 0;

	switch (iType_)
	{
	case eATBool:
		b = AnyHelper::ToBool(this);
		break;

	case eATUnsigned:
		b = AnyHelper::ToUnsigned(this) != 0;
		break;

	case eATSigned:
		b = AnyHelper::ToSigned(this) != 0;
		break;

	case eATFloat:
		b = _sc<S32>(AnyHelper::ToFloat(this)) != 0.0f;
		break;

	case eATStringW:
		StringUtil::Convert(*AnyHelper::ToStringW(this), b);
		break;

	case eATStringA:
		StringUtil::Convert(*AnyHelper::ToStringA(this), b);
		break;

	case eATVoidPtr:
		ASSERT(0);
		break;

	default:
		ASSERT(0);
		break;
	}

	return b;
}

Any::operator U32() const
{
	U32	i = 0;

	switch (iType_)
	{
	case eATBool:
		i = _sc<S32>(AnyHelper::ToBool(this));
		break;

	case eATUnsigned:
		i = AnyHelper::ToUnsigned(this);
		break;

	case eATSigned:
		i = AnyHelper::ToSigned(this);
		break;

	case eATFloat:
		i = _sc<S32>(AnyHelper::ToFloat(this));
		break;

	case eATStringW:
		StringUtil::Convert(*AnyHelper::ToStringW(this), i);
		break;

	case eATStringA:
		StringUtil::Convert(*AnyHelper::ToStringA(this), i);
		break;

	case eATVoidPtr:
		ASSERT(0);
		break;

	default:
		ASSERT(0);
		break;
	}

	return i;
}

Any::operator S32() const
{
	S32	i = 0;

	switch (iType_)
	{
	case eATBool:
		i = _sc<S32>(AnyHelper::ToBool(this));
		break;

	case eATUnsigned:
		i = AnyHelper::ToUnsigned(this);
		break;

	case eATSigned:
		i = AnyHelper::ToSigned(this);
		break;

	case eATFloat:
		i = _sc<S32>(AnyHelper::ToFloat(this));
		break;

	case eATStringW:
		StringUtil::Convert(*AnyHelper::ToStringW(this), i);
		break;

	case eATStringA:
		StringUtil::Convert(*AnyHelper::ToStringA(this), i);
		break;

	case eATVoidPtr:
		ASSERT(0);
		break;

	default:
		ASSERT(0);
		break;
	}

	return i;
}

Any::operator F32() const
{
	F32	f = 0.0f;

	switch (iType_)
	{
	case eATBool:
		f = _sc<F32>(AnyHelper::ToBool(this));
		break;

	case eATUnsigned:
		f = _sc<F32>(AnyHelper::ToUnsigned(this));
		break;

	case eATSigned:
		f = _sc<F32>(AnyHelper::ToSigned(this));
		break;

	case eATFloat:
		f = AnyHelper::ToFloat(this);
		break;

	case eATStringW:
		StringUtil::Convert(*AnyHelper::ToStringW(this), f);
		break;

	case eATStringA:
		StringUtil::Convert(*AnyHelper::ToStringA(this), f);
		break;

	case eATVoidPtr:
		ASSERT(0);
		break;

	default:
		ASSERT(0);
		break;
	}

	return f;
}

Any::operator StringW() const
{
	StringW	s;

	switch (iType_)
	{
	case eATBool:
		StringUtil::ToString(AnyHelper::ToBool(this), s);
		break;

	case eATUnsigned:
		StringUtil::ToString(AnyHelper::ToUnsigned(this), s);
		break;

	case eATSigned:
		StringUtil::ToString(AnyHelper::ToSigned(this), s);
		break;

	case eATFloat:
		StringUtil::ToString(AnyHelper::ToFloat(this), s);
		break;

	case eATStringW:
		s = *AnyHelper::ToStringW(this);
		break;

	case eATStringA:
		s = StringUtil::ToStringW(*AnyHelper::ToStringA(this));
		break;

	case eATVoidPtr:
		ASSERT(0);
		break;

	default:
		ASSERT(0);
		break;
	}

	return s;
}

Any::operator StringA() const
{
	StringA	s;

	switch (iType_)
	{
	case eATBool:
		StringUtil::ToString(AnyHelper::ToBool(this), s);
		break;

	case eATUnsigned:
		StringUtil::ToString(AnyHelper::ToUnsigned(this), s);
		break;

	case eATSigned:
		StringUtil::ToString(AnyHelper::ToSigned(this), s);
		break;

	case eATFloat:
		StringUtil::ToString(AnyHelper::ToFloat(this), s);
		break;

	case eATStringW:
		s = StringUtil::ToStringA(*AnyHelper::ToStringW(this));
		break;

	case eATStringA:
		s = *AnyHelper::ToStringA(this);
		break;

	case eATVoidPtr:
		ASSERT(0);
		break;

	default:
		ASSERT(0);
		break;
	}

	return s;
}

Any::operator const void*() const
{
	const void*	p = NULL;

	switch (iType_)
	{
	case eATBool:
		ASSERT(0);
		break;

	case eATUnsigned:
		ASSERT(0);
		break;

	case eATSigned:
		ASSERT(0);
		break;

	case eATFloat:
		ASSERT(0);
		break;

	case eATStringW:
		ASSERT(0);
		break;

	case eATStringA:
		ASSERT(0);
		break;

	case eATVoidPtr:
		p = AnyHelper::ToVoidPtr(this);
		break;

	default:
		ASSERT(0);
		break;
	}

	return p;
}

bool
Any::IsNull() const
{
	switch (iType_)
	{
	case eATBool:
		return AnyHelper::ToBool(this) == false;

	case eATUnsigned:
		return AnyHelper::ToUnsigned(this) == 0;

	case eATSigned:
		return AnyHelper::ToSigned(this) == 0;

	case eATFloat:
		return AnyHelper::ToFloat(this) == 0.0f;

	case eATStringW:
		return AnyHelper::ToStringW(this)->empty();

	case eATStringA:
		return AnyHelper::ToStringA(this)->empty();

	case eATVoidPtr:
		return AnyHelper::ToVoidPtr(this) == NULL;

	default:
		ASSERT(0);
		return true;
	}
}

bool
Any::IsAllocatedType() const
{
	return iType_ == eATStringW || iType_ == eATStringA;
}

void
Any::Alloc()
{
	ASSERT(IsAllocatedType());

	switch (iType_)
	{
	case eATStringW:	AllocStringW();		break;
	case eATStringA:	AllocStringA();		break;
	default:			ASSERT(0);			break;
	}
}

void
Any::AllocStringW()
{
	ASSERT(iType_ == eATStringW);
	AnyHelper::ToStringW(this).New();
}

void
Any::AllocStringA()
{
	ASSERT(iType_ == eATStringA);
	AnyHelper::ToStringA(this).New();
}

void
Any::Free()
{
	ASSERT(IsAllocatedType());

	switch (iType_)
	{
	case eATStringW:	FreeStringW();		break;
	case eATStringA:	FreeStringA();		break;
	default:			ASSERT(0);			break;
	}
}

void
Any::FreeStringW()
{
	ASSERT(iType_ == eATStringW);
	AnyHelper::ToStringW(this) = NULL;
}

void
Any::FreeStringA()
{
	ASSERT(iType_ == eATStringA);
	AnyHelper::ToStringA(this) = NULL;
}

void
Any::ChangeType(AnyType iType)
{
	ASSERT(_sc<AnyType>(iType_) != iType);

	if (IsAllocatedType())
		Free();

	iType_ = iType;
	iUnsigned_ = 0;

	if (IsAllocatedType())
		Alloc();
}

bool
Any::operator==(const U32 i) const
{
	if (iType_ == eATUnsigned)
	{
		return AnyHelper::ToUnsigned(this) == i;
	}

	ASSERT(0);
	return false;
}

bool
Any::operator==(const StringW& r) const
{
	if (iType_ == eATStringW)
	{
		return *AnyHelper::ToStringW(this) == r;
	}

	ASSERT(0);
	return false;
}

Any::operator PathKeyW() const
{
	if (iType_ == 4)
	{
		StringW sName = *AnyHelper::ToStringW(this);
		return PathKeyW(sName.c_str());
	}
	ASSERT(iType_ == eATVoidPtr);
	return PathKeyW(pVoid_);
}


////////////////////////////////////////////////////////////////////////////////
// Anys
////////////////////////////////////////////////////////////////////////////////
const Any Anys::cNull_;

void
Anys::Clear()
{
	cMap_.clear();
	cArray_.clear();
}

void
Anys::Add(J9::PathKeyW sName, const Any& rAny)
{
	Size	i = _Add(rAny);
	cMap_[sName] = i;
}

void
Anys::Add(const Any& rAny)
{
	_Add(rAny);
}

const Any&
Anys::Get(J9::PathKeyW sName) const
{
	AnyMap::const_iterator	cIter = cMap_.find(sName);

	if (cIter == cMap_.end())	return cNull_;

	return Get(cIter->second);
}

const Any&
Anys::Get(Size i) const
{
	if (cArray_.size() <= i)	return cNull_;

	return cArray_[i];
}

Size
Anys::_Add(const Any& rAny)
{
	Size	i = cArray_.size();
	cArray_.push_back(rAny);
	return i;
}

