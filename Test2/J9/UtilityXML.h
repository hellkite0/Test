// $Id: UtilityXML.h 6279 2010-06-29 07:06:01Z hyungsub.shim $

#ifndef __J9_UTILITY_XML_H__
#define __J9_UTILITY_XML_H__

#include "XML.h"

namespace J9
{
	namespace XMLUtil
	{
		template <typename Type>
		void	XML2Array(XMLAttribute cAttribute, Type& rVector)
		{
			J9::StringUtil::String2Array(cAttribute.GetValue(), rVector);
		}

		template<typename Type, Size iSize>
		void	XML2Array(XMLNode cXML, Type (&TypeArray)[iSize])
		{
			J9::StringUtil::String2Array(cXML.GetData(), TypeArray);
		}

		template<typename Type, Size iSize>
		void	XML2Array(XMLNode cXML, const WChar* pChild, Type (&TypeArray)[iSize])
		{
			XML2Array(cXML.GetChild(pChild), TypeArray);
		}

		template <typename Type>
		void	XML2Vector(XMLAttribute cAttribute, Type& rVector)
		{
			J9::StringUtil::String2Vector(cAttribute.GetValue(), rVector);
		}

		template <typename Type>
		void	XML2Vector(XMLNode cXML, Type& rVector)
		{
			J9::StringUtil::String2Vector(cXML.GetData(), rVector);
		}

		template <typename Type>
		void	XML2Vector(XMLNode cXML, const WChar* pChildName, Type& rVector)
		{
			J9::StringUtil::String2Vector(cXML.GetChild(pChildName).GetData(), rVector);
		}

		template <typename Type>
		void	XML2StdVector(XMLNode cXML, std::vector<Type>& rVector)
		{
			J9::StringUtil::String2StdVector(cXML.GetData(), rVector);
		}

		template <typename Type>
		void	XML2StdVector(XMLNode cXML, const WChar* pChildName, std::vector<Type>& rVector)
		{
			J9::StringUtil::String2StdVector(cXML.GetChild(pChildName).GetData(), rVector);
		}

		template <typename Type>
		void	XML2StdVector(XMLAttribute cAttribute, std::vector<Type>& rVector)
		{
			J9::StringUtil::String2StdVector(cAttribute.GetValue(), rVector);
		}

		template<typename KeyType, typename Type>
		void	XML2Map(XMLNode cXML, const WChar* pChild, stdext::hash_map<KeyType, Type>& rMap)
		{
			Size	iChildNum = 0;
			XMLNodeList	cChildList = cXML.GetChildList(pChild, &iChildNum);
			for (Size iChild = 0; iChild < iChildNum; ++iChild)
			{
				XMLNode	cChild = cChildList.Get(iChild);
				J9::StringUtil::String2Map(cChild.GetData(), rMap);
			}
		}

		template<typename KeyType, typename Type>
		void	XML2Map(XMLNode cXML, const WChar* pChild, std::map<KeyType, Type>& rMap)
		{
			Size	iChildNum = 0;
			XMLNodeList	cChildList = cXML.GetChildList(pChild, &iChildNum);
			for (Size iChild = 0; iChild < iChildNum; ++iChild)
			{
				XMLNode	cChild = cChildList.Get(iChild);
				J9::StringUtil::String2Map(cChild.GetData(), rMap);
			}
		}
	}
}

#endif//__J9_UTILITY_XML_H__
