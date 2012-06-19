// $Id: GetOpt.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_GETOPT_H__
#define __J9_GETOPT_H__

namespace J9
{
	extern S32		iOptionError;
	extern S32		iOptionIndex;
	extern S32		iOptionChar;
	extern S32		iOptionReset;
	extern WChar*	pOptionArg;

	extern S32 GetOption(S32 iArgc, WChar* const* pArgv, const WChar* pOptions);
};

#endif // __J9_GETOPT_H__
