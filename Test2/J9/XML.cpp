// $Id: XML.cpp 8980 2011-07-15 05:18:21Z donghyuk.lee $

#include "Stdafx.h"
#include "XML.h"

#include <comutil.h>
//_bstr_t를 _cc<WChar*>로 치환하여
//외부 모듈 종속성을 최소화했다.
//안정성검증이 필요하다.
//#pragma comment(lib, "comsupp.lib")

#include "UtilityMisc.h"
#include "UtilityFile.h"
#include "UtilityString.h"

using namespace J9;

COM::COM()
{
}

COM::~COM()
{
	CoUninitialize();
	bInitialized_ = false;
}

bool
COM::Initialize()
{
#ifndef _JUST9_MFC
	if (bInitialized_)	return true;
#endif

#ifdef WIN32
#ifdef _JUST9_MFC
	HRESULT		hr;

	#ifdef _MT
		U32	iMT = COINIT_MULTITHREADED;
	#else
		U32	iMT = COINIT_APARTMENTTHREADED;
	#endif

	iMT = COINIT_APARTMENTTHREADED;

	// COM을 초기화 한다.
	hr = CoInitializeEx(NULL, iMT);

	bInitialized_ = (hr == S_OK) || (hr == S_FALSE);
#else
	if (bInitialized_)	return true;

	HRESULT		hr;

	#ifdef _MT
		U32	iMT = COINIT_MULTITHREADED;
	#else
		U32	iMT = COINIT_APARTMENTTHREADED;
	#endif

	// COM을 초기화 한다.
	hr = CoInitializeEx(NULL, iMT);

	if (hr == RPC_E_CHANGED_MODE)
	{
		// 두가지 모드로 다 시도해 본다.
		// 물론 땜빵이다. 일단 3ds max exporter에서 돌아가게 하기 위해서.
		iMT = iMT == COINIT_MULTITHREADED ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED;
		hr = CoInitializeEx(NULL, iMT);
	}

	bInitialized_ = (hr == S_OK) || (hr == S_FALSE);
#endif
#else
	bInitialized_ = false;
#endif

	return bInitialized_;
}

////////////////////////////////////////////////////////////////////////////////
///	XMLDocument
////////////////////////////////////////////////////////////////////////////////
class XMLDocument
{
public:
	XMLDocument();
	XMLDocument(IXMLDOMNode* pNode);

	~XMLDocument();

	void				Clear();
	IXMLDOMDocument*	GetDocument() const { ASSERT(pDoc_); return pDoc_; }

	IXMLDOMNode*		CreateNode(const WChar* pName);
	IXMLDOMNode*		CreateAttribute(const WChar* pName);

private:
	IXMLDOMNode*		Create(VARIANT cType, const WChar* pName);

private:
	IXMLDOMDocument*	pDoc_;
};

XMLDocument::XMLDocument()
: pDoc_(NULL)
{
	VERIFY(COM::Instance()->Initialize());

	IXMLDOMDocument*	pDoc;
	HRESULT				hr;

	hr = ::CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, _rc<void**>(&pDoc));

	if (hr == S_OK && pDoc != NULL)
		pDoc_ = pDoc;
	else
		ASSERT(false);
}

XMLDocument::XMLDocument(IXMLDOMNode* pNode)
: pDoc_(NULL)
{
	VERIFY(COM::Instance()->Initialize());

	IXMLDOMDocument*	pDoc;
	HRESULT				hr;

	hr = pNode->get_ownerDocument(&pDoc);
	if (hr == S_OK)
		pDoc_ = pDoc;
}

XMLDocument::~XMLDocument()
{
	Clear();
}

void
XMLDocument::Clear()
{
	if (pDoc_ != NULL)
	{
		pDoc_->Release();
		pDoc_ = NULL;
	}
}

IXMLDOMNode*
XMLDocument::CreateNode(const WChar* pName)
{
	return Create(_variant_t(NODE_ELEMENT), pName);
}

IXMLDOMNode*
XMLDocument::CreateAttribute(const WChar* pName)
{
	return Create(_variant_t(NODE_ATTRIBUTE), pName);
}

IXMLDOMNode*
XMLDocument::Create(VARIANT cType, const WChar* pName)
{
	VERIFY(COM::Instance()->Initialize());
	ASSERT(pDoc_ != NULL);

	HRESULT			hr;
	IXMLDOMNode*	pNode;

	hr = pDoc_->createNode(cType, _cc<WChar*>(pName), NULL, &pNode);
	if (hr != S_OK)		return NULL;

	return pNode;
}

////////////////////////////////////////////////////////////////////////////////
///	XMLNode
////////////////////////////////////////////////////////////////////////////////

XMLNode::XMLNode()
: pNode_(NULL)
{
}

