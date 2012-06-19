#pragma once

#include "String.h"

namespace J9
{
	class FileWatcher
	{
		DISABLE_COPY_CREATOR(FileWatcher);
	public:
		typedef std::vector< J9::StringW > StringVec;
		typedef void (*ChangedFileSetCallback)(const StringVec &);
	public:
		FileWatcher();
		~FileWatcher();

		void	Init(LPCWSTR pFilePath);
		bool	Update();
		void	InstallChangedFileSetCallback(ChangedFileSetCallback cb) { pfCallBack_ = cb; }

	private:
		bool	Listen();
		void	UpdateReportTiming();
		void	ReportChangedFile(J9::StringW sFileName, DWORD iFileNameLength);

	private:
		HANDLE					hDirectoryHandle_;
		HANDLE					hNotifyEvent_;
		OVERLAPPED				cOverlappedIO_;
		char*					pNotifyResults_;

		StringVec				sPendingChangedFiles_;
		__time64_t				iTimeOfFirstPendingChange_;
		bool					bTiming_;

		ChangedFileSetCallback	pfCallBack_;
	};
}
