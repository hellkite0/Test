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
	// 자신의 스레드 컨텍스트는 올바르게 얻어올 수 없으므로 다른 스레드를 생성한다
	HANDLE				hThread;
	UINT				iThreadId;

	// 현재 스레드의 핸들을 복제한다
	if (!DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(), ::GetCurrentProcess(), &hDuplicated_, THREAD_ALL_ACCESS, false, 0))
	{
		return false;
	}

	hThread = _rc<HANDLE>(_beginthreadex(NULL, NULL, ThreadFunc, _sc<void*>(this), 0, &iThreadId));
	if (hThread != NULL)
	{
		::WaitForSingleObject(hThread, INFINITE);
	}

	// 복제된 핸들을 제거한다
	::CloseHandle(hDuplicated_);

	return bSuccess_;
}

bool
CallStackCapturer::TraceCallStack()
{

	// 트레이스를 수행한다
	for (Size i = 0; i < pCallStack_->iTraceDepth_; ++i)
	{
		CallStackInfo	cTrace;

		if (!TraceCallStackOneStep(cTrace))
		{
			break;
		}

		if (cStackFrame_.AddrPC.Offset != NULL)
		{
			// 스택 트레이스 결과에 추가한다
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

	// 모듈 정보를 강제로 로드한다
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

	// 소스 파일 정보가 유효하다
	rStackTrace.bSourceInfoValid_ = true;

	return true;
}

bool
CallStackCapturer::LoadModuleByAddress(U64 iAddress)
{
	MEMORY_BASIC_INFORMATION	cMBI;

	// VirtualQuery()로 이미지의 정보를 얻어온다
	// do the VirtualQueryEx() to see if I can find the start of
	// this module.  Since the HMODULE is the start of a module
	// in memory, viola, this will give me the HMODULE.
	if (0 != VirtualQueryEx(hProcess_, _rc<LPCVOID>(iAddress), &cMBI, sizeof(cMBI)))
	{
		// Try and load it.
		Size				iNameLen = 0;
		std::vector<char>	cBuf(MAX_PATH);

		// 모듈 파일 이름을 얻어온다
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
	// 스택 프레임을 준비한다
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
/// \brief		생성자
///
///	\param[in]	iTraceDepth	캡쳐할 콜스택의 최대 depth를 설정한다.
/// \author		boro
/// \date		2005/04/25
////////////////////////////////////////////////////////////////////////////////
CallStack::CallStack(Size iTraceDepth)
	: iTraceDepth_(iTraceDepth)
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief		현 시점의 콜스택을 캡쳐한다.
///
///				불러만 주면 된다.
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
/// \brief		캡쳐 하는 시점에서 추가로 생성된 스택 정보를 삭제한다.
///
///				Capture()를 부르는 시점에서 부터, 실제 캡쳐작업을 수행하는 곳 까지
///				내부 구현에 의해 추가로 생성된 시점까지의 스택정보를 삭제하여
///				실제 사용자가 캡쳐를 요청한 곳까지의 스택정보만을 남긴다.\n
///				콜스택 구현이나 빌드 옵션에 따라 추가로 생성되는 스택의 깊이가 다르므로
///				캡쳐되는 콜스택 정보가 남거나 모자라면 이 함수 구현부에
///				하드코딩된 삭제할 스택의 depth 정보를 수정할 것.
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
/// \brief		캡쳐된 스택을 출력한다.
///
///				현재는 OutputDebugString을 사용하여 Visual Studio IDE의
///				Output창으로 출력
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
/// \brief		캡쳐된 스택을 스트링으로 출력한다.
///
///				캡쳐된 스택 정보를 형식에 맞춰 스트링으로 출력한다.
///				출력되는 형식은 하드코딩된 것이므로
///				마음에 안들 경우 코드를 수정하거나 flexible하게 수정할 수 있도록
///				구현을 고칠 것.
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