XMLNode::XMLNode(IXMLDOMNode* pNode)
: pNode_(NULL)
{
	Set(pNode);
}

XMLNode::XMLNode(const XMLNode& r)
: pNode_(NULL)
{
	*this = r;
}

XMLNode::XMLNode(const WChar* p, bool bPreserveWhiteSpace)
: pNode_(NULL)
{
	Parse(p, bPreserveWhiteSpace);
}

XMLNode::~XMLNode()
{
	Clear();
}

XMLNode& XMLNode::operator=(const XMLNode& r)
{
	Clear();

	pNode_ = r.pNode_;

	if (pNode_ != NULL)
		pNode_->AddRef();

	return *this;
}

void
XMLNode::Clear()
{
	if (pNode_ != NULL)
	{
		pNode_->Release();
		pNode_ = NULL;
	}
}

void
XMLNode::Set(IXMLDOMNode* p)
{
	Clear();

	if (p == NULL)	return;

	pNode_ = p;
	pNode_->AddRef();
}

bool
XMLNode::Copy(const XMLNode& r)
{
	VERIFY(COM::Instance()->Initialize());

	Clear();

	if (r.pNode_ == NULL)	return false;

	HRESULT	hr = r.pNode_->cloneNode(VARIANT_TRUE, &pNode_);
	if (hr != S_OK)			return false;

	return true;
}

bool
XMLNode::Create(const WChar* pName, bool bPreserveWhiteSpace)
{
	VERIFY(COM::Instance()->Initialize());

	Clear();

	XMLDocument			cDoc;
	IXMLDOMDocument*	pDoc = cDoc.GetDocument();

	// 생성 OK!!
	pDoc->put_async(VARIANT_FALSE);
	pDoc->put_validateOnParse(VARIANT_FALSE);
	pDoc->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL	cPreserveWhiteSpace = bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE;
	pDoc->put_preserveWhiteSpace(cPreserveWhiteSpace);

	pNode_ = cDoc.CreateNode(pName);
	return true;
}

bool
XMLNode::Parse(const WChar* pData, bool bPreserveWhiteSpace)
{
	VERIFY(COM::Instance()->Initialize());

	Clear();

	if (!pData || wcslen(pData) == 0)	return false;

	StringW	sData(pData);

	// IXMLDOMDocument::loadXML은 제한된 인코딩 모드만을 지원한다
	// 따라서 <? ... ?> 타입의 태그는 모두 제거해 줘야 한다
	while (Always(true))
	{
		Size	iFoundLeft = sData.find(_W("<?"));
		if (iFoundLeft == StringW::npos)	break;

		Size	iFoundRight = sData.find(_W("?>"));
		if (iFoundRight == StringW::npos)	return false;

		if (iFoundRight < iFoundLeft)		return false;

		sData.erase(iFoundLeft, iFoundRight - iFoundLeft + 2);
	}

	XMLDocument			cDoc;
	IXMLDOMDocument*	pDoc = cDoc.GetDocument();

	pDoc->put_async(VARIANT_FALSE);
	pDoc->put_validateOnParse(VARIANT_FALSE);
	pDoc->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL	cPreserveWhiteSpace = bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE;
	pDoc->put_preserveWhiteSpace(cPreserveWhiteSpace);

	HRESULT				hr;
	VARIANT_BOOL		bSuccess;

	hr = pDoc->loadXML(_cc<WChar*>(sData.c_str()), &bSuccess);
	if (!bSuccess || hr != S_OK)
		return false;

	// 로드 & 파싱은 성공했다. 이제 첫번째 엘리먼트 노드를 구해 세팅한다.
	hr = pDoc->selectSingleNode(_cc<WChar*>(_T("*")), &pNode_);
	if (hr != S_OK)					return false;

	return true;
}

bool
XMLNode::Write(StringW& rData) const
{
	VERIFY(COM::Instance()->Initialize());

	if (pNode_ == NULL)		return false;

	BSTR		bstrData = NULL;
	HRESULT		hr = pNode_->get_xml(&bstrData);
	if (hr != S_OK)			return false;

	rData = bstrData;
	::SysFreeString(bstrData);

	return true;
}

bool
XMLNode::IsPreserveWhiteSpace() const
{
	if (pNode_ == NULL)	return false;

	XMLDocument	cDoc(pNode_);

	VARIANT_BOOL	bPreserve = VARIANT_FALSE;
	cDoc.GetDocument()->get_preserveWhiteSpace(&bPreserve);

	return bPreserve != VARIANT_FALSE;
}

bool
XMLNode::GetName(StringW& rName) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			bstrName = NULL;

	hr = pNode_->get_nodeName(&bstrName);
	if (hr != S_OK)			return false;

	rName = bstrName;
	::SysFreeString(bstrName);
	return true;
}

