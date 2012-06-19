// $Id$

#ifndef __J9_STRING_TRAITS__
#define __J9_STRING_TRAITS__

namespace J9
{
	template<typename _Type, bool bCaseSensitive = true>
	class StringTraits
	{
	};

	template<>
	class StringTraits<AChar, true>
	{
	public:
		typedef AChar							Type;
		typedef std::basic_string<AChar>		String;

	public:
		static const Type*	Slash()										{ return "/"; }
		static const Type*	BackSlash()									{ return "\\"; }
		static const Type*	Multiply()									{ return "*"; }
		static bool			Compare(const Type* pLhs, const Type* pRhs)	{ return ::strcmp(pLhs, pRhs) == 0; }
		static int			ToLower(AChar iChar)						{ return ::tolower(iChar); }
		static int			ToUpper(AChar iChar)						{ return ::toupper(iChar); }
	};

	template<>
	class StringTraits<AChar, false>
	{
	public:
		typedef AChar	Type;
		typedef std::basic_string<AChar>		String;

	public:
		static const Type*	Slash()				{ return "/"; }
		static const Type*	BackSlash()			{ return "\\"; }
		static const Type*	Multiply()			{ return "*"; }
		static bool			Compare(const Type* pLhs, const Type* pRhs)	{ return ::_stricmp(pLhs, pRhs) == 0; }
		static int			ToLower(AChar iChar)						{ return ::tolower(iChar); }
		static int			ToUpper(AChar iChar)						{ return ::toupper(iChar); }
	};

	template<>
	class StringTraits<WChar, true>
	{
	public:
		typedef WChar							Type;
		typedef std::basic_string<WChar>		String;

	public:
		static const Type*	Slash()				{ return _W("/"); }
		static const Type*	BackSlash()			{ return _W("\\"); }
		static const Type*	Multiply()			{ return _W("*"); }
		static bool			Compare(const Type* pLhs, const Type* pRhs)	{ return ::wcscmp(pLhs, pRhs) == 0; }
		static int			ToLower(WChar iChar)						{ return ::towlower(iChar); }
		static int			ToUpper(WChar iChar)						{ return ::towupper(iChar); }
	};

	template<>
	class StringTraits<WChar, false>
	{
	public:
		typedef WChar							Type;
		typedef std::basic_string<WChar>		String;

	public:
		static const Type*	Slash()				{ return _W("/"); }
		static const Type*	BackSlash()			{ return _W("\\"); }
		static const Type*	Multiply()			{ return _W("*"); }
		static bool			Compare(const Type* pLhs, const Type* pRhs)	{ return ::_wcsicmp(pLhs, pRhs) == 0; }
		static int			ToLower(WChar iChar)						{ return ::towlower(iChar); }
		static int			ToUpper(WChar iChar)						{ return ::towupper(iChar); }
	};
}

#endif//__J9_STRING_TRAITS__