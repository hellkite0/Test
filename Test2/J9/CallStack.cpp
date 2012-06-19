// $Id: CallStack.cpp 516 2008-06-07 08:18:54Z hyojin.lee $

#include "Stdafx.h"
#include "CallStack.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#include <process.h>

#include "UtilityMisc.h"
#include "Log.h"
#include "UtilityString.h"
#include "UtilityFile.h"

using namespace J9;

CallStackInfo::CallStackInfo()
	: sModuleName_(_W("<unknown module>"))
	, sSymbolName_(_W("<unknown symbol>"))
	, iOffsetFromSymbol_(0)
	, sSourceName_(_W("<unknown source>"))
	, iLine_(0)
	, iOffsetFromLine_(0)
	, bSourceInfoValid_(false)
{
}

CallStackInfo::CallStackInfo(const CallStackInfo& r)
{
	sModuleName_			= r.sModuleName_;

	sSymbolName_			= r.sSymbolName_;
	iOffsetFromSymbol_		= r.iOffsetFromSymbol_;

	sSourceName_			= r.sSourceName_;
	iLine_					= r.iLine_;
	iOffsetFromLine_		= r.iOffsetFromLine_;

	bSourceInfoValid_		= r.bSourceInfoValid_;
}

CallStackInfo&
CallStackInfo::operator=(const CallStackInfo& r)
{
	sModuleName_			= r.sModuleName_;

	sSymbolName_			= r.sSymbolName_;
	iOffsetFromSymbol_		= r.iOffsetFromSymbol_;

	sSourceName_			= r.sSourceName_;
	iLine_					= r.iLine_;
	iOffsetFromLine_		= r.iOffsetFromLine_;

	bSourceInfoValid_		= r.bSourceInfoValid_;

	return *this;
}

#ifdef UNICODE
struct ImageHelpModule : public IMAGEHLP_MODULEW64
#else
struct ImageHelpModule : public IMAGEHLP_MODULE64
#endif
{
	ImageHelpModule()
	{
		SetZero(*this);
		SizeOfStruct = sizeof(*this);
	}
};

struct ImageHelpLine : public IMAGEHLP_LINE64
{
	ImageHelpLine()
	{
		SetZero(*this);
		SizeOfStruct = sizeof(*this);
	}
};

class J9::CallStackCapturer
{
public:
	CallStackCapturer(CallStack* pCallStack);
	~CallStackCapturer();

	void					Init();
	bool					Capture();

private:
	bool					TraceCallStack();
	bool					TraceCallStackOneStep(CallStackInfo& rStackTrace);

	bool					GetModuleInfo(U64 iAddress, CallStackInfo& rStackTrace);
	bool					GetSymbolInfo(U64 iAddress, CallStackInfo& rStackTrace);
	bool					GetSourceInfo(U64 iAddress, CallStackInfo& rStackTrace);

	bool					LoadModuleByAddress(U64 iAddress);

	bool					ReadyStackFrame();
	static UINT __stdcall	ThreadFunc(void* pVoid);

private:
	U32					iMachine_;
	HANDLE				hProcess_;
	HANDLE				hDuplicated_;

	CallStack*			pCallStack_;

	CONTEXT				cContext_;
	STACKFRAME64		cStackFrame_;

	bool				bSuccess_;
};

CallStackCapturer::CallStackCapturer(CallStack* pCallStack)
	: iMachine_()
	, hProcess_(NULL)
	, hDuplicated_(NULL)
	, pCallStack_(pCallStack)
	, bSuccess_(false)
{
	Init();
}

CallStackCapturer::~CallStackCapturer()
{
	if (hProcess_ != NULL)
	{
		::SymCleanup(hProcess_);
	}
}

void
CallStackCapturer::Init()
{
	iMachine_ = IMAGE_FILE_MACHINE_I386;

	hProcess_ = ::GetCurrentProcess();

	U32	dwOpts = SymGetOptions();
	::SymSetOptions(dwOpts | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
	::SymInitialize(hProcess_, NULL, true);
}

bool
CallStackCapturer::Capture()
{
	//---------------------------
	// �ڽ��� ������ ���ؽ�Ʈ�� �ùٸ��� ���� �� �����Ƿ� �ٸ� �����带 �����Ѵ�
	HANDLE				hThread;
	UINT				iThreadId;

	// ���� �������� �ڵ��� �����Ѵ�
	if (!DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(), ::GetCurrentProcess(), &hDuplicated_, THREAD_ALL_ACCESS, false, 0))
	{
		return false;
	}

	hThread = _rc<HANDLE>(_beginthreadex(NULL, NULL, ThreadFunc, _sc<void*>(this), 0, &iThreadId));
	if (hThread != NULL)
	{
		::WaitForSingleObject(hThread, INFINITE);
	}

	// ������ �ڵ��� �����Ѵ�
	::CloseHandle(hDuplicated_);

	return bSuccess_;
}