StringW
XMLNode::GetName() const
{
	StringW	sRet;
	GetName(sRet);
	return sRet;
}


bool
XMLNode::GetNodeName(StringW& rName) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			bstrName = NULL;

	hr = pNode_->get_baseName(&bstrName);
	if (hr != S_OK)			return false;

	rName = bstrName;
	::SysFreeString(bstrName);
	return true;
}

StringW
XMLNode::GetNodeName() const
{
	StringW	sRet;
	GetNodeName(sRet);
	return sRet;
}

bool
XMLNode::GetType(J9::DOMNodeType& rType) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	::DOMNodeType	eType;

	hr = pNode_->get_nodeType(&eType);
	if (hr != S_OK)			return false;

	rType = static_cast<DOMNodeType>(eType);
	return true;
}

J9::DOMNodeType
XMLNode::GetType() const
{
	DOMNodeType	eRet;
	GetType(eRet);
	return eRet;
}

bool
XMLNode::GetData(StringW& rData) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			bstrData = NULL;

	hr = pNode_->get_text(&bstrData);
	if (hr != S_OK)			return false;

	rData = bstrData;
	::SysFreeString(bstrData);
	return true;
}

StringW
XMLNode::GetData() const
{
	StringW		sRet;
	GetData(sRet);
	return sRet;
}


bool
J9::XMLNode::CompareData(const WChar* pData) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			pStr = NULL;

	hr = pNode_->get_text(&pStr);
	if (hr != S_OK)			return false;

	bool	bCompare = ::wcscmp(pStr, pData) == 0;
	::SysFreeString(pStr);
	return bCompare;
}

bool
J9::XMLNode::CompareDataI(const WChar* pData) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			pStr = NULL;

	hr = pNode_->get_text(&pStr);
	if (hr != S_OK)			return false;

	bool	bCompare = ::_wcsicmp(pStr, pData) == 0;
	::SysFreeString(pStr);
	return bCompare;
}

#ifdef SUPPORT_XML_ATTRIBUTE
XMLAttribute
XMLNode::GetAttribute(const WChar* pKey, bool bCreate)
{
	if (bCreate && IsNull())	Create(_W("document"));

	if (pNode_ == NULL)		return XMLAttribute();

	XMLAttributeMap	cMap = GetAttributeMap();
	XMLAttribute	cAttribute = cMap.Get(pKey);

	if (cAttribute.IsNull() && bCreate)
	{
		// attribute가 없다. 새로 생성한다.
		IXMLDOMNode*			pNode;

		XMLDocument	cDoc(pNode_);
		pNode = cDoc.CreateAttribute(pKey);
		if (pNode == NULL)	return XMLAttribute();

		HRESULT		hr;
		IXMLDOMNode*	pNewNode;
		hr = cMap.pMap_->setNamedItem(pNode, &pNewNode);
		pNode->Release();
		if (hr != S_OK)		return XMLAttribute();

		cAttribute.Set(pNewNode);
		pNewNode->Release();
	}

	return cAttribute;
}

XMLAttribute
XMLNode::GetAttribute(Size iIndex) const
{
	if (pNode_ == NULL)		return XMLAttribute();

	return GetAttributeMap().Get(iIndex);
}

XMLAttributeMap
XMLNode::GetAttributeMap(Size* pSize) const
{
	if (pSize != NULL)		*pSize = 0;

	if (pNode_ == NULL)		return XMLAttributeMap(NULL);

	IXMLDOMNamedNodeMap*	pMap;
	HRESULT					hr;
	hr = pNode_->get_attributes(&pMap);

	if (hr != S_OK)			return XMLAttributeMap(NULL);

	if (pSize != NULL)
	{
		S32	iSize;
		pMap->get_length(&iSize);
		*pSize = iSize;
	}

	XMLAttributeMap	cMap(pMap);
	pMap->Release();

	return cMap;
}

Size
XMLNode::GetAttributeNum() const
{
	if (pNode_ == NULL)		return 0;

	// get attributes
	IXMLDOMNamedNodeMap*	pMap;
	HRESULT					hr;

	hr = pNode_->get_attributes(&pMap);

	if (hr != S_OK)			return 0;

	long	iSize = 0;
	pMap->get_length(&iSize);
	pMap->Release();
	return iSize;
}
#endif

XMLNode
XMLNode::GetParent() const
{
	if (pNode_ == NULL)	return XMLNode();

	IXMLDOMNode*	pParent;
	HRESULT			hr;

	hr = pNode_->get_parentNode(&pParent);

	if (hr != S_OK)		return XMLNode();

	XMLNode	cNode(pParent);
	pParent->Release();

	return cNode;
}

