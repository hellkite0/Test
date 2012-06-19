// $Id: Exception.cpp 6196 2010-06-17 06:03:40Z hyojin.lee $
// this file name should be changed.

#include "Stdafx.h"
#include "Exception.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// J9Error
////////////////////////////////////////////////////////////////////////////////
Error::Error(U32 iError)
: iError_(iError)
{
	__debugbreak();
}

////////////////////////////////////////////////////////////////////////////////
// J9Error
////////////////////////////////////////////////////////////////////////////////
J9Error::J9Error(J9ErrorCode iError)
: Error(iError)
{
	::wcsncpy_s(sError_, GetErrorMsg(iError), countof(sError_));
}

J9Error::J9Error(const WChar* pMsg)
: Error(J9ECGeneral)
{
	::wcsncpy_s(sError_, pMsg, countof(sError_));
}

const WChar*
J9Error::GetErrorMsg(J9ErrorCode iError)
{
	switch (iError)
	{
	case J9ECGeneral:		return _W("General error.");
	case J9ECFileOpen:		return _W("File open error.");
	case J9ECCasting:		return _W("Casting error.");
	case J9ECInvalidArg:	return _W("Invalid arg.");
	case J9ECHardware:		return _W("Unknown hardware.");
	default:				return _W("");
	}
}

////////////////////////////////////////////////////////////////////////////////
// SystemError
////////////////////////////////////////////////////////////////////////////////
SystemError::SystemError(HRESULT hError)
: Error(hError)
{
}