bool
CallStackCapturer::TraceCallStack()
{

	// Ʈ���̽��� �����Ѵ�
	for (Size i = 0; i < pCallStack_->iTraceDepth_; ++i)
	{
		CallStackInfo	cTrace;

		if (!TraceCallStackOneStep(cTrace))
		{
			break;
		}

		if (cStackFrame_.AddrPC.Offset != NULL)
		{
			// ���� Ʈ���̽� ����� �߰��Ѵ�
			pCallStack_->cStack_.push_back(cTrace);
		}
	}

	return true;
}

bool
CallStackCapturer::TraceCallStackOneStep(CallStackInfo& rStackTrace)
{
	if (!StackWalk64(iMachine_, hProcess_, hDuplicated_, &cStackFrame_, &cContext_, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
	{
		LOG_WIN32_ERROR(_W("StackWalk64()"), ::GetLastError());

		return false;
	}

	if (cStackFrame_.AddrPC.Offset != NULL)
	{
		GetModuleInfo(cStackFrame_.AddrPC.Offset, rStackTrace);
		GetSymbolInfo(cStackFrame_.AddrPC.Offset, rStackTrace);
		GetSourceInfo(cStackFrame_.AddrPC.Offset, rStackTrace);
	}

	return true;
}

bool
CallStackCapturer::GetModuleInfo(U64 iAddress, CallStackInfo& rStackTrace)
{
	ImageHelpModule	sImgHlpModule;

	// ��� ������ ������ �ε��Ѵ�
	LoadModuleByAddress(iAddress);

#ifdef UNICODE
	if (!::SymGetModuleInfoW64(hProcess_, iAddress, &sImgHlpModule))
#else
	if (!::SymGetModuleInfo64(hProcess_, iAddress, &sImgHlpModule))
#endif
	{
		LOG_WIN32_ERROR(_W("SymGetModuleInfo64()"), GetLastError());
		return false;
	}

	rStackTrace.sModuleName_ = sImgHlpModule.ImageName;

	return true;
}

bool
CallStackCapturer::GetSymbolInfo(U64 iAddress, CallStackInfo& rStackTrace)
{
	U64				iDisplacementSymbol;
	U8				iBuf[sizeof(SYMBOL_INFO) + MAX_PATH];
	PSYMBOL_INFO	pSymbolInfo = _rc<PSYMBOL_INFO>(iBuf);

	SetZero(iBuf);
	pSymbolInfo->SizeOfStruct	= sizeof(SYMBOL_INFO);
	pSymbolInfo->MaxNameLen		= MAX_PATH;

	if (!::SymFromAddr(hProcess_, iAddress, &iDisplacementSymbol, pSymbolInfo))
	{
		LOG_WIN32_ERROR(_W("SymFromAddr()"), GetLastError());
		return false;
	}

	rStackTrace.iOffsetFromSymbol_	= iDisplacementSymbol;
	rStackTrace.sSymbolName_		= StringUtil::ToStringW(pSymbolInfo->Name);

	return true;
}

bool
CallStackCapturer::GetSourceInfo(U64 iAddress, CallStackInfo& rStackTrace)
{
	U32				iDisplacementLine;
	ImageHelpLine	sImgHlpLine;

	if (!::SymGetLineFromAddr64(hProcess_, iAddress, &iDisplacementLine, &sImgHlpLine))
	{
		return false;
	}

	rStackTrace.sSourceName_		= StringUtil::ToStringW(sImgHlpLine.FileName);
	rStackTrace.iLine_				= sImgHlpLine.LineNumber;
	rStackTrace.iOffsetFromLine_	= iDisplacementLine;

	// �ҽ� ���� ������ ��ȿ�ϴ�
	rStackTrace.bSourceInfoValid_ = true;

	return true;
}

bool
CallStackCapturer::LoadModuleByAddress(U64 iAddress)
{
	MEMORY_BASIC_INFORMATION	cMBI;

	// VirtualQuery()�� �̹����� ������ ���´�
	// do the VirtualQueryEx() to see if I can find the start of
	// this module.  Since the HMODULE is the start of a module
	// in memory, viola, this will give me the HMODULE.
	if (0 != VirtualQueryEx(hProcess_, _rc<LPCVOID>(iAddress), &cMBI, sizeof(cMBI)))
	{
		// Try and load it.
		Size				iNameLen = 0;
		std::vector<char>	cBuf(MAX_PATH);

		// ��� ���� �̸��� ���´�
		// using the address base for the memory location, try
		// to grab the module filename.
		iNameLen = GetModuleFileNameA(_rc<HINSTANCE>(cMBI.AllocationBase), &cBuf[0], _sc<U32>(cBuf.size()));

		HANDLE hFile = NULL;

		if (0 != iNameLen)
		{
			// Very cool, I found the DLL.  Now open it up for
			// reading.
			hFile = CreateFileA(&cBuf[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
		}

		// Go ahead and try to load the module anyway.
		U64 qwRet = ::SymLoadModule64(hProcess_, hFile, (iNameLen ? &cBuf[0] : NULL), NULL, _rc<U64>(cMBI.AllocationBase), 0);

		if (0 == qwRet)
		{
			U32		iError = ::GetLastError();
			if (iError != S_OK)
			{
				LOG_WIN32_ERROR(_W("SymLoadModule64()"), iError);
			}
		}

		return true;
	}

	return false;
}

bool
CallStackCapturer::ReadyStackFrame()
{
	// ���� �������� �غ��Ѵ�
	// ready stack frame
	ZeroMemory(&cStackFrame_, sizeof(cStackFrame_));

	cStackFrame_.AddrPC.Offset		= cContext_.Eip;
	cStackFrame_.AddrPC.Mode		= AddrModeFlat;
	cStackFrame_.AddrStack.Offset	= cContext_.Esp;
	cStackFrame_.AddrStack.Mode		= AddrModeFlat;
	cStackFrame_.AddrFrame.Offset	= cContext_.Ebp;
	cStackFrame_.AddrFrame.Mode		= AddrModeFlat;

	return true;
}

UINT __stdcall
CallStackCapturer::ThreadFunc(void* pVoid)
{
	CallStackCapturer*		pThis = _sc<CallStackCapturer*>(pVoid);

	pThis->bSuccess_ = false;

	pThis->cContext_.ContextFlags = CONTEXT_FULL;
	if (!::GetThreadContext(pThis->hDuplicated_, &pThis->cContext_))
	{
		return U32_MAX;
	}

	if (!pThis->ReadyStackFrame())
	{
		return U32_MAX;
	}

	if (!pThis->TraceCallStack())
	{
		return U32_MAX;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		������
///
///	\param[in]	iTraceDepth	ĸ���� �ݽ����� �ִ� depth�� �����Ѵ�.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
CallStack::CallStack(Size iTraceDepth)
	: iTraceDepth_(iTraceDepth)
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		�� ������ �ݽ����� ĸ���Ѵ�.
///
///				�ҷ��� �ָ� �ȴ�.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
void
CallStack::Capture()
{
	CallStackCapturer	cCapturer(this);
	cCapturer.Capture();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		ĸ�� �ϴ� �������� �߰��� ������ ���� ������ �����Ѵ�.
///
///				Capture()�� �θ��� �������� ����, ���� ĸ���۾��� �����ϴ� �� ����
///				���� ������ ���� �߰��� ������ ���������� ���������� �����Ͽ�
///				���� ����ڰ� ĸ�ĸ� ��û�� �������� ������������ �����.\n
///				�ݽ��� �����̳� ���� �ɼǿ� ���� �߰��� �����Ǵ� ������ ���̰� �ٸ��Ƿ�
///				ĸ�ĵǴ� �ݽ��� ������ ���ų� ���ڶ�� �� �Լ� �����ο�
///				�ϵ��ڵ��� ������ ������ depth ������ ������ ��.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
void
CallStack::ExcludeIntermidate()
{
#ifdef _DEBUG
	Size	iIntermediate	= 4;
#else
	Size	iIntermediate	= 3;
#endif

	if (iIntermediate <= cStack_.size())
	{
		CallStackInfoVec::iterator	iterFrom = cStack_.begin();
		cStack_.erase(iterFrom, iterFrom + iIntermediate);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		ĸ�ĵ� ������ ����Ѵ�.
///
///				����� OutputDebugString�� ����Ͽ� Visual Studio IDE��
///				Outputâ���� ���
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
void
CallStack::Dump() const
{
	StringW	sMsg;

	Dump(sMsg);

	::OutputDebugStringW(sMsg.c_str());
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		ĸ�ĵ� ������ ��Ʈ������ ����Ѵ�.
///
///				ĸ�ĵ� ���� ������ ���Ŀ� ���� ��Ʈ������ ����Ѵ�.
///				��µǴ� ������ �ϵ��ڵ��� ���̹Ƿ�
///				������ �ȵ� ��� �ڵ带 �����ϰų� flexible�ϰ� ������ �� �ֵ���
///				������ ��ĥ ��.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
void
CallStack::Dump(StringW& rString) const
{
	rString.clear();

	Size	iCount = cStack_.size();

	for (U32 i = 0; i < iCount; ++i)
	{
		const CallStackInfo&	sTrace = cStack_[i];

		if (sTrace.bSourceInfoValid_)
		{
			rString += StringUtil::Format(_W("%s(%u) : "), sTrace.sSourceName_.c_str(), sTrace.iLine_);

			if (sTrace.iOffsetFromLine_ != 0)
			{
				rString += StringUtil::Format(_W("+ 0x%x"), sTrace.iOffsetFromLine_);
				rString += _W(", ");
			}
		}

		rString += StringW(sTrace.sModuleName_, FileUtil::GetFileNamePos(sTrace.sModuleName_));
		rString += _W("::");
		rString += StringUtil::Format(_W("%s() + 0x%I64x\r\n"), sTrace.sSymbolName_.c_str(), sTrace.iOffsetFromSymbol_);
	}
}