XMLNode
XMLNode::GetPrevSibling() const
{
	if (pNode_ == NULL)		return XMLNode();

	IXMLDOMNode*	pPrev;
	HRESULT			hr;

	hr = pNode_->get_previousSibling(&pPrev);
	if (hr != S_OK)			return XMLNode();

	XMLNode	cNode(pPrev);
	pPrev->Release();

	return cNode;
}

XMLNode
XMLNode::GetNextSibling() const
{
	if (pNode_ == NULL)		return XMLNode();

	IXMLDOMNode*	pNext;
	HRESULT			hr;

	hr = pNode_->get_nextSibling(&pNext);
	if (hr != S_OK)			return XMLNode();

	XMLNode	cNode(pNext);
	pNext->Release();

	return cNode;
}

XMLNode
XMLNode::GetFirstChild() const
{
	if (pNode_ == NULL)		return XMLNode();

	IXMLDOMNode*	pChild;
	HRESULT			hr;

	hr = pNode_->get_firstChild(&pChild);
	if (hr != S_OK)			return XMLNode();

	XMLNode	cNode(pChild);
	pChild->Release();

	return cNode;
}

XMLNode
XMLNode::GetLastChild() const
{
	if (pNode_ == NULL)		return XMLNode();

	IXMLDOMNode*	pChild;
	HRESULT			hr;

	hr = pNode_->get_lastChild(&pChild);
	if (hr != S_OK)			return XMLNode();

	XMLNode	cNode(pChild);
	pChild->Release();

	return cNode;
}

XMLNode
XMLNode::GetChild(const WChar* pKey) const
{
	if (pNode_ == NULL)		return XMLNode();

	IXMLDOMNode*	pChild;
	HRESULT			hr;

	hr = pNode_->selectSingleNode(_cc<WChar*>(pKey), &pChild);
	if (hr != S_OK)			return XMLNode();

	XMLNode	cNode(pChild);
	pChild->Release();

	return cNode;
}

XMLNodeList
XMLNode::GetChildList(Size* pSize) const
{
	if (pSize != NULL)	*pSize = 0;

	if (pNode_ == NULL)		return XMLNodeList();

	IXMLDOMNodeList*	pNodeList;
	HRESULT				hr;

	hr = pNode_->get_childNodes(&pNodeList);
	if (hr != S_OK)			return XMLNodeList();

	if (pSize != NULL)
	{
		S32	iSize;
		pNodeList->get_length(&iSize);
		*pSize = iSize;
	}

	XMLNodeList	cNodeList(pNodeList);
	pNodeList->Release();

	return cNodeList;
}

XMLNodeList
XMLNode::GetChildList(const WChar* pKey, Size* pSize) const
{
	if (pSize != NULL)		*pSize = 0;

	XMLNodeList		cNodeList;

	if (!pKey || wcslen(pKey) == 0)		return cNodeList;
	if (pNode_ == NULL)		return cNodeList;

	IXMLDOMNodeList*	pNodeList;
	HRESULT				hr;

	hr = pNode_->selectNodes(_cc<WChar*>(pKey), &pNodeList);
	if (hr != S_OK)			return cNodeList;

	if (pSize != NULL)
	{
		S32	iSize;
		pNodeList->get_length(&iSize);
		*pSize = iSize;
	}

	cNodeList.Set(pNodeList);
	pNodeList->Release();

	return cNodeList;
}

bool
XMLNode::HasChild() const
{
	if (pNode_ == NULL)	return false;

	VARIANT_BOOL	bHas = VARIANT_FALSE;
	pNode_->hasChildNodes(&bHas);
	return bHas != VARIANT_FALSE;
}

Size
XMLNode::GetChildNum() const
{
	if (pNode_ == NULL)	return 0;

	IXMLDOMNodeList*	pNodeList;
	HRESULT				hr;

	hr = pNode_->get_childNodes(&pNodeList);
	if (hr != S_OK)		return 0;

	S32	iSize = 0;
	pNodeList->get_length(&iSize);
	pNodeList->Release();
	return iSize;
}

bool
XMLNode::SetData(const WChar* pData)
{
	if (pNode_ == NULL)		return false;

	HRESULT		hr;
	hr = pNode_->put_text(_cc<WChar*>(pData));
	if (hr != S_OK)			return false;

	return true;
}

#ifdef SUPPORT_XML_ATTRIBUTE
bool
XMLNode::RemoveAttribute(const WChar* pKey)
{
	if (pNode_ == NULL) return false;

	XMLAttributeMap	cMap = GetAttributeMap();

	return cMap.Remove(pKey);
}
#endif

