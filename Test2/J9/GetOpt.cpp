// $Id: GetOpt.cpp 3959 2009-09-25 09:50:04Z hyojin.lee $

#include "StdAfx.h"
#include "GetOpt.h"

//
// Copyright (c) 1987, 1993, 1994
//	The Regents of the University of California.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//	This product includes software developed by the University of
//	California, Berkeley and its contributors.
// 4. Neither the name of the University nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#include <stdio.h>
#include <string.h>

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getopt.c	8.3 (Berkeley) 4/27/95";
#endif // LIBC_SCCS and not lint

S32		J9::iOptionError = 1;		// if error message should be printed
S32		J9::iOptionIndex = 1;		// index into parent argv vector
S32		J9::iOptionChar;			// character checked for validity
S32		J9::iOptionReset;		// reset getopt
WChar*	J9::pOptionArg;			// argument associated with option

#define	BADCH	(S32)'?'
#define	BADARG	(S32)':'
#define	EMSG	_W("")

//////////////////////////////////////////////
//  getopt --
//	Parse argc/argv argument vector.
//////////////////////////////////////////////

using namespace J9;

S32
J9::GetOption(S32 iArgc, WChar* const* pArgv, const WChar* pOptions)
{
	#define __PROGNAME__ pArgv[0]
	static WChar *pPlace = EMSG;				// option letter processing
	const WChar *pOptionLetter;					// option letter list index

	if (iOptionReset || !*pPlace)
	{
		// update scanning pointer
		iOptionReset = 0;
		if (iOptionIndex >= iArgc || *(pPlace = pArgv[iOptionIndex]) != '-')
		{
			pPlace = EMSG;
			return (-1);
		}
		if (pPlace[1] && *++pPlace == '-')
		{
			// found "--"
			++iOptionIndex;
			pPlace = EMSG;
			return (-1);
		}
	} // option letter okay?

	if ((iOptionChar = (S32) *pPlace++, iOptionChar) == (S32) ':'
		|| (pOptionLetter = wcschr(pOptions, _sc<WChar>(iOptionChar)), !pOptionLetter))
	{
		// if the user didn't specify '-' as an option,
		// assume it means -1.
		if (iOptionChar == (S32) '-') return (-1);
		if (!*pPlace) ++iOptionIndex;
		if (iOptionError && *pOptions != ':')
			fprintf(stderr, "%s: illegal option -- %c\n", __PROGNAME__, iOptionChar);
		return (BADCH);
	}
	if (*++pOptionLetter != ':')
	{
		// don't need argument
		pOptionArg = NULL;
		if (!*pPlace) ++iOptionIndex;
	}
	else
	{
		// need an argument
		if (*pPlace) pOptionArg = pPlace; // no white space
		else if (iArgc <= ++iOptionIndex)
		{
			// no arg
			pPlace = EMSG;
			if (*pOptions == ':') return (BADARG);
			if (iOptionError)
				fprintf(stderr, "%s: option requires an argument -- %c\n", __PROGNAME__, iOptionChar);
			return (BADCH);
		}
		else				// white space
			pOptionArg = pArgv[iOptionIndex];
		pPlace = EMSG;
		++iOptionIndex;
	}

	return (iOptionChar);			// dump back option letter
}