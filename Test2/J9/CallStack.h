// $Id: CallStack.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_CALL_STACK_H__
#define __J9_CALL_STACK_H__

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		�ݽ��� ������ �����ϱ� ���� ����ü
	///
	///				�ݽ��� ���� �����, ����, �ɺ� �̸�, ����, ����, ���� ������
	///				�����ϰ� �ִ� ����ü �̴�.
	/// \author		boro
	/// \date		2005/04/25
	/// \see		CallStack
	////////////////////////////////////////////////////////////////////////////////
	struct CallStackInfo
	{
		StringW				sModuleName_;						// ��� Ǯ �н��� / mudule full path name

		StringW				sSymbolName_;						// �ɺ� �̸� / symbol name
		U64					iOffsetFromSymbol_;					// �ɺ��κ����� �ɼ� / offset from the symbol

		StringW				sSourceName_;						// �ҽ� ���� / source file
		U32					iLine_;								// ���� ī��Ʈ
		U64					iOffsetFromLine_;					// �������κ����� �ɼ� / offset from the line

		bool				bSourceInfoValid_;					// ���� ������ ��ȿ�� �� ���� / validity of line info

		CallStackInfo();
		CallStackInfo(const CallStackInfo& r);
		CallStackInfo&	operator=(const CallStackInfo& r);
	};

	typedef std::vector<CallStackInfo>	CallStackInfoVec;

	class CallStackCapturer;
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		�ݽ����� ĸ���ϴ� Ŭ����
	///
	///				�ݽ����� ĸ���ϰ�, ĸ�ĵ� �ݽ����� ����Ѵ�.
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