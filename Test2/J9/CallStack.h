// $Id: CallStack.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_CALL_STACK_H__
#define __J9_CALL_STACK_H__

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		콜스택 정보를 추출하기 위한 구조체
	///
	///				콜스택 정보 추출시, 모듈명, 심볼 이름, 파일, 라인, 등의 정보를
	///				저장하고 있는 구조체 이다.
	/// \author		boro
	/// \date		2005/04/25
	/// \see		CallStack
	////////////////////////////////////////////////////////////////////////////////
	struct CallStackInfo
	{
		StringW				sModuleName_;						// 모듈 풀 패스명 / mudule full path name

		StringW				sSymbolName_;						// 심볼 이름 / symbol name
		U64					iOffsetFromSymbol_;					// 심볼로부터의 옵셋 / offset from the symbol

		StringW				sSourceName_;						// 소스 파일 / source file
		U32					iLine_;								// 라인 카운트
		U64					iOffsetFromLine_;					// 라인으로부터의 옵셋 / offset from the line

		bool				bSourceInfoValid_;					// 라인 정보가 유효한 지 여부 / validity of line info

		CallStackInfo();
		CallStackInfo(const CallStackInfo& r);
		CallStackInfo&	operator=(const CallStackInfo& r);
	};

	typedef std::vector<CallStackInfo>	CallStackInfoVec;

	class CallStackCapturer;
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		콜스택을 캡쳐하는 클래스
	///
	///				콜스택을 캡쳐하고, 캡쳐된 콜스택을 출력한다.
	/// \author		boro
	/// \date		2005/04/25
	////////////////////////////////////////////////////////////////////////////////
	class CallStack
	{
		friend class CallStackCapturer;
	public:
		enum
		{
			TraceDepth = 32,
		};

	public:
		CallStack(Size iTraceDepth = TraceDepth);

		void				Capture();
		void				ExcludeIntermidate();
		void				Dump() const;
		void				Dump(StringW& rString) const;

	private:
		Size				iTraceDepth_;
		CallStackInfoVec	cStack_;
	};
};

#endif//__J9_CALL_STACK_H__