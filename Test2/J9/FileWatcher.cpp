#include "stdafx.h"
#include "FileWatcher.h"
#include "UtilityFile.h"
#include <time.h>

using namespace J9;

const int iResultBufferSize = 4096;

FileWatcher::FileWatcher() 
	: hDirectoryHandle_(NULL)
	, hNotifyEvent_(NULL)
	, pNotifyResults_(NULL)
	, iTimeOfFirstPendingChange_(0)
	, bTiming_(false)
	, pfCallBack_(NULL)
{
}

void
FileWatcher::Init(LPCWSTR pFilePath)
{
	hDirectoryHandle_ = CreateFile(pFilePath,
		FILE_LIST_DIRECTORY, 
		FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);

	hNotifyEvent_ = CreateEvent(NULL, FALSE, FALSE, _W("FileWriteEvent"));
	ASSERT(hNotifyEvent_);

	cOverlappedIO_.hEvent = hNotifyEvent_;

	pNotifyResults_ = new char[iResultBufferSize];

	Listen();
}

FileWatcher::~FileWatcher()
{
	if (hDirectoryHandle_)
	{
		CloseHandle(hDirectoryHandle_);
	}

	delete [] pNotifyResults_;
}

bool
FileWatcher::Listen()
{
	DWORD unused;
	BOOL rv = ReadDirectoryChangesW(hDirectoryHandle_, 
									pNotifyResults_, 
									iResultBufferSize,
									TRUE,
									FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
									&unused,
									&cOverlappedIO_,
									NULL);

	if (rv == 0)
	{
		LPVOID pBuffer;

		if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL,
						  GetLastError(),
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  (LPWSTR)&pBuffer,
						  0,
						  NULL))
			_LOG(_T("ReadDirectoryChangesW failed with '%s'\r\n"), (const WCHAR*)pBuffer);
		else
			_LOG(_W("Couldn't format error msg for ReadDirectoryChangesW failure.\r\n"));

		LocalFree(pBuffer);
	}

	return (rv != 0);
}

bool
FileWatcher::Update()
{
	UpdateReportTiming();

	DWORD bytesWritten;
	BOOL rv = GetOverlappedResult(hNotifyEvent_, &cOverlappedIO_, &bytesWritten, FALSE);

	if (rv == FALSE)
	{
		ASSERT(GetLastError() == ERROR_IO_INCOMPLETE);
		return true;
	}

	const char *pCurEntry = pNotifyResults_;

	bool bDone_ = false;
	while (!bDone_)
	{
		const FILE_NOTIFY_INFORMATION *pFileNotifyInfo = 
			_rc< const FILE_NOTIFY_INFORMATION * >(pCurEntry);

		if (pFileNotifyInfo->Action == FILE_ACTION_ADDED || pFileNotifyInfo->Action == FILE_ACTION_MODIFIED || pFileNotifyInfo->Action == FILE_ACTION_RENAMED_NEW_NAME)
		{
			ReportChangedFile(pFileNotifyInfo->FileName, pFileNotifyInfo->FileNameLength / 2);
		}

		bDone_ = (pFileNotifyInfo->NextEntryOffset == 0);
		pCurEntry += pFileNotifyInfo->NextEntryOffset;
	}

	return Listen();
}

void
FileWatcher::ReportChangedFile(J9::StringW sFileName, DWORD iFileNameLength)
{
	sFileName = sFileName.substr(0, iFileNameLength);
	if (std::find(sPendingChangedFiles_.begin(), 
				  sPendingChangedFiles_.end(), 
				  sFileName) == sPendingChangedFiles_.end())
	{
		if (!J9::FileUtil::Access(sFileName.c_str()))
			return;
		sPendingChangedFiles_.push_back(sFileName);

		if (!bTiming_)
		{
			bTiming_ = true;
			_time64(&iTimeOfFirstPendingChange_);
		}
	}
}

void
FileWatcher::UpdateReportTiming()
{
	if (bTiming_)
	{
		__time64_t iNow;
		_time64(&iNow);

		if (iNow - iTimeOfFirstPendingChange_ > 1)
		{
			if (pfCallBack_)
				pfCallBack_(sPendingChangedFiles_);

			sPendingChangedFiles_.clear();
			bTiming_ = false;
		}
	}
}