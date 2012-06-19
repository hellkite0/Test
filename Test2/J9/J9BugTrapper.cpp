#include "stdafx.h"
#include "J9BugTrapper.h"

#ifdef _SERVER

#include <dbghelp.h>

DWORD J9::BugTrapper::dFlags_ = BTF_DETAILEDMODE;
J9::String J9::BugTrapper::sAppName_ = _W("cpmw");
J9::String J9::BugTrapper::sEmail_ = _W("cpmw_tech@just9.co.kr");
J9::String J9::BugTrapper::sSupportServer_ = _W("192.168.1.254");
J9::String J9::BugTrapper::sDumpFilePath_ = _W(".");
SHORT J9::BugTrapper::iSupportPort_ = 9999;

void
J9::BugTrapper::SetupExceptionHandler()
{
	BT_InstallSehFilter();
	BT_SetActivityType(BTA_SAVEREPORT);
	BT_SetReportFilePath(sDumpFilePath_.c_str());
	BT_SetAppName(sAppName_.c_str());
//	BT_SetSupportEMail(sEmail_.c_str());
	BT_SetFlags(dFlags_);
	BT_SetDumpType( MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpFilterMemory | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules );
//	BT_SetSupportServer(sSupportServer_.c_str(), iSupportPort_);
//	BT_SetSupportURL(_T("http://www.just9.net"));
}
#endif