// $Id: Exception.h 6196 2010-06-17 06:03:40Z hyojin.lee $
// this file name should be changed.

#ifndef __J9_EXCEPTION_H__
#define __J9_EXCEPTION_H__

namespace J9
{
	// 일단 Win32기준으로 작성하고 포팅이 필요할 경우 추가 구현을 한다.
	class Error
	{
	protected:
		Error()					: iError_(0) { sError_[0] = 0; }
		Error(U32 iError);

	public:
		virtual ~Error() {}

		const WChar*	GetErrorMessage() const	{ return sError_; }

		void	Throw() { __debugbreak(); throw *this; }
		void	Report();

	protected:
		U32				iError_;
		WChar			sError_[1024];
	};

	enum J9ErrorCode
	{
		J9ECGeneral,
		J9ECFileOpen,
		J9ECCasting,
		J9ECInvalidArg,
		J9ECHardware,
	};

	class J9Error : public Error
	{
	public:
		J9Error(J9ErrorCode iError);
		J9Error(const WChar* pMsg);

	private:
		static const WChar*	GetErrorMsg(J9ErrorCode iError);
	};

	class SystemError : public Error
	{
	public:
		SystemError(HRESULT hError);
	};

	class COMError : public Error
	{
	public:
		COMError(HRESULT hError);
	};

	template<typename _Exception>
	void Throw()
	{
		throw _Exception();
	}

	template<typename _Exception, typename Arg1>
	void Throw(const Arg1& cArg1)
	{
		throw _Exception(cArg1);
	}

	template<typename _Exception, typename Arg1, typename Arg2>
	void Throw(const Arg1& cArg1, const Arg2& cArg2)
	{
		throw _Exception(cArg1, cArg2);
	}
};

#endif//__J9_EXCEPTION_H__