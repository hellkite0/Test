// $Id$

#include "Stdafx.h"
#include "Version.h"

#include "UtilityString.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////
// Version
////////////////////////////////////////////////////////////////////////
Version::Version()
{
	for (Size i = 0; i < countof(i_); ++i)
		i_[i] = 0;
}
Version::Version(const Version& r)
{
	for (Size i = 0; i < countof(i_); ++i)
		i_[i] = r.i_[i];
}

StringW
Version::GetVersion() const
{
	return StringUtil::Format(_W("%d.%d.%d.%d"), i_[0], i_[1], i_[2], i_[3]);
}

void
Version::SetVersion(const J9::StringW& rVersion)
{
	StringW sVersion = rVersion;
	Size	iPos = 0;
	for (U8 i = 0; i < 4; ++i)
	{
		StringW sTemp = StringUtil::Tokenize(sVersion, StringW(_W(".")), iPos);
		if (sTemp.empty())
		{
			i_[i] = 0;
		}
		else
		{
			StringUtil::Convert(sTemp, i_[i]);
		}
	}
}
