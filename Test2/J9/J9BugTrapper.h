#ifdef _SERVER
#include "../../../../externals/BugTrap/BugTrap.h"

#ifdef UNICODE
	#pragma comment(lib, "BugTrapU.lib")
#else
	#pragma comment(lib, "BugTrap.lib")
#endif

namespace J9
{
	class BugTrapper
	{
	public:
		static void SetupExceptionHandler();

		static J9::String sAppName_;
		static J9::String sEmail_;
		static DWORD dFlags_;
		static J9::String sSupportServer_;
		static J9::String sDumpFilePath_;
		static SHORT iSupportPort_;
	};
}

#endif