XMLNode
XMLNode::CreateChild(const WChar* pKey, bool bMultiple)
{
	if (pNode_ == NULL)
		Create(_W("document"));
	ASSERT(pNode_ != NULL);

	XMLNode	cNode = GetChild(pKey);
	if (!cNode.IsNull() && bMultiple == false)
	{
		return cNode;
	}

	IXMLDOMNode*	pNode;
	pNode = XMLDocument(pNode_).CreateNode(pKey);

	if (!pNode)
		return XMLNode();

	XMLNode	cNewNode(pNode);
	pNode->Release();

	if (AppendChild(cNewNode))
		return cNewNode;

	return XMLNode();
}

bool
XMLNode::AppendChild(XMLNode& rNode)
{
	if (pNode_ == NULL || rNode.pNode_ == NULL)	return false;

	HRESULT			hr;
	hr = pNode_->appendChild(rNode.pNode_, &rNode.pNode_);
	if (hr != S_OK)		return false;

	rNode.pNode_->Release();
	return true;
}

void
XMLNode::RemoveChild(XMLNode& rNode)
{
	if (pNode_ == NULL || rNode.pNode_ == NULL)	return;

	IXMLDOMNode*	pOldNode;
	HRESULT			hr;

	hr = pNode_->removeChild(rNode.pNode_, &pOldNode);
	if (hr != S_OK)		return;

	rNode.pNode_ = pOldNode;
	return;
}

void
XMLNode::RemoveChild(const WChar* pKey, bool bMultiple)
{
	if (pNode_ == NULL) return;

	// 노드를 하나 하나 찾아서 지운다. selectNodes를 사용하여
	// NodeList를 얻어와서 지우는 것과의 성능 차이를 생각해 보자.
	do
	{
		IXMLDOMNode*	pOldNode;
		IXMLDOMNode*	pNode;
		HRESULT			hr;

		hr = pNode_->selectSingleNode(_cc<WChar*>(pKey), &pNode);
		if (hr != S_OK)	break;

		hr = pNode_->removeChild(pNode, &pOldNode);
		pNode->Release();

		if (hr != S_OK)	continue;

		pOldNode->Release();
	}
	while (bMultiple);
}

bool
XMLNode::ReplaceChild(XMLNode& rOldNode, XMLNode& rNewNode)
{
	if (pNode_ == NULL || rOldNode.pNode_ == NULL)	return false;

	IXMLDOMNode*	pOldNode;
	HRESULT			hr;

	hr = pNode_->replaceChild(rNewNode.pNode_, rOldNode.pNode_, &pOldNode);
	if (hr != S_OK)		return false;

	rOldNode.pNode_ = pOldNode;
	return true;
}

XMLNode
XMLNode::operator[](const WChar* pKey)
{
	return CreateChild(pKey, false);
}

XMLNode
XMLNode::operator[](const WChar* pKey) const
{
	return GetChild(pKey);
}

bool
XMLNode::Load(const WChar* pFileName)
{
	//TextFile cFile;
	J9::TextPackageFile cFile;

	static const WChar* pLocalExtention = _W(".local");

	J9::StringW sExtention = FileUtil::GetExtension(pFileName, true);
	// dot + extention 과 같은 string 이 pFileName 어딘가에 또 있다면 문제가 된다.
	J9::StringW sLocalFile = pFileName;
	if (!sExtention.empty())
		StringUtil::Replace<WChar>(sLocalFile, sExtention, pLocalExtention);
	else
		sLocalFile += pLocalExtention;

	J9::StringW sFileName = J9::String(pFileName);

	// .local 파일이 있으면 .local 파일을 우선적으로 읽고 .xml 파일은 무시한다.
	if (FileUtil::Access(sLocalFile.c_str()))
	{
		sFileName = sLocalFile;
	}

	if (!cFile.Open(sFileName.c_str(), J9::FileOpenRead))
	{
		if (!cFile.IsOpened())
			return false;

		CHECK(false, eCheckDebug, _W("%s 파일이 Unicode XML 이 아닙니다."), pFileName);
		Size	iSize = cFile.GetSize() - cFile.GetPos();
		std::vector<U8>	cBuf(iSize);
		cFile.Read(iSize, &cBuf[0]);
		StringA	sContentA(_rc<AChar*>(&cBuf[0]), iSize / sizeof(AChar));
		return Parse(StringUtil::ToStringW(sContentA).c_str());
	}

	Size	iSize = cFile.GetSize() - cFile.GetPos();
	std::vector<U8>	cBuf(iSize);
	cFile.Read(iSize, &cBuf[0]);
	StringW	sContent(_rc<WChar*>(&cBuf[0]), iSize / sizeof(WChar));

	sPath_ = pFileName;
	return Parse(sContent.c_str());
}

bool
XMLNode::Load(const AChar* pFileName)
{
	return Load(StringUtil::ToStringW(pFileName).c_str());
}

