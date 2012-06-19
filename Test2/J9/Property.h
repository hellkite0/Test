// $Id: Property.h 5209 2010-01-18 09:18:12Z hyojin.lee $

#ifndef __J9_PROPERTY_H__
#define __J9_PROPERTY_H__

#include "Tokens.h"
#include "XML.h"

namespace J9
{
	#define MAKECONFIGPATH(pDir, pFile) J9::StringUtil::Format(_W("%s%s"), pDir, pFile).c_str()
	template<typename Type>
	void cCONVERTEXIST(XMLNode cNode, const WChar* pName, Type& rArg)
	{
		if (!cNode.GetAttribute(pName).IsNull())
			J9::StringUtil::Convert(cNode.GetAttribute(pName).GetValue(), rArg);
	}
	//#define cCONVERTEXIST(cNode, sName, cArg) if (!cNode.GetAttribute(sName).IsNull()) J9::StringUtil::Convert(cNode.GetAttribute(sName).GetValue(), cArg);
	template<typename Type>
	void pCONVERTEXIST(XMLNode* pNode, const WChar* pName, Type& rArg)
	{
		if (!pNode->GetAttribute(pName).IsNull())
			J9::StringUtil::Convert(pNode->GetAttribute(pName).GetValue(), rArg);
	}
	//#define pCONVERTEXIST(pNode, sName, cArg) if (!pNode->GetAttribute(sName).IsNull()) pNode->GetAttribute(sName).GetValue().ConvertTo(cArg);
	#define cGET(cNode, sName) cNode.GetAttribute(sName).GetValue()
	#define pGET(pNode, sName) pNode->GetAttribute(sName).GetValue()
	#define pCONVERT(pNode, sName, cArg) J9::StringUtil::Convert(pNode->GetAttribute(sName).GetValue(), cArg)
	#define cCONVERT(cNode, sName, cArg) J9::StringUtil::Convert(cNode.GetAttribute(sName).GetValue(), cArg)
	#define pPUT(pNode, sName, cArg) if (cArg) { J9::XMLAttribute cAttr = pNode->GetAttribute(sName, true); cAttr = cArg; }
	#define cPUT(cNode, sName, cArg) if (cArg) { J9::XMLAttribute cAttr = cNode.GetAttribute(sName, true); cAttr = cArg; }
	#define pPUTZ(pNode, sName, cArg) { J9::XMLAttribute cAttr = pNode->GetAttribute(sName, true); cAttr = cArg; }
	#define cPUTZ(cNode, sName, cArg) { J9::XMLAttribute cAttr = cNode.GetAttribute(sName, true); cAttr = cArg; }
	#define pPUTSTRING(pNode, sName, cArg) { J9::XMLAttribute cAttr = pNode->GetAttribute(sName, true); cAttr = cArg; }
	#define cPUTSTRING(cNode, sName, cArg) { J9::XMLAttribute cAttr = cNode.GetAttribute(sName, true); cAttr = cArg; }

	// Property structure를 read할때는 . operator를 사용하고
	// write할때는 -> operator를 사용한다. -> 를 사용함으로써 dirty 필드 자동세팅이 가능하다.

	struct MinMax
	{
		MinMax() : iMax_(0), iMin_(0) {}
		U32 iMax_;
		U32 iMin_;

		bool Parse(J9::XMLNode* pNode)
		{
			iMax_ = pNode->GetChild(Tokens::max::W());
			iMin_ = pNode->GetChild(Tokens::min::W());
			return true;
		}
	};

	class Property
	{
	protected:
		bool	bDirty_;

	public:
		inline void* GetEncodeBegin()			{ return this + sizeof(Property); }
		inline bool IsDirty()					{ return bDirty_; }
		inline void SetDirty(bool b = true)		{ bDirty_ = b; }
		inline void ClearDirty()				{ bDirty_ = false; }
	};

	#define DECL_PROPERTY(TypeName)																			\
	TypeName* operator->()		{ bDirty_ = true; return this; }											\
	void Clear()				{ memset(this, 0, sizeof(TypeName)); }										\
	TypeName()					{ Clear(); }																\
	Size GetAttrSize()			{ return sizeof(TypeName) - sizeof(Property);}								\
	Size Encode(U8 *p)			{ memcpy(p, GetEncodeBegin(), GetAttrSize()); return GetAttrSize(); }		\
	Size Decode(const U8 *p)	{ memcpy(GetEncodeBegin(), p, GetAttrSize()); return GetAttrSize();	}		\
	Size Encode(J9::Packet* p)	{ p->AddWriteOffset(Encode(p->GetWritePointer())); return GetAttrSize(); }	\
	Size Decode(J9::Packet* p)	{ p->AddReadOffset(Decode(p->GetReadPointer())); return GetAttrSize(); }
};

#endif // __J9_PROPERTY_H__
