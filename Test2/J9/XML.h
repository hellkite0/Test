// $Id: XML.h 8904 2011-07-12 05:37:08Z donghyuk.lee $

#ifndef __J9_XML_H__
#define __J9_XML_H__

#include "UtilityString.h"
#include "StringKey.h"

struct IXMLDOMNode;
struct IXMLDOMNodeList;
struct IXMLDOMNamedNodeMap;

//#ifdef _SERVER
	#define SUPPORT_XML_ATTRIBUTE
//#endif

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	///	\brief	COM Object�� ����ϱ� ���� CoInitialize�� �Ѵ�.
	///
	///	COM Object�� ����ϱ� ���� CoInitialize�� �Ѵ�. �ٸ� �÷����� ������� �ʰ�
	///	Win32ȯ�濡 �°� ���۵Ǿ���.
	///	\author	hyojin.lee
	///	\date	2005-06-29
	////////////////////////////////////////////////////////////////////////////////
	class COM
	{
		STATIC_SINGLETON(COM);

	public:
		COM();
		~COM();

		bool	Initialize();

	private:
		bool	bInitialized_;
	};

	enum DOMNodeType
	{
		eNodeInvalid				= 0,
		eNodeElement,
		eNodeAttribute,
		eNodeText,
		eNodeCDataSection,
		eEntityReference,
		eEntity,
		eProcessingInstruction,
		eComment,
		eDocument,
		eDocumentType,
		eDocumentFragment,
		eNotation
	};

	class XMLNodeList;
#ifdef SUPPORT_XML_ATTRIBUTE
	class XMLAttribute;
	class XMLAttributeMap;