bool
XMLNode::Save(const WChar* pFileName, bool bIndent, bool bLineBreak)
{
	TextFile cFile;
	StringW s;

	Write(s);

	Size	iPos = 0;
	S32		iTab = 0;
	StringW	cStr;

	if (bIndent)
	{
		while ((iPos = s.find(_W("<"), iPos)) != StringW::npos)
		{
			bool bClose = false;
			// "</" 를 만나면 탭을 한단계 내려준다
			if ((iPos + 1) == s.find(_W("/"), iPos))
			{
				bClose = true;
				--iTab;
			}

			iPos = s.find(_W(">"), iPos);
			// "/>" 를 만나면 탭을 한단계 내려준다
			if ((iPos - 1) == s.find(_W("/"), iPos - 1))
			{
				bClose = true;
				--iTab;
			}

			// "><" 일경우 줄 바꿈
			if (bLineBreak)
			{
				if ((iPos + 1) == s.find(_W("<"), iPos))
				{
					// "></" 가 아니라면 탭을 한단계 올려준다
					if (((iPos + 2) != s.find(_W("/"), iPos)))
					{
						++iTab;

						cStr = _W(">\r\n");
						for (S32 i = 0; i < iTab; ++i)
							cStr += _W("\t");
						s.replace(iPos, 1, cStr);
						iPos += (2 + iTab);
					}
					else if (bClose)
					{
						cStr = _W(">\r\n");
						for (S32 i = 0; i < iTab; ++i)
							cStr += _W("\t");
						s.replace(iPos, 1, cStr);
						iPos += (2 + iTab);
					}
				}
			}
			++iPos;
		}
	}
	else if (bLineBreak)
	{
		while ((iPos = s.find(_W(">"), iPos)) != StringW::npos)
		{
			if ((iPos + 1) == s.find(_W("<"), iPos))
			{
				s.replace(iPos, 1, _W(">\r\n"));
				iPos += 2;
			}
			++iPos;
		}
	}

	if (!cFile.Open(pFileName, J9::FileOpenCreate | J9::FileOpenWrite)) return false;

	cFile << s;

	return true;
}

#ifdef SUPPORT_XML_ATTRIBUTE
bool
XMLNode::Attribute2Child(XMLNode cNewXML)
{
	bool	bPureNode = true;

	// get attribute number
	Size	iAttNum = GetAttributeNum();

	// change all attribute to child
	for (Size iAtt = 0; iAtt < iAttNum; ++iAtt)
	{
		XMLAttribute	cAtt = GetAttribute(iAtt);
		StringW	sAttName = cAtt.GetName();
		StringW	sAttValue = cAtt.GetValue();

		XMLNode	cChild = cNewXML.CreateChild(sAttName.c_str(), true);
		if (!cChild.IsNull())
		{
			cChild.SetData(sAttValue.c_str());
			bPureNode = false;
		}
	}

	Size	iChildNum = 0;
	XMLNodeList	cChildList = GetChildList(&iChildNum);

	// iterate all children
	for (Size iChild = 0; iChild < iChildNum; ++iChild)
	{
		XMLNode	cChild = cChildList.Get(iChild);

		// ignore comments and texts
		DOMNodeType		cType = cChild.GetType();
		if (cType == eComment || cType == eNodeText)
			continue;

		XMLNode	cNewChild = cNewXML.CreateChild(cChild.GetName().c_str(), true);
		cChild.Attribute2Child(cNewChild);
		bPureNode = false;
	}

	if (bPureNode)
	{
		cNewXML.SetData(GetData().c_str());
		bPureNode = false;
	}

	return bPureNode;
}
#endif

////////////////////////////////////////////////////////////////////////////////
///	XMLNodeList
////////////////////////////////////////////////////////////////////////////////
XMLNodeList::XMLNodeList()
: pList_(NULL)
{
}

XMLNodeList::XMLNodeList(IXMLDOMNodeList* pList)
: pList_(NULL)
{
	Set(pList);
}

XMLNodeList::XMLNodeList(const XMLNodeList& r)
: pList_(NULL)
{
	*this = r;
}

XMLNodeList::~XMLNodeList()
{
	Clear();
}

XMLNodeList&
XMLNodeList::operator=(const XMLNodeList& r)
{
	Clear();

	pList_ = r.pList_;

	if (pList_ != NULL)
		pList_->AddRef();

	return *this;
}

void
XMLNodeList::Clear()
{
	if (pList_ != NULL)
	{
		pList_->Release();
		pList_ = NULL;
	}
}

void
XMLNodeList::Set(IXMLDOMNodeList* pList)
{
	Clear();

	if (pList == NULL)
		return;

	pList_ = pList;
	pList_->AddRef();
}

Size
XMLNodeList::GetSize() const
{
	if (pList_ == NULL)
		return 0;

	S32	iSize = 0;
	pList_->get_length(&iSize);
	return iSize;
}

