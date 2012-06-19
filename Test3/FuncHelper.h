#pragma once

class FuncHelper
{
public:
	FuncHelper(void);
	~FuncHelper(void);
};

template <typename T>
struct FunctionCallHelper
{
	typedef typename void (T::*VoidFunction)();
};

class BaseFunction
{
public:
	BaseFunction() {}
	virtual ~BaseFunction() {}; 
	virtual void DoAction() {};
};

template <typename T>
class FunctionCaller : public BaseFunction
{
public:
	FunctionCaller(T* pInst, typename FunctionCallHelper<T>::VoidFunction pFunc){ pInst_ = pInst;  pFunc_ = pFunc; }
	~FunctionCaller() { delete (T*)pInst_; }
	virtual void DoAction() { (pInst_->*pFunc_)(); }
private:
	T* pInst_;
	typename FunctionCallHelper<T>::VoidFunction pFunc_;
};

#define pFuncCallMgr Manager::Instance()
class Manager
{
	static Manager cInstance_;
public:
	Manager() {}
	~Manager() { Destroy(); }

	template <typename T>
	void Register(T* pInst, typename FunctionCallHelper<T>::VoidFunction pFunc)
	{
		FunctionCaller<T>* pCaller = new FunctionCaller<T>(pInst, pFunc);
		Put(pCaller);
	}
	void Put(BaseFunction* pFunc) { cVec_.push_back(pFunc); }
	void DoAction()
	{
		std::vector<BaseFunction*>::iterator cIter = cVec_.begin();
		for (; cIter != cVec_.end(); ++cIter)
		{
			(*cIter)->DoAction();
		}
	}
	void Destroy()
	{
		std::vector<BaseFunction*>::iterator cIter = cVec_.begin();
		for (; cIter != cVec_.end(); ++cIter)
		{
			delete *cIter;
		}
	}
	inline static Manager* Instance() { return &cInstance_; }

private:
	std::vector<BaseFunction*> cVec_;
};