#endif
	class XMLNode
	{
		friend class XMLNodeList;

	public:
		XMLNode();
		XMLNode(IXMLDOMNode* pNode);
		XMLNode(const XMLNode& r);
		XMLNode(const WChar* p, bool bPreserveWhiteSpace = true);

		~XMLNode();

		XMLNode&		operator=(const XMLNode& r);
		bool			operator==(const XMLNode& r) const { return pNode_ == r.pNode_; }
		bool			operator!=(const XMLNode& r) const { return !operator==(r); }

		bool			IsNull() const { return pNode_ == NULL; }
		void			Clear();

		bool			Load(const WChar* pFileName);
		bool			Load(const AChar* pFileName);
		bool			Save(const WChar* pFileName, bool bIndent = true, bool bLineBreak = true);

		bool			Copy(const XMLNode& r);
		bool			Create(const WChar* pName, bool bPreserveWhiteSpace = true);
		bool			Parse(const WChar* pData, bool bPreserveWhiteSpace = true);
		bool			Write(StringW& rData) const;

		bool			IsPreserveWhiteSpace() const;

		bool			GetName(StringW& _strName) const;
		StringW			GetName() const;

		bool			GetNodeName(StringW& _strName) const;
		StringW			GetNodeName() const;

		bool			GetType(DOMNodeType& _eType) const;
		DOMNodeType 	GetType() const;

		bool			GetData(StringW& rData) const;
		StringW			GetData() const;
		bool			CompareData(const WChar* pData) const;
		bool			CompareDataI(const WChar* pData) const;

		//template<typename ConvertType>
		//operator ConvertType() const	{ ConvertType c; StringUtil::Convert(GetData(), c); return c; }
		operator bool() const			{ bool c; StringUtil::Convert(GetData(), c); return c; }
		operator S8() const				{ S8 c; StringUtil::Convert(GetData(), c); return c; }
		operator S16() const			{ S16 c; StringUtil::Convert(GetData(), c); return c; }
		operator S32() const			{ S32 c; StringUtil::Convert(GetData(), c); return c; }
		operator S64() const			{ S64 c; StringUtil::Convert(GetData(), c); return c; }
		operator U8() const				{ U8 c; StringUtil::Convert(GetData(), c); return c; }
		operator U16() const			{ U16 c; StringUtil::Convert(GetData(), c); return c; }
		operator U32() const			{ U32 c; StringUtil::Convert(GetData(), c); return c; }
		operator U64() const			{ U64 c; StringUtil::Convert(GetData(), c); return c; }
		operator F32() const			{ F32 c; StringUtil::Convert(GetData(), c); return c; }
		operator F64() const			{ F64 c; StringUtil::Convert(GetData(), c); return c; }
		operator StringW() const		{ return GetData(); }
		operator StringA() const		{ return StringUtil::ToStringA(GetData()); }

		template<typename DataType>
		const XMLNode&		operator>>(DataType& t) const { if (!IsNull()) t = *this; return *this; }

		//template<typename DataType>
		//XMLNode&		operator=(typename ArgType<DataType>::Type t) { StringW s; StringUtil::ToString(t, s); SetData(s.c_str()); }
		XMLNode&		operator=(bool b)			{ StringW s; StringUtil::ToString(b, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(S8 i)				{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(S16 i)			{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(S32 i)			{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(S64 i)			{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(U8 i)				{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(U16 i)			{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(U32 i)			{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(U64 i)			{ StringW s; StringUtil::ToString(i, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(F32 f)			{ StringW s; StringUtil::ToString(f, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(F64 f)			{ StringW s; StringUtil::ToString(f, s); SetData(s.c_str()); return *this; }
		XMLNode&		operator=(const StringW& r)	{ SetData(r.c_str()); return *this; }
		XMLNode&		operator=(const StringA& r)	{ SetData(StringUtil::ToStringW(r).c_str()); return *this; }

		template<typename DataType>
		XMLNode&		operator<<(const DataType& t) { *this = t; return *this; }

		bool			SetData(const WChar* pData);

#ifdef SUPPORT_XML_ATTRIBUTE
		// attributes
		Size			GetAttributeNum() const;
		XMLAttributeMap	GetAttributeMap(Size* pSize = NULL) const;

		XMLAttribute	GetAttribute(const WChar* pKey, bool bCreate = false);
		XMLAttribute	GetAttribute(Size iIndex) const;
		bool			RemoveAttribute(const WChar* pKey);
#endif

		// child nodes
		XMLNode			GetParent() const;
		XMLNode			GetPrevSibling() const;
		XMLNode			GetNextSibling() const;
		XMLNode			GetFirstChild() const;
		XMLNode			GetLastChild() const;

		bool			HasChild() const;
		Size			GetChildNum() const;
		XMLNodeList		GetChildList(Size* pSize = NULL) const;
		XMLNodeList		GetChildList(const WChar* pKey, Size* pSize = NULL) const;

		XMLNode			GetChild(const WChar* pKey) const;
		XMLNode			GetChildWithAttribute(const WChar* pKey, const WChar* pAttrName, const WChar* pAttrValue) const;
		XMLNode			GetChildWithChlid(const WChar* pKey, const WChar* pChildName, const WChar* pChildValue) const;
		XMLNode			CreateChild(const WChar* pKey, bool bMultiple = false);
		bool			AppendChild(XMLNode& rChild);
		void			RemoveChild(const WChar* pKey, bool bMultiple = false);
		void			RemoveChild(XMLNode& rChild);
		bool			ReplaceChild(XMLNode& rOld, XMLNode& rNew);

		XMLNode			operator[](const WChar* pKey);
		XMLNode			operator[](const WChar* pKey) const;

		IXMLDOMNode*	GetNode() const  { return pNode_; }
		PathKeyW		GetPath() const { return sPath_; }

#ifdef SUPPORT_XML_ATTRIBUTE
		// utility
		bool			Attribute2Child(XMLNode cNewXML);
#endif

	private:
		void			Set(IXMLDOMNode* p);

	private:
		IXMLDOMNode*	pNode_;
		PathKeyW	sPath_;
	};


	template<typename IO>
	struct TypeTraits_Binary_Custom<IO, XMLNode>
	{
		static Size
		Write(IO* pIO, const XMLNode& rNode)
		{
			Size	iWritten = 0;

			//����� �̸��� �����Ѵ�.
			iWritten += TypeTraits_Binary::Write(pIO, rNode.GetName());

			//Attribute �� ������ �����Ѵ�.
			U32 iAttributeNum = _sc<U32>(rNode.GetAttributeNum());
			iWritten += TypeTraits_Binary_Primitive<IO, U32>::Write(pIO, iAttributeNum);

			for (U32 iloop = 0; iloop < iAttributeNum; ++iloop)
			{
				XMLAttribute cTempAttribute = rNode.GetAttribute(iloop);
				//Attribute�̸��� �����Ѵ�.
				iWritten += TypeTraits_Binary::Write(pIO, cTempAttribute.GetName());
				//Attribute���� �����Ѵ�.
				iWritten += TypeTraits_Binary::Write(pIO, cTempAttribute.GetValue());
			}

			XMLNodeList		cChildList = rNode.GetChildList();

			//�ڽ��� ������ ���Ѵ�.(Ŀ��Ʈ�� �ִ�)
			U32 iTempChildNum = _sc<U32>(rNode.GetChildNum());
			U32 iRealChildNum = 0;

			XMLNode			cTempNode;
			DOMNodeType		cType;

			//�ڸ�Ʈ�� �ڽİ������� ���ܽ�Ų��.
			for (U32 iloop = 0; iloop < iTempChildNum; ++iloop)
			{
				cTempNode = cChildList.Get(iloop);
				cType	  = cTempNode.GetType();
				if ((cType != eComment) && (cType != eNodeText))
					++iRealChildNum;
			}

			//�ڽİ����� �����Ѵ�.
			iWritten += TypeTraits_Binary_Primitive<IO, U32>::Write(pIO, iRealChildNum);

			for (U32 iloop = 0; iloop < iTempChildNum; ++iloop)
			{
				//�ڽİ�����ŭ Save������ �ݺ��Ѵ�.
				cTempNode = cChildList.Get(iloop);
				cType	  = cTempNode.GetType();
				if ((cType != eComment) && (cType != eNodeText))
					iWritten += Write(pIO,cTempNode);
			}

			return iWritten;
		}
		static Size
		Read(IO* pIO, XMLNode& rNode)
		{
			Size	iRead = 0;

			StringW sStrBuf;

			//��尡 ��������� �װ� ���� ó�� ����� �Ҹ���.(�׿����� ó���� ���ش�.)
			if (rNode.IsNull())
			{
				//���� ����̸��� �о���δ�.
				iRead += TypeTraits_Binary::Read(pIO, sStrBuf);
				//��带 �����Ѵ�.
				rNode.Create(sStrBuf);
			}

			//Attribute������ �д´�.
			U32 iAttriNum;
			iRead += TypeTraits_Binary_Primitive<IO, U32>::Read(pIO, iAttriNum);

			//Attribute������ŭ �ݺ��ϸ鼭 Attribute�� �а� �����ϰ� �� �Ѵ�.
			for (U32 iloop = 0; iloop < iAttriNum; ++iloop)
			{
				//���� Attribute �� �̸��� �о�´�.
				iRead += TypeTraits_Binary::Read(pIO, sStrBuf);
				//���̸����� Attribute�� �����Ѵ�.
				XMLAttribute cTempAttribute = rNode.GetAttribute(sStrBuf, true);
				//���� Attribute�� ���� �о���δ�.
				iRead += TypeTraits_Binary::Read(pIO, sStrBuf);
				//�� ���� Attribute�� �������ش�.
				cTempAttribute.SetValue(sStrBuf);
			}

			//�ڽĳ�� ������ �о���δ�.
			U32 iChildNum;
			iRead += TypeTraits_Binary_Primitive<IO, U32>::Read(pIO, iChildNum);

			//�ڽĳ�� ������ŭ ��带 �����ϰ� �� ��带 �д� ������ Load�� ���ư���.
			for (iloop = 0; iloop < iChildNum; ++iloop)
			{
				XMLNode cTempChildNode;
				//���� ����̸��� �о���δ�.
				iRead += TypeTraits_Binary::Read(pIO, sStrBuf);
				//����� �ڽĳ�带 �����ϰ� �� �ٽ� ��� ��������� �ݺ��Ѵ�.
				cTempChildNode = rNode.CreateChild(sStrBuf, true);
				iRead += Read(pIO, cTempChildNode);
			}
			return iRead;
		}
	};


	class XMLNodeList
	{
		friend class XMLNode;

	public:
		XMLNodeList(const XMLNodeList& r);
		~XMLNodeList();

		XMLNodeList&		operator=(const XMLNodeList& r);

		inline bool			IsEmpty() const { return pList_ == NULL; }
		Size				GetSize() const;

		XMLNode				Get(Size iIndex) const;
#ifdef SUPPORT_XML_ATTRIBUTE
		XMLNode				Find(const WChar* pAttrName, const WChar* pAttrValue, Size* pIndex = NULL);
		XMLNode				FindWithAttribute(const WChar* pAttrName, const WChar* pAttrValue, Size* pIndex = NULL);
#endif
		XMLNode				FindWithChild(const WChar* pChildName, const WChar* pChildValue, Size* pIndex = NULL);
		XMLNode				FindWithChildI(const WChar* pChildName, const WChar* pChildValue, Size* pIndex = NULL);

	private:
		XMLNodeList();
		XMLNodeList(IXMLDOMNodeList* pList);

		void				Clear();
		void				Set(IXMLDOMNodeList* p);

	private:
		IXMLDOMNodeList*	pList_;
	};

#ifdef SUPPORT_XML_ATTRIBUTE
	class XMLAttribute
	{
		friend class XMLNode;
		friend class XMLAttributeMap;

	public:
		XMLAttribute();
		XMLAttribute(const XMLAttribute& r);
		~XMLAttribute();

		XMLAttribute&	operator=(const XMLAttribute& r);

		bool			IsNull() const { return pNode_ == NULL; }
		void			Clear();

		bool			GetName(StringW& rName) const;
		StringW			GetName() const;

		bool			GetType(DOMNodeType& rType) const;
		DOMNodeType 	GetType() const;

		bool			GetValue(StringW& rValue) const;
		StringW			GetValue() const;
		bool			SetValue(const WChar* pValue);

		operator S8() const { S8 c; StringUtil::Convert(GetValue(), c); return c; }
		operator S16() const { S16 c; StringUtil::Convert(GetValue(), c); return c; }
		operator S32() const { S32 c; StringUtil::Convert(GetValue(), c); return c; }
		operator S64() const { S64 c; StringUtil::Convert(GetValue(), c); return c; }
		operator U8() const { U8 c; StringUtil::Convert(GetValue(), c); return c; }
		operator U16() const { U16 c; StringUtil::Convert(GetValue(), c); return c; }
		operator U32() const { U32 c; StringUtil::Convert(GetValue(), c); return c; }
		operator U64() const { U64 c; StringUtil::Convert(GetValue(), c); return c; }
		operator F32() const { F32 c; StringUtil::Convert(GetValue(), c); return c; }
		operator F64() const { F64 c; StringUtil::Convert(GetValue(), c); return c; }
		operator bool() const { bool c; StringUtil::Convert(GetValue(), c); return c; }
		operator StringW() const { return GetValue(); }
		operator StringA() const { return StringUtil::ToStringA(GetValue()); }

		template<typename SourceType>
		XMLAttribute& operator=(const SourceType& r)
		{
			StringW	s;
			StringUtil::ToString(r, s);
			SetValue(s.c_str());
			return *this;
		}

		XMLAttribute& operator=(const StringW& r)
		{
			SetValue(r.c_str());
			return *this;
		}

		XMLAttribute& operator=(const WChar* p)
		{
			SetValue(p);
			return *this;
		}

	private:
		XMLAttribute(IXMLDOMNode* pNode);

		void			Set(IXMLDOMNode* p);

	private:
		IXMLDOMNode*	pNode_;
	};

	class XMLAttributeMap
	{
		friend class XMLNode;

	public:
		XMLAttributeMap(const XMLAttributeMap& r);
		~XMLAttributeMap();

		XMLAttributeMap&	operator=(const XMLAttributeMap& r);

	public:
		bool				IsEmpty() const { return pMap_ == NULL; }
		Size				GetSize() const;

		XMLAttribute		Get(const WChar* pKey);
		XMLAttribute		Get(Size iIndex) const;

		XMLAttribute		Pop(const WChar* pKey);
		XMLAttribute		Pop(Size iIndex);

	private:
		XMLAttributeMap(IXMLDOMNamedNodeMap* pMap);

		void				Clear();
		void				Set(IXMLDOMNamedNodeMap* pMap);
		bool				Remove(const WChar* pKey);

	private:
		IXMLDOMNamedNodeMap*	pMap_;
	};
#endif
};

#endif//__J9_XML_H__