XMLNode
XMLNodeList::Get(Size iIndex) const
{
	if (pList_ == NULL)	return XMLNode();

	IXMLDOMNode*	pNode;
	HRESULT			hr;

	hr = pList_->get_item(_sc<S32>(iIndex), &pNode);
	if (hr != S_OK)		return XMLNode();

	XMLNode	cNode(pNode);
	pNode->Release();
	return cNode;
}

#ifdef SUPPORT_XML_ATTRIBUTE
XMLNode
XMLNodeList::Find(const WChar* pAttrName, const WChar* pAttrValue, Size* pIndex)
{
	return FindWithAttribute(pAttrName, pAttrValue, pIndex);
}

XMLNode
XMLNodeList::FindWithAttribute(const WChar* pAttrName, const WChar* pAttrValue, Size* pIndex)
{
	// optimize?
	if (pIndex != NULL)	*pIndex = U32_MAX;
	if (pList_ == NULL)	return XMLNode();

	Size	iCount = GetSize();
	XMLNode	cFound;
	for (Size i = 0; i < iCount; ++i)
	{
		XMLNode	cCurNode = Get(i);

		if (cCurNode.GetAttribute(pAttrName).GetValue() == pAttrValue)
		{
			cFound = cCurNode;
			if (pIndex != NULL)	*pIndex = i;
			break;
		}
	}

	return cFound;
}
#endif

XMLNode
XMLNodeList::FindWithChild(const WChar* pChildName, const WChar* pChildValue, Size* pIndex)
{
	// optimize?
	if (pIndex != NULL)	*pIndex = U32_MAX;
	if (pList_ == NULL)	return XMLNode();

	Size	iCount = GetSize();
	XMLNode	cFound;
	for (Size i = 0; i < iCount; ++i)
	{
		XMLNode	cCurNode = Get(i);

		if (cCurNode.GetChild(pChildName).CompareData(pChildValue))
		{
			cFound = cCurNode;
			if (pIndex != NULL)	*pIndex = i;
			break;
		}
	}

	return cFound;
}


J9::XMLNode
J9::XMLNodeList::FindWithChildI(const WChar* pChildName, const WChar* pChildValue, Size* pIndex)
{
	// optimize?
	if (pIndex != NULL)	*pIndex = U32_MAX;
	if (pList_ == NULL)	return XMLNode();

	Size	iCount = GetSize();
	XMLNode	cFound;
	for (Size i = 0; i < iCount; ++i)
	{
		XMLNode	cCurNode = Get(i);

		if (cCurNode.GetChild(pChildName).CompareDataI(pChildValue))
		{
			cFound = cCurNode;
			if (pIndex != NULL)	*pIndex = i;
			break;
		}
	}

	return cFound;
}

////////////////////////////////////////////////////////////////////////////////
///	XMLAttribute
////////////////////////////////////////////////////////////////////////////////
#ifdef SUPPORT_XML_ATTRIBUTE
XMLAttribute::XMLAttribute()
: pNode_(NULL)
{
}

XMLAttribute::XMLAttribute(IXMLDOMNode* pNode)
: pNode_(NULL)
{
	Set(pNode);
}

XMLAttribute::XMLAttribute(const XMLAttribute& r)
: pNode_(NULL)
{
	*this = r;
}

XMLAttribute::~XMLAttribute()
{
	Clear();
}

XMLAttribute& XMLAttribute::operator=(const XMLAttribute& r)
{
	Clear();

	pNode_ = r.pNode_;

	if (pNode_ != NULL)
		pNode_->AddRef();

	return *this;
}

void
XMLAttribute::Clear()
{
	if (pNode_ != NULL)
	{
		pNode_->Release();
		pNode_ = NULL;
	}
}

void
XMLAttribute::Set(IXMLDOMNode* p)
{
	Clear();

	if (p == NULL)	return;

	pNode_ = p;
	pNode_->AddRef();
}

bool
XMLAttribute::GetName(StringW& rName) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			bstrName = NULL;

	hr = pNode_->get_nodeName(&bstrName);
	if (hr != S_OK)			return false;

	rName = bstrName;
	::SysFreeString(bstrName);
	return true;
}

StringW
XMLAttribute::GetName() const
{
	StringW	sRet;
	GetName(sRet);
	return sRet;
}

bool
XMLAttribute::GetType(J9::DOMNodeType& rType) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	::DOMNodeType	eType;

	hr = pNode_->get_nodeType(&eType);
	if (hr != S_OK)			return false;

	rType = static_cast<DOMNodeType>(eType);
	return true;
}

J9::DOMNodeType
XMLAttribute::GetType() const
{
	DOMNodeType	eRet;
	GetType(eRet);
	return eRet;
}

bool
XMLAttribute::GetValue(StringW& rValue) const
{
	if (pNode_ == NULL)		return false;

	HRESULT			hr;
	BSTR			bstrData = NULL;

	hr = pNode_->get_text(&bstrData);
	if (hr != S_OK)			return false;

	rValue = bstrData;
	::SysFreeString(bstrData);
	return true;
}

StringW
XMLAttribute::GetValue() const
{
	StringW		sRet;
	GetValue(sRet);
	return sRet;
}

bool
XMLAttribute::SetValue(const WChar* pValue)
{
	if (pNode_ == NULL)		return false;

	HRESULT		hr;
	hr = pNode_->put_text(_cc<WChar*>(pValue));
	if (hr != S_OK)			return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
///	XMLAttributeMap
////////////////////////////////////////////////////////////////////////////////
XMLAttributeMap::XMLAttributeMap(IXMLDOMNamedNodeMap* pMap)
: pMap_(NULL)
{
	Set(pMap);
}

XMLAttributeMap::XMLAttributeMap(const XMLAttributeMap& r)
: pMap_(NULL)
{
	*this = r;
}

XMLAttributeMap::~XMLAttributeMap()
{
	Clear();
}

XMLAttributeMap&
XMLAttributeMap::operator=(const XMLAttributeMap& r)
{
	Clear();

	pMap_ = r.pMap_;
	if (pMap_ != NULL)
		pMap_->AddRef();

	return *this;
}

void
XMLAttributeMap::Clear()
{
	if (pMap_ != NULL)
	{
		pMap_->Release();
		pMap_ = NULL;
	}
}

void
XMLAttributeMap::Set(IXMLDOMNamedNodeMap* pMap)
{
	Clear();

	if (pMap == NULL)	return;

	pMap_ = pMap;
	pMap_->AddRef();
}

Size
XMLAttributeMap::GetSize() const
{
	if (pMap_ == NULL)	return 0;

	S32	iSize = 0;
	pMap_->get_length(&iSize);
	return iSize;
}

XMLAttribute
XMLAttributeMap::Get(const WChar* pKey)
{
	if (pMap_ == NULL)	return XMLAttribute();

	IXMLDOMNode*	pNode;
	HRESULT			hr;

	hr = pMap_->getNamedItem(_cc<WChar*>(pKey), &pNode);
	if (hr != S_OK)			return XMLAttribute();

	XMLAttribute	cAttribute(pNode);
	pNode->Release();

	return cAttribute;
}

XMLAttribute
XMLAttributeMap::Get(Size iIndex) const
{
	if (pMap_ == NULL)	return XMLAttribute();

	IXMLDOMNode*	pNode;
	HRESULT			hr;

	hr = pMap_->get_item(_sc<S32>(iIndex), &pNode);
	if (hr != S_OK)			return XMLAttribute();

	XMLAttribute	cAttribute(pNode);
	pNode->Release();

	return cAttribute;
}

XMLAttribute
XMLAttributeMap::Pop(const WChar* pKey)
{
	if (pMap_ == NULL)	return XMLAttribute();

	IXMLDOMNode*	pNode;
	HRESULT			hr;

	hr = pMap_->getNamedItem(_cc<WChar*>(pKey), &pNode);
	if (hr != S_OK)			return XMLAttribute();

	XMLAttribute	cAttribute(pNode);

	hr = pMap_->removeNamedItem(_cc<WChar*>(pKey), &pNode);
	if (GetSize() == 0)
	{
		pMap_->Release();
		pMap_ = NULL;
	}
	pNode->Release();

	return cAttribute;
}

XMLAttribute
XMLAttributeMap::Pop(Size iIndex)
{
	if (pMap_ == NULL)	return XMLAttribute();

	IXMLDOMNode*	pNode;
	HRESULT			hr;

	hr = pMap_->get_item(_sc<S32>(iIndex), &pNode);
	if (hr != S_OK)			return XMLAttribute();

	XMLAttribute	cAttribute(pNode);

	BSTR			bstrName = NULL;

	hr = pNode->get_nodeName(&bstrName);
	hr = pMap_->removeNamedItem(bstrName, &pNode);
	::SysFreeString(bstrName);

	if (GetSize() == 0)
	{
		pMap_->Release();
		pMap_ = NULL;
	}
	pNode->Release();

	return cAttribute;
}

bool
XMLAttributeMap::Remove(const WChar* pKey)
{
	if (pMap_ == NULL)	return false;

	HRESULT			hr;
	IXMLDOMNode*	pNode;
	hr = pMap_->removeNamedItem(_cc<WChar*>(pKey), &pNode);
	pMap_->Release();

	if (GetSize() == 0)	pMap_ = NULL;

	if (hr != S_OK)		return false;

	pNode->Release();
	return true;
}
#endif
