// $Id: Containers.h 9014 2011-07-22 08:21:30Z hyojin.lee $

#ifndef __J9_CONTAINERS_H__
#define __J9_CONTAINERS_H__

#include "HashFunc.h"
#include "Mutex.h"
#include "Random.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief		정렬된 자료구조(set, map, multi_set, multi_map 및 그외의 정렬된 것을
///				보장하는 자료구조)에서 검색에서 원소를 찾는 방법을 지정한다
///
///	정렬되어있는 자료구조에서 검색 조건과 주어진 키를 이용하여 해당하는 키를 찾는다.
/// 검색 조건은 아래와 같이 정할 수 있지만, 여러 자료구조에 대한 자세한 구현을
/// 템플릿으로 할 것인지 아니면 다른 방법을 사용하여 구현할 것인지에 대해서는
/// 논의가 더 필요하다. 일단은 아래와 같은 방법으로 검색을 할 필요가 있는 곳에서
/// 구현이 필요한 부분을 스스로 구현하도록 한다.
///
/// \author		boro
/// \date		2005/05/20
////////////////////////////////////////////////////////////////////////////////
enum FindMethod
{
	FindFirst,		//< first key
	FindLast,		//< last key
	FindNext,		//< first key after time, including time
	FindPrev,		//< last key before time, including time
	FindFrom,		//< first key after time, not including time
	FindTo,			//< last key before time, not including time
	NumFindMethod,	//< number of FindMethod
};

//IMPORTANT NOTE
//일단 standard class를 상속받아서 구현된 모든 클래스는 disable한다.
//필요에 의해서 재구현 해야 하는 경우에는 심사숙고하여 결정하고 재구현 하도록 하자.
//포팅이나 코드 튜닝, 리팩토링에 심각한 영향을 미칠 수 있다.
//심지어 hash_map은 아직까지 standard도 아니다.
//standard class를 상속받아 사용하는 것은 포팅시 다음과 같은 문제점을 야기할 수 있다.
//1. 드문 경우지만, 그리고 특히 표준이 아닌 경우에 더 많이 발생하는 문제인데.
//   표준 규약이 바뀔 때 해당 클래스를 사용하는 많은 부분에서 구현을 수정해야하는 문제가 발생한다.
//   예를 들어 stlport의 hash_map를 ms stl의 hash_map으로 바꿀 때 해시 값을 만드는 부분의 클래스 구조가 달라져서
//   HashFunc를 사용하는 곳들의 코드를 수정해야 하는 것과 같은 문제이다.
//   이런 경우의 부작용을 최소화하기 위해서 부득이 standard class를 wrapping해야하는 경우에는
//   상속이 아닌 멤버로 해당 클래스를 가지고 있고, 외부에 노출되는 interface는 최소가 되도록 한다.
//   즉 is-a가 아닌 has-a관계가 되도록 한다.
//
//2. 클래스의 크기가 작을수록, 기능이 간단할 수록 코드가 일목 요연해지고, 유지보수가 쉽다는 관점에서 볼 때
//   기능성 함수들을 클래스 함수로 구현하는 것은 옳지 못하다.
//   특히 자체 제작 클래스가 아닌 표준 클래스를 상속받아서 구현하는 것은
//   외부 모듈과의 연동등에 안좋은 영향을 미칠 수 있다.
//   예를 들어 std::wstring을 상속받은 StringW클래스를 사용하는 함수가 외부 모듈에 스트링을 전달할 때는
//   std::wstring으로 다시 캐스팅해야할 필요가 있고, 특히 standard container들의 특성상 가상함수를 사용하지 않는 등
//   기본적으로 상속을 고려하지 않고 디자인된 클래스를 down casting하는 것은 위험하다.
//   그래서 standard class에 특정 기능을 추가하고자 할 때는 utility function과 같은 형식으로 구현하도록 한다.
//   stl에서 container, iterator, algorithm, functor를 구분해 놓은 이유에대해서
//   다시 한번 고민해 볼 필요가 있는 것이다.

namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		std::list 을 warping 한 List Class
	///
	/// standard library 에서 필요한 interface를 제공한다.
	/// 필요한 함수가 있다면, 따로 구현하거나 standard libaray 에서 warping 하도록 한다.
	///
	/// \author		nevermind
	/// \date		2006/03/02
	////////////////////////////////////////////////////////////////////////////////
//#ifdef _SERVER
	template<class T>
	class List : private std::list<T>
	{
	public:
		typedef typename std::list<T>::iterator Iterator;
		typedef typename std::list<T>::reverse_iterator RIterator;
		typedef typename std::list<T>::const_iterator ConstIterator;

		inline Iterator Begin()			{ return begin(); }
		inline Iterator End()			{ return end(); }
		
		inline RIterator RBegin()		{ return rbegin(); }
		inline RIterator REnd()			{ return rend(); }

		Size Size() const				{ return size(); }
		bool IsEmpty() const			{ return empty(); }
		void Clear()					{ clear(); }

		void InsertHead(const T& rItem)	{ push_front(rItem); }
		void InsertTail(const T& rItem)	{ push_back(rItem); }
		void Insert(ConstIterator cIter, const T& rItem)		{ insert(cIter, rItem); }

		void RemoveHead()				{ pop_front(); }
		void RemoveTail()				{ pop_back(); }

		const T& GetHead() const		{ return front(); }
		const T& GetTail() const		{ return back(); }

		void Remove(const T& rItem)		{ this->remove(rItem); }
		Iterator Erase(ConstIterator cIter) { return erase(cIter); }

	private:
	};
//#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		std::set 을 warping 한 Set Class
	///
	/// standard library 에서 필요한 interface 인 insert, remove, upper_bound를 제공한다.
	/// 필요한 함수가 있다면, 따로 구현하거나 standard libaray 에서 warping 하도록 한다.
	///
	/// \author		nevermind
	/// \date		2006/03/02
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class Set:  private std::set<T>
	{
	public:
		typedef typename std::set<T>::iterator Iterator;
		typedef typename std::set<T>::const_iterator ConstIterator;

		void Put(T cKey) { insert(cKey); }
		void Remove(T cKey) { erase(cKey); }
		Iterator UpperBound(T cKey) { return upper_bound(cKey); }

	private:
	};
	//extern Lock* que_mutex;
#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		FIFO, first in first out,의 que data structure implementation
	///
	/// standard library 에서 필요한 interface 인 push 그리고 pop 을 제공한다.
	/// 필요한 함수가 있다면, 따로 구현하도록 한다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class Que: private std::deque<T>
	{
	public:
		Que() : bOwnLock_(true)			{ pLock_ = new Lock(); }
		Que(Lock* p)					{ pLock_ = p; bOwnLock_ = false; }
		virtual	~Que()					{ if (bOwnLock_) delete pLock_; }

		inline Size Size() { return size(); }
		inline void Clear() { return clear(); }
		inline bool IsEmpty() { return empty(); }

		void Push(const T& c)	{
			if (pLock_)
			{
				pLock_->Lock();
				push_back(c);
				pLock_->Unlock();
			}
			else push_back(c);
		}

		T Pop()
		{
			T c = 0;
			if (pLock_)
			{
				pLock_->Lock();
				if (!empty()) { c = front(); pop_front(); }
				pLock_->Unlock();
			}
			else if (!empty()) { c = front(); pop_front(); }
			CASSERT(sizeof(T) <= 8, Too_Much_Que_Copy_Operation);

			return c;
		}

		T Top()
		{
			T c = 0;
			if (pLock_)
			{
				pLock_->Lock();
				if (!empty()) { c = front(); }
				pLock_->Unlock();
			}
			else if (!empty()) { c = front(); }
			CASSERT(sizeof(T) <= 8, Too_Much_Que_Copy_Operation);

			return c;
		}

		void Top(T& c) {
			if (pLock_)
			{
				pLock_->Lock();
				if (!Que::empty()) { c = front(); }
				pLock_->Unlock();
			}
			else if (!empty()) { c = front(); }
		}

		void Pop(T& c) {
			if (pLock_)
			{
				pLock_->Lock();
				if (!Que::empty()) { c = front(); pop_front(); }
				pLock_->Unlock();
			}
			else if (!empty()) { c = front(); pop_front(); }
		}

	private:
		Lock*		pLock_;
		bool		bOwnLock_;
	};
#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		FIFO, first in first out,의 que data structure implementation
	///				Conditional Lock 를 이용 Que에 Item 이 들어올때 바로 받아서 쓸수있는
	///				Wait 함수가 따로 제공된다.
	///
	/// Que 와 마찬가지로 standard library 에서 필요한 interface 인 push 그리고 pop 을 제공한다.
	/// 또한 Conditional Lock 를 이용한 Wait 함수도 제공된다.
	/// 필요한 함수가 있다면, 따로 구현하도록 한다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class CQue:  private std::deque<T>
	{
	private:
		ConditionalMutex*	pLock_;
		bool		bOwnLock_;

	public:
		CQue() : bOwnLock_(true)	{ pLock_ = new ConditionalMutex(); }
		CQue(ConditionalMutex* p) : bOwnLock_(false) { pLock_ = p; }
		virtual	~CQue()				{ if (bOwnLock_) delete pLock_; }

		inline Size Size() { return size(); }
		inline void Signal()
		{
			// conditional que 는 que에 객체가 없는 상태에서는 절대로 signal로 wait 해제가 되지 않는다. 뺑뺑이 구조라서 그렇다.
			ASSERT(false);
		}
		inline void Broadcast()
		{
			// conditional que 는 que에 객체가 없는 상태에서는 절대로 signal로 wait 해제가 되지 않는다. 뺑뺑이 구조라서 그렇다.
			ASSERT(false);
		}

		void Push(const T c) {
			pLock_->Lock();
			push_back(c);
			pLock_->Signal();
			pLock_->Unlock();
		}

		T Pop() {
			T c = 0;
			pLock_->Lock();
			if (!empty()) { c= front(); pop_front(); }
			pLock_->Unlock();

			CASSERT(sizeof(T) <= 8, Too_Much_Conditional_Que_Copy_Operation);
			return c;
		}

		void Pop(T& c) {
			pLock_->Lock();
			if (!empty()) { c = front(); pop_front(); }
			pLock_->Unlock();
		}

		T Wait() {
			T c = 0;
			pLock_->Lock();
			while (!c) {
				if (!empty())
				{
					c = front();
					pop_front();
				}
				else
				{
					pLock_->Wait();
				}
			}
			pLock_->Unlock();

			CASSERT(sizeof(T) <= 8, Too_Much_Conditional_Que_Wait_Copy_Operation);
			return c;
		}

		void Wait(T& c) {
			pLock_->Lock();
			while (!c) {
				if (!empty()) {
					c = front();
					pop_front();
				} else pLock_->Wait();
			}
			pLock_->Unlock();
		}
	};
#endif


	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Hash Map data structure implemenation
	///
	/// Standard Library에서 필요한 함수들을 상속된 Class이다.
	/// 필요한 함수가 있다면, 따로 상속받거나 구현하도록 한다.
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<typename Key, typename Data, typename HashCompare = stdext::hash_compare<Key, std::less<Key> > >
	class HashMap : private stdext::hash_map<Key, Data, HashCompare>
	{
		typedef typename value_type			Pair;
		typedef typename ArgType<Key>::Type	KeyArgType;

	public:

		HashMap(J9::Lock *p) :bOwnLock_(false) { pLock_ = p; }
		HashMap() :bOwnLock_(true) { pLock_ = new J9::Lock(); }
		~HashMap() 
		{
			if (bOwnLock_ && pLock_) 
			{ 
				delete pLock_; 
				pLock_ = NULL;
			}
		}

		void Lock() { pLock_->Lock(); }
		void Unlock() { pLock_->Unlock(); }

		typedef typename stdext::hash_map<Key, Data, HashCompare>::iterator Iterator;
		typedef typename stdext::hash_map<Key, Data, HashCompare>::const_iterator ConstIterator;

		inline Iterator End() { LOCK_SCOPE(pLock_); return this->end(); }
		inline Iterator Begin() { LOCK_SCOPE(pLock_); return this->begin(); }

		inline void Clear() { LOCK_SCOPE(pLock_); this->clear(); }

		void	Remove(KeyArgType cKey) { LOCK_SCOPE(pLock_); this->erase(cKey); }
		Iterator Erase(Iterator cIter) { LOCK_SCOPE(pLock_); return this->erase(cIter); }

		void	Put(KeyArgType cKey, Data cData, bool bReplace = false)
		{
			LOCK_SCOPE(pLock_);
			if (bReplace) (*this)[cKey] = cData;
			else insert(Pair(cKey, cData)); // won't insert if already exists
		}

		template<typename T>
		struct NULL_TYPE {};

		template<>
		struct NULL_TYPE<S32>
		{
			S32 operator()() const
			{
				return 0;
			}
		};

		inline bool Has(KeyArgType cKey)
		{
			LOCK_SCOPE(pLock_);
			return this->find(cKey) != this->end();
		}

		const Data&	Get(KeyArgType cKey) const
		{
			LOCK_SCOPE(pLock_);
			const_iterator iter = this->find(cKey);
			if (iter == this->end()) return NULL_TYPE<typename Data>()();

			return iter->second;
		}

		Data&	Get(KeyArgType cKey)
		{
			LOCK_SCOPE(pLock_);
			static Data dummy = Data();
			iterator iter = this->find(cKey);
			if (iter == this->end()) return dummy;

			return iter->second;
		}

		inline Size Size() { return this->size(); }
		J9::Lock* pLock_;
		bool bOwnLock_;
	};
#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		U64를 Key로 하는 Hash Map
	///
	/// \author		thinkong
	/// \date		2007/10/25
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class LargeIndexPool : public HashMap<U64, T>
	{
	public:
		LargeIndexPool() : HashMap<U64, T>() {}
		LargeIndexPool(J9::Lock *p) : HashMap<U64, T>(p) {}
	};
#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Integer를 Key로 하는 Hash Map
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class IndexPool : public HashMap<S32, T>
	{
	public:
		IndexPool() : HashMap<S32, T>() {}
		IndexPool(J9::Lock *p) : HashMap<S32, T>(p) {}
	};
#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		String을 Key로 하는 Hash Map
	///
	///
	/// \author		nevermind
	/// \date		2005/06/23
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class NamedPool : public HashMap<StringW, T>
	{
	public:
		NamedPool() : HashMap<StringW, T>() {}
		NamedPool(J9::Lock *p) : HashMap<StringW, T>(p) {}
	};
#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		String을 Key로 하는 Case Insensitive 한 Hash Map
	///
	///	case insensitive 한 namedpool을 사용하려면 CompFunc로 J9::IEqualTo<>를 주면 된다.
	///
	/// \author		nevermind
	/// \date		2007/09/05
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER
	template<class T>
	class INamedPool : public HashMap<StringW, T, IHashCompare<StringW> >
	{
	public:
		INamedPool() : HashMap<StringW, T, IHashCompare<StringW> >() {}
		INamedPool(J9::Lock *p) : HashMap<StringW, T, IHashCompare<StringW> >(p) {}
	};
#endif



#ifdef _SERVER

#pragma region Templates
	////////////////////////////////////////////////////////////////////////////////
	/// \brief		String 과 U64로 찾을수 있는 맵
	///
	///
	/// \author		nokdu
	/// \date		2011/06/14
	////////////////////////////////////////////////////////////////////////////////
	template<class T>
	class NameLargeIndexPool
	{
	public:
		NameLargeIndexPool()
			:cNamedPool_(cIndexPool_.pLock_)
		{}
		void Put(J9::String s, U64 i, T t) {cIndexPool_.Put(i, t); cNamedPool_.Put(s,t); }
		T Get(J9::String s) { return cNamedPool_.Get(s);}
		T Get(U64 i) {return cIndexPool_.Get(i);}
		bool Has(U64 i) { return cIndexPool_.Has(i); }
		bool Has(J9::String s) { return cNamedPool_.Has(s); }
		
		// private:
		J9::LargeIndexPool<T>	cIndexPool_;
		J9::NamedPool<T>	cNamedPool_;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		String 과 U32로 찾을수 있는 맵
	///
	///
	/// \author		nokdu
	/// \date		2011/06/14
	////////////////////////////////////////////////////////////////////////////////
	template<class T>
	class NameIndexPool
	{
	public:
		NameIndexPool()
			:cNamedPool_(cIndexPool_.pLock_)
		{}
		void Put(J9::String s, U32 i, T t) {cIndexPool_.Put(i, t, true); cNamedPool_.Put(s, t, true); }
		T Get(J9::String s) { return cNamedPool_.Get(s);}
		T Get(U32 i) {return cIndexPool_.Get(i);}
		bool Has(U32 i) { return cIndexPool_.Has(i); }
		bool Has(J9::String s) { return cNamedPool_.Has(s); }
		//private:
		J9::IndexPool<T>	cIndexPool_;
		J9::NamedPool<T>	cNamedPool_;
	};

#endif

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		CircularVector
	///
	/// \author		hyojin.lee
	/// \date		2006/06/07
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class CircularVector
	{
	public:
		// constructor
		CircularVector()					{ Clear(); }
		CircularVector(Size iSize)			{ Resize(iSize); }
		virtual	~CircularVector() {}

		void Clear()
		{
			cElems_.clear();
			iHead_ = 0;
			iTail_ = 0;
			iSize_ = 0;
			bOverflow_ = false;
		}

		// clear and resize
		void Resize(Size iSize)
		{
			Clear();

			// free reserved memory
			Elements().swap(cElems_);
			cElems_.reserve(iSize);
			iSize_ = iSize;
		}

		Size GetSize() const
		{
			return bOverflow_ ? iSize_ : (iTail_ - iHead_);
		}

		void Push(const T& rElem)
		{
			ASSERT(iSize_ != 0);

			// append elem
			if (bOverflow_)
				cElems_[iTail_] = rElem;
			else
				cElems_.push_back(rElem);

			// move head if overflowed
			if (bOverflow_)
			{
				ASSERT(iHead_ == iTail_);
				iHead_ = (iHead_ + 1) % iSize_;
			}

			// move tail
			iTail_ = (iTail_ + 1) % iSize_;

			// check overflow
			if (!bOverflow_)
                bOverflow_ = cElems_.size() == iSize_;
		}
		const T& Get(Size i) const
		{
			ASSERT(iSize_ != 0);

			if (bOverflow_)
			{
				Size iAdj = (iHead_ + i) % iSize_;

				return cElems_[iAdj];
			}

			ASSERT(i < iTail_);
			return cElems_[i];
		}

		void Remove(Size i)
		{
			UNUSED(i);
			CASSERT(0, NOT_IMPL);
		}

	protected:
		typedef std::vector<T>
		Elements;

		Elements	cElems_;
		Size		iHead_;		// points head elem
		Size		iTail_;		// points appending point
		Size		iSize_;
		bool		bOverflow_;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		std::vector<>에 대한 TypeTraits
	/// \author		hyojin.lee
	/// \date		2006/08/28
	////////////////////////////////////////////////////////////////////////////////
	template<typename IO, typename T>
	struct TypeTraits_Binary_Custom<IO, std::vector<T> >
	{
		static Size
		Read(IO* pIO, std::vector<T>& r)
		{
			CASSERT(TypeCategory<T>::bValueType, NOT_VALUE_TYPE);
			r.clear();

			Size	iRead = 0;
			U32		iSize;

			iRead += TypeTraits_Binary::Read(pIO, iSize);
			ASSERT(iRead == sizeof(iSize));

			if (0 < iSize)
			{
				r.resize(iSize);
				iRead += pIO->Read(iSize * sizeof(T), &r[0]);
			}

			return iRead;
		}

		static Size
		Write(IO* pIO, const std::vector<T>& r)
		{
			CASSERT(TypeCategory<T>::bValueType, NOT_VALUE_TYPE);
			Size	iWritten = 0;
			U32		iSize = _sc<U32>(r.size());

			iWritten += TypeTraits_Binary::Write(pIO, iSize);
			if (0 < iSize)
				iWritten += pIO->Write(iSize * sizeof(T), &r[0]);

			return iWritten;
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		std::vector 을 warping 한 array Class
	///
	/// standard library 에서 필요한 interface를 제공한다.
	/// 필요한 함수가 있다면, 따로 구현하거나 standard libaray 에서 wraping 하도록 한다.
	///
	/// \author		nevermind
	/// \date		2006/07/02
	////////////////////////////////////////////////////////////////////////////////
#ifdef _SERVER

	template<class T>
	class Array : private std::vector<T>
	{
	public:
		typedef typename std::vector<T>::iterator Iterator;
		typedef typename std::vector<T>::const_iterator ConstIterator;

	public:
		Iterator	Begin()				{ return this->begin(); }
		Iterator	End()				{ return this->end(); }

		bool	IsEmpty() const			{ return empty(); }
		void	Clear()					{ clear(); }
		Size	Capacity() const		{ return capacity(); }
		Size	MaxSize() const			{ return max_size(); }
		void	Resize(Size i)				{ resize(i); }
		T&		operator[](Size i) 		{ return this->at(i); }
		T&		Get(Size i)				{ return this->at(i); }
		Size	GetSize() const			{ return size(); }
		void	Add(const T& rItem)		{ this->push_back(rItem); }		
		void	Erase(Iterator cPos)		{ this->erase(cPos); }
		void	RemoveLast()			{ if (empty()) return; this->erase(this->end() - 1); }
		T&		GetLast()				{ return this->back(); }
		void	Shuffle(U8 i = 1)
		{
			for (Size j = 0; j < i; ++j)
			{
				Size	iSize = this->GetSize();
				for (Size k = 0; k < iSize; ++k)
				{
					Size	iTarget = RAND_U32() % iSize;
					if (k == iTarget) continue;
					std::swap(this->at(k), this->at(iTarget));
				}
			}			
		}
	};
#endif

	template<typename Key, typename Value>
	class LookupTable
	{
		typedef std::map<Key, Value>	Map;

	public:
		void	Add(typename J9::ArgType<Key>::Type cKey, typename J9::ArgType<Value>::Type cValue)
		{
			cMap_[cKey] = cValue;
		}

		Value	Get(typename J9::ArgType<Key>::Type cKey)
		{
			// 이후 키
			Map::iterator	cNext;
			cNext = cMap_.lower_bound(cKey);
			if (cNext == cMap_.end())	return TypeHelper<Value>::Zero();

			// 이전 키
			Map::iterator	cPrev;
			cPrev = cMap_.upper_bound(cKey);
			if (cPrev == cMap_.begin())	return TypeHelper<Value>::Zero();
			--cPrev;

			if (cPrev->first != cNext->first)
				return Lerp<Value>(cPrev->second, cNext->second, 1.f - ((cNext->first - cKey) / (cNext->first - cPrev->first)));
			else
				return cPrev->second;
		}

		bool	Remove(typename J9::ArgType<Key>::Type cKey)
		{
			Map::iterator cIter = cMap_.find(cKey);
			if (cMap_.end() == cIter)
				return false;

			cMap_.erase(cKey);
			return true;
		}

		void	Clear()
		{
			cMap_.clear();
		}

	private:
		Map	cMap_;
	};

	template<typename T>
	void
	MoveFront(typename std::list<T>& rList, typename std::list<typename T>::iterator cIter)
	{
		std::list<T>	cTempList;
		cTempList.splice(cTempList.begin(), rList, cIter);
		rList.splice(rList.begin(), cTempList);
	}

#pragma push_macro("max")
#undef max
#pragma push_macro("new")
#undef new

	struct __Position
	{
	};
	typedef __Position* Position;

	/* generic version */
	template<typename T>
	inline bool AtlAdd(T* ptResult, T tLeft, T tRight)
	{
		if(std::numeric_limits<T>::max() - tLeft < tRight)
		{
			return false;
		}
		*ptResult= tLeft + tRight;
		return true;
	}

	/* generic but compariatively slow version */
	template<typename T>
	inline bool AtlMultiply(T* ptResult, T tLeft, T tRight)
	{
		/* avoid divide 0 */
		if(tLeft==0)
		{
			*ptResult=0;
			return true;
		}
		if(std::numeric_limits<T>::max() / tLeft < tRight)
		{
			return false;
		}
		*ptResult= tLeft * tRight;
		return true;
	}

	/* fast	version	for	32 bit integers	*/
	template<>
	inline bool AtlMultiply(int *piResult, int iLeft, int iRight)
	{
		__int64 i64Result=static_cast<__int64>(iLeft) * static_cast<__int64>(iRight);
		if(i64Result>INT_MAX || i64Result < INT_MIN)
		{
			return false;
		}
		*piResult=static_cast<int>(i64Result);
		return true;
	}

	template<>
	inline bool AtlMultiply(unsigned int	*piResult, unsigned int iLeft, unsigned int iRight)
	{
		unsigned __int64 i64Result=static_cast<unsigned __int64>(iLeft) * static_cast<unsigned __int64>(iRight);
		if(i64Result>UINT_MAX)
		{
			return false;
		}
		*piResult=static_cast<unsigned int>(i64Result);
		return true;
	}

	template<>
	inline bool AtlMultiply(long *piResult, long iLeft, long iRight)
	{
		__int64 i64Result=static_cast<__int64>(iLeft) * static_cast<__int64>(iRight);
		if(i64Result>LONG_MAX || i64Result < LONG_MIN)
		{
			return false;
		}
		*piResult=static_cast<long>(i64Result);
		return true;
	}

	template<>
	inline bool AtlMultiply(unsigned long *piResult, unsigned long iLeft, unsigned long iRight)
	{
		unsigned __int64 i64Result=static_cast<unsigned __int64>(iLeft) * static_cast<unsigned __int64>(iRight);
		if(i64Result>ULONG_MAX)
		{
			return false;
		}
		*piResult=static_cast<unsigned long>(i64Result);
		return true;
	}

	struct AtlPlex     // warning variable length structure
	{
		AtlPlex* pNext;
		// BYTE data[maxNum*elementSize];

		void* GetData() { return this+1; }

		static AtlPlex* Create(AtlPlex*& head, size_t nMax, size_t cbElement);
		// like 'calloc' but no zero fill
		// may throw memory exceptions

		void FreeDataChain();       // free this one and links
	};

	inline AtlPlex* AtlPlex::Create(AtlPlex*& pHead, size_t nMax, size_t nElementSize)
	{
		AtlPlex* pPlex;

		ASSERT(nMax > 0);
		ASSERT(nElementSize > 0);

		size_t nBytes=0;
		if(!AtlMultiply(&nBytes, nMax, nElementSize) || !AtlAdd(&nBytes, nBytes, sizeof(AtlPlex)))
		{
			return NULL;
		}
		pPlex = static_cast< AtlPlex* >(malloc(nBytes));
		if(pPlex == NULL)
		{
			return(NULL);
		}

		pPlex->pNext = pHead;
		pHead = pPlex;

		return(pPlex);
	}

	inline void AtlPlex::FreeDataChain()
	{
		AtlPlex* pPlex;

		pPlex = this;
		while(pPlex != NULL)
		{
			AtlPlex* pNext;

			pNext = pPlex->pNext;
			free(pPlex);
			pPlex = pNext;
		}
	}


	template< typename T >
	class ElementTraitsBase
	{
	public:
		typedef const T& INARGTYPE;
		typedef T& OUTARGTYPE;

		static void CopyElements(T* pDest, const T* pSrc, size_t nElements)
		{
			for(size_t iElement = 0; iElement < nElements; iElement++)
			{
				pDest[iElement] = pSrc[iElement];
			}
		}

		static void RelocateElements(T* pDest, T* pSrc, size_t nElements)
		{
			// A simple memmove works for nearly all types.
			// You'll have to override this for types that have pointers to their
			// own members.
			Checked::memmove_s(pDest, nElements*sizeof(T), pSrc, nElements*sizeof(T));
		}
	};

	template<typename T>
	inline ULONG CalcHash(T _Begin, T _End)
	{	// hash range of elements
		size_t _Val = 2166136261U;
		while(_Begin != _End)
			_Val = 16777619U * _Val ^ (size_t)*_Begin++;
		return (_Val);
	}

	template<typename T>
	class DefaultHashTraits
	{
	public:
		static ULONG Hash(const T& tElement) throw()
		{
			__if_exists (T::GetHash)
			{
				return tElement.GetHash();
			}
			__if_not_exists (T::GetHash)
			{
				return(ULONG(ULONG_PTR(tElement)));
			}
		}
	};

	template<>
	class DefaultHashTraits<J9::StringW>
	{
	public:
		static ULONG Hash(const J9::StringW& rElement) throw()
		{
			const WChar*	p = rElement.c_str();

			return CalcHash<const WChar*>(p, p + rElement.size());
		}
	};

	template< typename T >
	class DefaultCompareTraits
	{
	public:
		static bool CompareElements(const T& tElement1, const T& tElement2)
		{
			return((tElement1 == tElement2) != 0);  // != 0 to handle overloads of operator== that return BOOL instead of bool
		}

		static int CompareElementsOrdered(const T& tElement1, const T& tElement2)
		{
			if(tElement1 < tElement2)
			{
				return(-1);
			}
			else if(tElement1 == tElement2)
			{
				return(0);
			}
			else
			{
				ASSERT(tElement1 > tElement2);
				return(1);
			}
		}
	};

	template< typename T >
	class DefaultElementTraits :
		public ElementTraitsBase< T >,
		public DefaultHashTraits< T >,
		public DefaultCompareTraits< T >
	{
	};

	template< typename T >
	class ElementTraits :
		public DefaultElementTraits< T >
	{
	};

	template< typename Key, typename Value, class KTraits = ElementTraits< Key >, class VTraits = ElementTraits< Value > >
	class AtlMap
	{
	public:
		typedef typename KTraits::INARGTYPE KINARGTYPE;
		typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
		typedef typename VTraits::INARGTYPE VINARGTYPE;
		typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

		class Pair : public __Position
		{
		protected:
			Pair(KINARGTYPE key) : tKey_(key)
			{
			}

		public:
			Key		tKey_;
			Value	tValue_;
		};

	private:
		class Node : public Pair
		{
		public:
			Node(KINARGTYPE tKey, UINT iHash) : Pair(tKey), iHash_(iHash)
			{
			}

		public:
			UINT GetHash() const throw()
			{
				return(iHash_);
			}

		public:
			Node* pNext_;
			UINT iHash_;
		};

	public:
		AtlMap(UINT nBins = 17, float fOptimalLoad = 0.75f, float fLoThreshold = 0.25f, float fHiThreshold = 2.25f, UINT nBlockSize = 10) throw();

		size_t GetCount() const throw();
		bool IsEmpty() const throw();

		bool Find(KINARGTYPE key, VOUTARGTYPE value) const;
		const Pair* Find(KINARGTYPE key) const throw();
		Pair* Find(KINARGTYPE key) throw();
		Value& operator[](KINARGTYPE key) throw(...);

		Position SetAt(KINARGTYPE key, VINARGTYPE value);
		void SetValueAt(Position pos, VINARGTYPE value);

		bool RemoveKey(KINARGTYPE key) throw();
		void RemoveAll();
		void RemoveAtPos(Position pos) throw();

		Position GetStartPosition() const throw();
		void GetNextAssoc(Position& pos, KOUTARGTYPE key, VOUTARGTYPE value) const;
		const Pair* GetNext(Position& pos) const throw();
		Pair* GetNext(Position& pos) throw();
		const Key& GetNextKey(Position& pos) const;
		const Value& GetNextValue(Position& pos) const;
		Value& GetNextValue(Position& pos);
		void GetAt(Position pos, KOUTARGTYPE key, VOUTARGTYPE value) const;
		Pair* GetAt(Position pos) throw();
		const Pair* GetAt(Position pos) const throw();
		const Key& GetKeyAt(Position pos) const;
		const Value& GetValueAt(Position pos) const;
		Value& GetValueAt(Position pos);

		UINT GetHashTableSize() const throw();
		bool InitHashTable(UINT nBins, bool bAllocNow = true);
		void EnableAutoRehash() throw();
		void DisableAutoRehash() throw();
		void Rehash(UINT nBins = 0);
		void SetOptimalLoad(float fOptimalLoad, float fLoThreshold, float fHiThreshold, bool bRehashNow = false);

#ifdef _DEBUG
		void AssertValid() const;
#endif  // _DEBUG

		// Implementation
	private:
		Node**		ppBins_;
		size_t		iNumElements_;
		UINT		iNumBins_;
		float		fOptimalLoad_;
		float		fLoThreshold_;
		float		fHiThreshold_;
		size_t		iHiRehashThreshold_;
		size_t		iLoRehashThreshold_;
		ULONG		iLockCount_;
		UINT		iBlockSize_;
		AtlPlex*	pBlocks_;
		Node*		pFree_;

	private:
		bool IsLocked() const throw();
		UINT PickSize(size_t nElements) const throw();
		Node* NewNode(KINARGTYPE key, UINT iBin, UINT nHash);
		void FreeNode(Node* pNode);
		void FreePlexes() throw();
		Node* GetNode(KINARGTYPE key, UINT& iBin, UINT& nHash, Node*& pPrev) const throw();
		Node* GetNode(KINARGTYPE key) const throw();
		Node* CreateNode(KINARGTYPE key, UINT iBin, UINT nHash) throw(...);
		void RemoveNode(Node* pNode, Node* pPrev) throw();
		Node* FindNextNode(Node* pNode) const throw();
		void UpdateRehashThresholds() throw();

	public:
		~AtlMap() throw();

	private:
		// Private to prevent use
		AtlMap(const AtlMap&) throw();
		AtlMap& operator=(const AtlMap&) throw();
	};

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline size_t AtlMap<Key, Value, KTraits, VTraits>::GetCount() const throw()
	{
		return(iNumElements_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline bool AtlMap<Key, Value, KTraits, VTraits>::IsEmpty() const throw()
	{
		return(iNumElements_ == 0);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline Value& AtlMap<Key, Value, KTraits, VTraits>::operator[](KINARGTYPE key) throw(...)
	{
		Node* pNode;
		UINT iBin;
		UINT nHash;
		Node* pPrev;

		pNode = GetNode(key, iBin, nHash, pPrev);
		if(pNode == NULL)
		{
			pNode = CreateNode(key, iBin, nHash);
		}

		return(pNode->tValue_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline UINT AtlMap<Key, Value, KTraits, VTraits>::GetHashTableSize() const throw()
	{
		return(iNumBins_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline void AtlMap<Key, Value, KTraits, VTraits>::GetAt(Position pos, KOUTARGTYPE key, VOUTARGTYPE value) const
	{
		ASSERT(pos != NULL);

		Node* pNode = static_cast< Node* >(pos);

		key = pNode->tKey_;
		value = pNode->tValue_;
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline typename AtlMap<Key, Value, KTraits, VTraits>::Pair* AtlMap<Key, Value, KTraits, VTraits>::GetAt(Position pos) throw()
	{
		ASSERT(pos != NULL);

		return(static_cast< Pair* >(pos));
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline const typename AtlMap<Key, Value, KTraits, VTraits>::Pair* AtlMap<Key, Value, KTraits, VTraits>::GetAt(Position pos) const throw()
	{
		ASSERT(pos != NULL);

		return(static_cast< const Pair* >(pos));
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline const Key& AtlMap<Key, Value, KTraits, VTraits>::GetKeyAt(Position pos) const
	{
		ASSERT(pos != NULL);

		Node* pNode = (Node*)pos;

		return(pNode->tKey_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline const Value& AtlMap<Key, Value, KTraits, VTraits>::GetValueAt(Position pos) const
	{
		ASSERT(pos != NULL);

		Node* pNode = (Node*)pos;

		return(pNode->tValue_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline Value& AtlMap<Key, Value, KTraits, VTraits>::GetValueAt(Position pos)
	{
		ASSERT(pos != NULL);

		Node* pNode = (Node*)pos;

		return(pNode->tValue_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline void AtlMap<Key, Value, KTraits, VTraits>::DisableAutoRehash() throw()
	{
		iLockCount_++;
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline void AtlMap<Key, Value, KTraits, VTraits>::EnableAutoRehash() throw()
	{
		ASSUME(iLockCount_ > 0);
		iLockCount_--;
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	inline bool AtlMap<Key, Value, KTraits, VTraits>::IsLocked() const throw()
	{
		return(iLockCount_ != 0);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	UINT AtlMap<Key, Value, KTraits, VTraits>::PickSize(size_t nElements) const throw()
	{
		// List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
		static const UINT s_anPrimes[] =
		{
			17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821, 
			1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331, 
			13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033, 
			131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563, 
			832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319, 
			5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
		};

		size_t nBins = (size_t)(nElements/fOptimalLoad_);
		UINT nBinsEstimate = UINT( UINT_MAX < nBins ? UINT_MAX : nBins);

		// Find the smallest prime greater than our estimate
		int iPrime = 0;
		while(nBinsEstimate > s_anPrimes[iPrime])
		{
			iPrime++;
		}

		if(s_anPrimes[iPrime] == UINT_MAX)
		{
			return(nBinsEstimate);
		}
		else
		{
			return(s_anPrimes[iPrime]);
		}
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Node* AtlMap<Key, Value, KTraits, VTraits>::CreateNode(
		KINARGTYPE key, UINT iBin, UINT nHash) throw(...)
	{
		Node* pNode;

		if(ppBins_ == NULL)
		{
			bool bSuccess;

			bSuccess = InitHashTable(iNumBins_);
			if(!bSuccess)
			{
				throw(E_OUTOFMEMORY);
			}
		}

		pNode = NewNode(key, iBin, nHash);

		return(pNode);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	Position AtlMap<Key, Value, KTraits, VTraits>::GetStartPosition() const throw()
	{
		if(IsEmpty())
		{
			return(NULL);
		}

		for(UINT iBin = 0; iBin < iNumBins_; iBin++)
		{
			if(ppBins_[iBin] != NULL)
			{
				return(Position(ppBins_[iBin]));
			}
		}
		ASSERT(false);

		return(NULL);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	Position AtlMap<Key, Value, KTraits, VTraits>::SetAt(KINARGTYPE key, VINARGTYPE value)
	{
		Node* pNode;
		UINT iBin;
		UINT nHash;
		Node* pPrev;

		pNode = GetNode(key, iBin, nHash, pPrev);
		if(pNode == NULL)
		{
			pNode = CreateNode(key, iBin, nHash);
			try
			{
				pNode->tValue_ = value;
			}
			catch(...)
			{
				RemoveAtPos(Position(pNode));
				throw;
			}
		}
		else
		{
			pNode->tValue_ = value;
		}

		return(Position(pNode));
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::SetValueAt(Position pos, VINARGTYPE value)
	{
		ASSERT(pos != NULL);

		Node* pNode = static_cast< Node* >(pos);

		pNode->tValue_ = value;
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	AtlMap<Key, Value, KTraits, VTraits>::AtlMap(UINT nBins, float fOptimalLoad, float fLoThreshold, float fHiThreshold, UINT nBlockSize) throw() :
	ppBins_(NULL),
		iNumBins_(nBins),
		iNumElements_(0),
		iLockCount_(0),  // Start unlocked
		fOptimalLoad_(fOptimalLoad),
		fLoThreshold_(fLoThreshold),
		fHiThreshold_(fHiThreshold),
		iHiRehashThreshold_(UINT_MAX),
		iLoRehashThreshold_(0),
		pBlocks_(NULL),
		pFree_(NULL),
		iBlockSize_(nBlockSize)
	{
		ASSERT(nBins > 0);
		ASSERT(nBlockSize > 0);

		SetOptimalLoad(fOptimalLoad, fLoThreshold, fHiThreshold, false);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::SetOptimalLoad(float fOptimalLoad, float fLoThreshold, float fHiThreshold, bool bRehashNow)
	{
		ASSERT(fOptimalLoad > 0);
		ASSERT((fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad));
		ASSERT(fHiThreshold > fOptimalLoad);

		fOptimalLoad_ = fOptimalLoad;
		fLoThreshold_ = fLoThreshold;
		fHiThreshold_ = fHiThreshold;

		UpdateRehashThresholds();

		if(bRehashNow && ((iNumElements_ > iHiRehashThreshold_) || 
			(iNumElements_ < iLoRehashThreshold_)))
		{
			Rehash(PickSize(iNumElements_));
		}
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::UpdateRehashThresholds() throw()
	{
		iHiRehashThreshold_ = size_t(fHiThreshold_*iNumBins_);
		iLoRehashThreshold_ = size_t(fLoThreshold_*iNumBins_);
		if(iLoRehashThreshold_ < 17)
		{
			iLoRehashThreshold_ = 0;
		}
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	bool AtlMap<Key, Value, KTraits, VTraits>::InitHashTable(UINT nBins, bool bAllocNow)
	{
		ASSUME(iNumElements_ == 0);
		ASSERT(nBins > 0);

		if(ppBins_ != NULL)
		{
			delete[] ppBins_;
			ppBins_ = NULL;
		}

		if(bAllocNow)
		{
			ppBins_ = new Node*[nBins];
			if(ppBins_ == NULL)
			{
				return false;
			}

			ASSERT(UINT_MAX / sizeof(Node*) >= nBins);
			memset(ppBins_, 0, sizeof(Node*)*nBins);
		}
		iNumBins_ = nBins;

		UpdateRehashThresholds();

		return true;
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::RemoveAll()
	{
		DisableAutoRehash();
		if(ppBins_ != NULL)
		{
			for(UINT iBin = 0; iBin < iNumBins_; iBin++)
			{
				Node* pNext;

				pNext = ppBins_[iBin];
				while(pNext != NULL)
				{
					Node* pKill;

					pKill = pNext;
					pNext = pNext->pNext_;
					FreeNode(pKill);
				}
			}
		}

		delete[] ppBins_;
		ppBins_ = NULL;
		iNumElements_ = 0;

		if(!IsLocked())
		{
			InitHashTable(PickSize(iNumElements_), false);
		}

		FreePlexes();
		EnableAutoRehash();
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	AtlMap<Key, Value, KTraits, VTraits>::~AtlMap() throw()
	{
		try
		{
			RemoveAll();
		}
		catch(...)
		{
			ASSERT(false);		
		}
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Node* AtlMap<Key, Value, KTraits, VTraits>::NewNode(KINARGTYPE tKey, UINT iBin, UINT nHash)
	{
		Node* pNewNode;

		if(pFree_ == NULL)
		{
			AtlPlex* pPlex;
			Node* pNode;

			pPlex = AtlPlex::Create(pBlocks_, iBlockSize_, sizeof(Node));
			if(pPlex == NULL)
			{
				throw(E_OUTOFMEMORY);
			}
			pNode = (Node*)pPlex->GetData();
			pNode += iBlockSize_-1;
			for(int iBlock = iBlockSize_-1; iBlock >= 0; iBlock--)
			{
				pNode->pNext_ = pFree_;
				pFree_ = pNode;
				pNode--;
			}
		}
		ASSERT(pFree_ != NULL);
		pNewNode = pFree_;
		pFree_ = pNewNode->pNext_;

		try
		{
			::new(pNewNode) Node(tKey, nHash);
		}
		catch(...)
		{
			pNewNode->pNext_ = pFree_;
			pFree_ = pNewNode;

			throw;
		}
		iNumElements_++;

		pNewNode->pNext_ = ppBins_[iBin];
		ppBins_[iBin] = pNewNode;

		if((iNumElements_ > iHiRehashThreshold_) && !IsLocked())
		{
			Rehash(PickSize(iNumElements_));
		}

		return(pNewNode);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::FreeNode(Node* pNode)
	{
		ASSERT(pNode != NULL);

		pNode->~Node();
		pNode->pNext_ = pFree_;
		pFree_ = pNode;

		ASSUME(iNumElements_ > 0);
		iNumElements_--;

		if((iNumElements_ < iLoRehashThreshold_) && !IsLocked())
		{
			Rehash(PickSize(iNumElements_));
		}

		if(iNumElements_ == 0)
		{
			FreePlexes();
		}
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::FreePlexes() throw()
	{
		pFree_ = NULL;
		if(pBlocks_ != NULL)
		{
			pBlocks_->FreeDataChain();
			pBlocks_ = NULL;
		}
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Node* AtlMap<Key, Value, KTraits, VTraits>::GetNode(KINARGTYPE tKey, UINT& iBin, UINT& rHash, Node*& pPrev) const throw()
	{
		Node* pFollow;

		rHash = KTraits::Hash(tKey);
		iBin = rHash%iNumBins_;

		if(ppBins_ == NULL)
		{
			return(NULL);
		}

		pFollow = NULL;
		pPrev = NULL;
		for(Node* pNode = ppBins_[iBin]; pNode != NULL; pNode = pNode->pNext_)
		{
			if((pNode->GetHash() == rHash) && KTraits::CompareElements(pNode->tKey_, tKey))
			{
				pPrev = pFollow;
				return(pNode);
			}
			pFollow = pNode;
		}

		return(NULL);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Node* AtlMap<Key, Value, KTraits, VTraits>::GetNode(KINARGTYPE tKey) const throw()
	{
		UINT	iHash = KTraits::Hash(tKey);
		UINT	iBin = iHash%iNumBins_;

		if(ppBins_ == NULL)
		{
			return(NULL);
		}

		for(Node* pNode = ppBins_[iBin]; pNode != NULL; pNode = pNode->pNext_)
		{
			if((pNode->GetHash() == iHash) && KTraits::CompareElements(pNode->tKey_, tKey))
			{
				return(pNode);
			}
		}

		return(NULL);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	bool AtlMap<Key, Value, KTraits, VTraits>::Find(KINARGTYPE key, VOUTARGTYPE value) const
	{
		Node* pNode = GetNode(key);
		if(pNode == NULL)
		{
			return(false);
		}

		value = pNode->tValue_;

		return(true);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	const typename AtlMap<Key, Value, KTraits, VTraits>::Pair* AtlMap<Key, Value, KTraits, VTraits>::Find(KINARGTYPE key) const throw()
	{
		return GetNode(key);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Pair* AtlMap<Key, Value, KTraits, VTraits>::Find(KINARGTYPE key) throw()
	{
		return GetNode(key);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	bool AtlMap<Key, Value, KTraits, VTraits>::RemoveKey(KINARGTYPE tKey) throw()
	{
		Node* pNode;
		UINT iBin;
		UINT nHash;
		Node* pPrev;

		pPrev = NULL;
		pNode = GetNode(tKey, iBin, nHash, pPrev);
		if(pNode == NULL)
		{
			return(false);
		}

		RemoveNode(pNode, pPrev);

		return(true);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::RemoveNode(Node* pNode, Node* pPrev)
	{
		ASSERT(pNode != NULL);

		UINT iBin = pNode->GetHash() % iNumBins_;

		if(pPrev == NULL)
		{
			ASSUME(ppBins_[iBin] == pNode);
			ppBins_[iBin] = pNode->pNext_;
		}
		else
		{
			ASSERT(pPrev->pNext_ == pNode);
			pPrev->pNext_ = pNode->pNext_;
		}
		FreeNode(pNode);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::RemoveAtPos(Position pos)
	{
		ASSERT(pos != NULL);

		Node* pNode = static_cast< Node* >(pos);
		Node* pPrev = NULL;
		UINT iBin = pNode->GetHash() % iNumBins_;

		ASSUME(ppBins_[iBin] != NULL);
		if(pNode == ppBins_[iBin])
		{
			pPrev = NULL;
		}
		else
		{
			pPrev = ppBins_[iBin];
			while(pPrev->pNext_ != pNode)
			{
				pPrev = pPrev->pNext_;
				ASSERT(pPrev != NULL);
			}
		}
		RemoveNode(pNode, pPrev);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::Rehash(UINT nBins)
	{
		Node** ppBins = NULL;

		if(nBins == 0)
		{
			nBins = PickSize(iNumElements_);
		}

		if(nBins == iNumBins_)
		{
			return;
		}

		//LOG(_W("Rehash: %u bins\n"), nBins);

		if(ppBins_ == NULL)
		{
			// Just set the new number of bins
			InitHashTable(nBins, false);
			return;
		}

		ppBins = new Node*[nBins];
		if (ppBins == NULL)
		{
			throw(E_OUTOFMEMORY);
		}

		ASSERT(UINT_MAX / sizeof(Node*) >= nBins);
		memset(ppBins, 0, nBins*sizeof(Node*));

		// Nothing gets copied.  We just rewire the old nodes
		// into the new bins.
		for(UINT iSrcBin = 0; iSrcBin < iNumBins_; iSrcBin++)
		{
			Node* pNode;

			pNode = ppBins_[iSrcBin];
			while(pNode != NULL)
			{
				Node* pNext;
				UINT iDestBin;

				pNext = pNode->pNext_;  // Save so we don't trash it
				iDestBin = pNode->GetHash()%nBins;
				pNode->pNext_ = ppBins[iDestBin];
				ppBins[iDestBin] = pNode;

				pNode = pNext;
			}
		}

		delete[] ppBins_;
		ppBins_ = ppBins;
		iNumBins_ = nBins;

		UpdateRehashThresholds();
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::GetNextAssoc(Position& pos, KOUTARGTYPE key, VOUTARGTYPE value) const
	{
		Node* pNode;
		Node* pNext;

		ASSUME(ppBins_ != NULL);
		ASSERT(pos != NULL);

		pNode = (Node*)pos;
		pNext = FindNextNode(pNode);

		pos = Position(pNext);
		key = pNode->tKey_;
		value = pNode->tValue_;
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	const typename AtlMap<Key, Value, KTraits, VTraits>::Pair* AtlMap<Key, Value, KTraits, VTraits>::GetNext(Position& pos) const throw()
	{
		Node* pNode;
		Node* pNext;

		ASSUME(ppBins_ != NULL);
		ASSERT(pos != NULL);

		pNode = (Node*)pos;
		pNext = FindNextNode(pNode);

		pos = Position(pNext);

		return(pNode);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Pair* AtlMap<Key, Value, KTraits, VTraits>::GetNext(Position& pos) throw()
	{
		ASSUME(ppBins_ != NULL);
		ASSERT(pos != NULL);

		Node* pNode = static_cast< Node* >(pos);
		Node* pNext = FindNextNode(pNode);

		pos = Position(pNext);

		return(pNode);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	const Key& AtlMap<Key, Value, KTraits, VTraits>::GetNextKey(Position& pos) const
	{
		Node* pNode;
		Node* pNext;

		ASSUME(ppBins_ != NULL);
		ASSERT(pos != NULL);

		pNode = (Node*)pos;
		pNext = FindNextNode(pNode);

		pos = Position(pNext);

		return(pNode->tKey_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	const Value& AtlMap<Key, Value, KTraits, VTraits>::GetNextValue(Position& pos) const
	{
		Node* pNode;
		Node* pNext;

		ASSUME(ppBins_ != NULL);
		ASSERT(pos != NULL);

		pNode = (Node*)pos;
		pNext = FindNextNode(pNode);

		pos = Position(pNext);

		return(pNode->tValue_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	Value& AtlMap<Key, Value, KTraits, VTraits>::GetNextValue(Position& pos)
	{
		Node* pNode;
		Node* pNext;

		ASSUME(ppBins_ != NULL);
		ASSERT(pos != NULL);

		pNode = (Node*)pos;
		pNext = FindNextNode(pNode);

		pos = Position(pNext);

		return(pNode->tValue_);
	}

	template<typename Key, typename Value, class KTraits, class VTraits>
	typename AtlMap<Key, Value, KTraits, VTraits>::Node* AtlMap<Key, Value, KTraits, VTraits>::FindNextNode(Node* pNode) const throw()
	{
		Node* pNext;

		if(pNode == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		if(pNode->pNext_ != NULL)
		{
			pNext = pNode->pNext_;
		}
		else
		{
			UINT iBin;

			pNext = NULL;
			iBin = (pNode->GetHash()%iNumBins_)+1;
			while((pNext == NULL) && (iBin < iNumBins_))
			{
				if(ppBins_[iBin] != NULL)
				{
					pNext = ppBins_[iBin];
				}

				iBin++;
			}
		}

		return(pNext);
	}

#ifdef _DEBUG
	template<typename Key, typename Value, class KTraits, class VTraits>
	void AtlMap<Key, Value, KTraits, VTraits>::AssertValid() const
	{
		ASSUME(iNumBins_ > 0);
		// non-empty map should have hash table
		ASSERT(IsEmpty() || (ppBins_ != NULL));
	}
#endif

	template< typename Key, typename Value, class KTraits = CElementTraits< Key >, class VTraits = CElementTraits< Value > >
	class RBTree
	{
	public:
		typedef typename KTraits::INARGTYPE KINARGTYPE;
		typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
		typedef typename VTraits::INARGTYPE VINARGTYPE;
		typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

	public:
		class Pair : public __Position
		{
		protected:

			Pair(KINARGTYPE tKey, VINARGTYPE value) : tKey_(tKey), tValue_(value)
			{
			}
			~Pair() throw()
			{
			}

		public:
			Key tKey_;
			Value tValue_;
		};

	private:

		class Node : public Pair
		{
		public:
			enum RB_COLOR
			{
				RB_RED, 
				RB_BLACK
			};

		public:
			RB_COLOR iColor_;
			Node* pLeft_;
			Node* pRight_;
			Node* pParent_;

			Node(KINARGTYPE tKey, VINARGTYPE value) : 
			Pair(tKey, value),
				pParent_(NULL),
				iColor_(RB_BLACK)
			{
			}
			~Node() throw()
			{
			}
		};

	private:
		Node* pRoot_;
		size_t iCount_;
		Node* pFree_;
		AtlPlex* pBlocks_;
		size_t iBlockSize_;

		// sentinel node
		Node *pNil_;

		// methods
		bool IsNil(Node *p) const throw();
		void SetNil(Node **p) throw();

		Node* NewNode(KINARGTYPE tKey, VINARGTYPE value) throw(...);
		void FreeNode(Node* pNode) throw();
		void RemovePostOrder(Node* pNode) throw();
		Node* LeftRotate(Node* pNode) throw();
		Node* RightRotate(Node* pNode) throw();
		void SwapNode(Node* pDest, Node* pSrc) throw();
		Node* InsertImpl(KINARGTYPE tKey, VINARGTYPE value) throw(...);
		void RBDeleteFixup(Node* pNode) throw();
		bool RBDelete(Node* pZ) throw();

#ifdef _DEBUG
		// internal debugging code to verify red-black properties of tree:
		// 1) Every node is either red or black
		// 2) Every leaf (NIL) is black
		// 3) If a node is red, both its children are black
		// 4) Every simple path from a node to a descendant leaf node contains 
		//    the same number of black nodes
	private:
		void VerifyIntegrity(const Node *pNode, int nCurrBlackDepth, int &nBlackDepth) const throw();

	public:
		void VerifyIntegrity() const throw();
#endif // _DEBUG

	protected:
		Node* Minimum(Node* pNode) const throw();
		Node* Maximum(Node* pNode) const throw();
		Node* Predecessor(Node* pNode) const throw();
		Node* Successor(Node* pNode) const throw();
		Node* RBInsert(KINARGTYPE tKey, VINARGTYPE value) throw(...);
		Node* _Find(KINARGTYPE tKey) const throw();
		Node* FindPrefix(KINARGTYPE tKey) const throw();

	protected:
		explicit RBTree(size_t nBlockSize = 10) throw();  // protected to prevent instantiation

	public:
		~RBTree() throw();

		void RemoveAll() throw();
		void RemoveAt(Position pos) throw();

		size_t GetCount() const throw();
		bool IsEmpty() const throw();

		Position FindFirstKeyAfter(KINARGTYPE tKey) const throw();

		Position GetHeadPosition() const throw();
		Position GetTailPosition() const throw();
		void GetNextAssoc(Position& pos, KOUTARGTYPE tKey, VOUTARGTYPE value) const;
		const Pair* GetNext(Position& pos) const throw();
		Pair* GetNext(Position& pos) throw();
		const Pair* GetPrev(Position& pos) const throw();
		Pair* GetPrev(Position& pos) throw();
		const Key& GetNextKey(Position& pos) const throw();
		const Value& GetNextValue(Position& pos) const throw();
		Value& GetNextValue(Position& pos) throw();

		Pair* GetAt(Position pos) throw();
		const Pair* GetAt(Position pos) const throw();
		void GetAt(Position pos, KOUTARGTYPE tKey, VOUTARGTYPE value) const;
		const Key& GetKeyAt(Position pos) const;
		const Value& GetValueAt(Position pos) const;
		Value& GetValueAt(Position pos);
		void SetValueAt(Position pos, VINARGTYPE value);

	private:
		// Private to prevent use
		RBTree(const RBTree&) throw();
		RBTree& operator=(const RBTree&) throw();
	};

	template< typename Key, typename Value, class KTraits, class VTraits >
	inline bool RBTree< Key, Value, KTraits, VTraits >::IsNil(Node *p) const throw()
	{
		return (p == pNil_);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	inline void RBTree< Key, Value, KTraits, VTraits >::SetNil(Node **p)
	{
		ASSERT(p != NULL);
		*p = pNil_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	RBTree< Key, Value, KTraits, VTraits >::RBTree(size_t nBlockSize) throw() :
	pRoot_(NULL),
		iCount_(0),
		iBlockSize_(nBlockSize),
		pFree_(NULL),
		pBlocks_(NULL),
		pNil_(NULL)
	{
		ASSERT(nBlockSize > 0);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	RBTree< Key, Value, KTraits, VTraits >::~RBTree() throw()
	{
		RemoveAll();
		if (pNil_ != NULL)
		{
			free(pNil_);
		}
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::RemoveAll() throw()
	{
		if (!IsNil(pRoot_))
			RemovePostOrder(pRoot_);
		iCount_ = 0;
		pBlocks_->FreeDataChain();
		pBlocks_ = NULL;
		pFree_ = NULL;
		pRoot_ = pNil_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	size_t RBTree< Key, Value, KTraits, VTraits >::GetCount() const throw()
	{
		return iCount_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	bool RBTree< Key, Value, KTraits, VTraits >::IsEmpty() const throw()
	{
		return(iCount_ == 0);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	Position RBTree< Key, Value, KTraits, VTraits >::FindFirstKeyAfter(KINARGTYPE tKey) const throw()
	{
		return(FindPrefix(tKey));
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::RemoveAt(Position pos) throw()
	{
		ASSERT(pos != NULL);
		RBDelete(static_cast<Node*>(pos));
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	Position RBTree< Key, Value, KTraits, VTraits >::GetHeadPosition() const throw()
	{
		return(Minimum(pRoot_));
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	Position RBTree< Key, Value, KTraits, VTraits >::GetTailPosition() const throw()
	{
		return(Maximum(pRoot_));
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::GetNextAssoc(Position& pos, KOUTARGTYPE tKey, VOUTARGTYPE value) const
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast< Node* >(pos);

		tKey = pNode->tKey_;
		value = pNode->tValue_;

		pos = Successor(pNode);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const typename RBTree< Key, Value, KTraits, VTraits >::Pair* RBTree< Key, Value, KTraits, VTraits >::GetNext(Position& pos) const throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast< Node* >(pos);
		pos = Successor(pNode);
		return pNode;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Pair* RBTree< Key, Value, KTraits, VTraits >::GetNext(Position& pos) throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast< Node* >(pos);
		pos = Successor(pNode);
		return pNode;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const typename RBTree< Key, Value, KTraits, VTraits >::Pair* RBTree< Key, Value, KTraits, VTraits >::GetPrev(Position& pos) const throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast< Node* >(pos);
		pos = Predecessor(pNode);

		return pNode;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Pair* RBTree< Key, Value, KTraits, VTraits >::GetPrev(Position& pos) throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast< Node* >(pos);
		pos = Predecessor(pNode);

		return pNode;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const Key& RBTree< Key, Value, KTraits, VTraits >::GetNextKey(Position& pos) const throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast<Node*>(pos);
		pos = Successor(pNode);

		return pNode->tKey_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const Value& RBTree< Key, Value, KTraits, VTraits >::GetNextValue(Position& pos) const throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast<Node*>(pos);
		pos = Successor(pNode);

		return pNode->tValue_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	Value& RBTree< Key, Value, KTraits, VTraits >::GetNextValue(Position& pos) throw()
	{
		ASSERT(pos != NULL);
		Node* pNode = static_cast<Node*>(pos);
		pos = Successor(pNode);

		return pNode->tValue_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Pair* RBTree< Key, Value, KTraits, VTraits >::GetAt(Position pos) throw()
	{
		ASSERT(pos != NULL);

		return(static_cast< Pair* >(pos));
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const typename RBTree< Key, Value, KTraits, VTraits >::Pair* RBTree< Key, Value, KTraits, VTraits >::GetAt(Position pos) const throw()
	{
		ASSERT(pos != NULL);

		return(static_cast< const Pair* >(pos));
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::GetAt(Position pos, KOUTARGTYPE tKey, VOUTARGTYPE value) const
	{
		ASSERT(pos != NULL);
		tKey = static_cast<Node*>(pos)->tKey_;
		value = static_cast<Node*>(pos)->tValue_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const Key& RBTree< Key, Value, KTraits, VTraits >::GetKeyAt(Position pos) const
	{
		ASSERT(pos != NULL);
		return static_cast<Node*>(pos)->tKey_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const Value& RBTree< Key, Value, KTraits, VTraits >::GetValueAt(Position pos) const
	{
		ASSERT(pos != NULL);
		return static_cast<Node*>(pos)->tValue_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	Value& RBTree< Key, Value, KTraits, VTraits >::GetValueAt(Position pos)
	{
		ASSERT(pos != NULL);
		return static_cast<Node*>(pos)->tValue_;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::SetValueAt(Position pos, VINARGTYPE value)
	{
		ASSERT(pos != NULL);
		static_cast<Node*>(pos)->tValue_ = value;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::NewNode(KINARGTYPE tKey, VINARGTYPE value) throw(...)
	{
		if(pFree_ == NULL)
		{
			if (pNil_ == NULL)
			{
				pNil_ = reinterpret_cast<Node *>(malloc(sizeof(Node)));
				if (pNil_ == NULL)
				{
					throw(E_OUTOFMEMORY);
				}
				memset(pNil_, 0x00, sizeof(Node));
				pNil_->iColor_ = Node::RB_BLACK;
				pNil_->pParent_ = pNil_->pLeft_ = pNil_->pRight_ = pNil_;
				pRoot_ = pNil_;
			}

			AtlPlex* pPlex = AtlPlex::Create(pBlocks_, iBlockSize_, sizeof(Node));
			if(pPlex == NULL)
			{
				throw(E_OUTOFMEMORY);
			}
			Node* pNode = static_cast< Node* >(pPlex->GetData());
			pNode += iBlockSize_-1;
			for(INT_PTR iBlock = iBlockSize_-1; iBlock >= 0; iBlock--)
			{
				pNode->pLeft_ = pFree_;
				pFree_ = pNode;
				pNode--;
			}
		}
		ASSUME(pFree_ != NULL);

		Node* pNewNode = pFree_;
		::new(pNewNode) Node(tKey, value);

		pFree_ = pFree_->pLeft_;
		pNewNode->iColor_ = Node::RB_RED;
		SetNil(&pNewNode->pLeft_);
		SetNil(&pNewNode->pRight_);
		SetNil(&pNewNode->pParent_);

		iCount_++;
		ASSUME(iCount_ > 0);

		return(pNewNode);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::FreeNode(Node* pNode)
	{
		ASSERT(pNode != NULL);
		pNode->~Node();
		pNode->pLeft_ = pFree_;
		pFree_ = pNode;
		ASSUME(iCount_ > 0);
		iCount_--;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::RemovePostOrder(Node* pNode) throw()
	{
		if (IsNil(pNode))
			return;
		RemovePostOrder(pNode->pLeft_);
		RemovePostOrder(pNode->pRight_);
		FreeNode(pNode);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::LeftRotate(Node* pNode) throw()
	{
		ASSERT(pNode != NULL);
		if(pNode == NULL)
			return NULL;

		Node* pRight = pNode->pRight_;
		pNode->pRight_ = pRight->pLeft_;
		if (!IsNil(pRight->pLeft_))
			pRight->pLeft_->pParent_ = pNode;

		pRight->pParent_ = pNode->pParent_;
		if (IsNil(pNode->pParent_))
			pRoot_ = pRight;
		else if (pNode == pNode->pParent_->pLeft_)
			pNode->pParent_->pLeft_ = pRight;
		else 
			pNode->pParent_->pRight_ = pRight;

		pRight->pLeft_ = pNode;
		pNode->pParent_ = pRight;
		return pNode;

	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::RightRotate(Node* pNode) throw()
	{
		ASSERT(pNode != NULL);
		if(pNode == NULL)
			return NULL;

		Node* pLeft = pNode->pLeft_;
		pNode->pLeft_ = pLeft->pRight_;
		if (!IsNil(pLeft->pRight_))
			pLeft->pRight_->pParent_ = pNode;

		pLeft->pParent_ = pNode->pParent_;
		if (IsNil(pNode->pParent_))
			pRoot_ = pLeft;
		else if (pNode == pNode->pParent_->pRight_)
			pNode->pParent_->pRight_ = pLeft;
		else
			pNode->pParent_->pLeft_ = pLeft;

		pLeft->pRight_ = pNode;
		pNode->pParent_ = pLeft;
		return pNode;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::_Find(KINARGTYPE tKey) const throw()
	{
		Node* pKey = NULL;
		Node* pNode = pRoot_;
		while(!IsNil(pNode) && (pKey == NULL))
		{
			int nCompare = KTraits::CompareElementsOrdered(tKey, pNode->tKey_);
			if(nCompare == 0)
			{
				pKey = pNode;
			}
			else
			{
				if(nCompare < 0)
				{
					pNode = pNode->pLeft_;
				}
				else
				{
					pNode = pNode->pRight_;
				}
			}
		}

		if(pKey == NULL)
		{
			return(NULL);
		}

#pragma warning(push)
#pragma warning(disable:4127)

		while(true)
		{
			Node* pPrev = Predecessor(pKey);
			if((pPrev != NULL) && KTraits::CompareElements(tKey, pPrev->tKey_))
			{
				pKey = pPrev;
			}
			else
			{
				return(pKey);
			}
		}

#pragma warning(pop)
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::FindPrefix(KINARGTYPE tKey) const throw()
	{
		// First, attempt to find a node that matches the key exactly
		Node* pParent = NULL;
		Node* pKey = NULL;
		Node* pNode = pRoot_;
		while(!IsNil(pNode) && (pKey == NULL))
		{
			pParent = pNode;
			int nCompare = KTraits::CompareElementsOrdered(tKey, pNode->tKey_);
			if(nCompare == 0)
			{
				pKey = pNode;
			}
			else if(nCompare < 0)
			{
				pNode = pNode->pLeft_;
			}
			else
			{
				pNode = pNode->pRight_;
			}
		}

		if(pKey != NULL)
		{
			// We found a node with the exact key, so find the first node after 
			// this one with a different key 
			while(true)
			{
				Node* pNext = Successor(pKey);
				if ((pNext != NULL) && KTraits::CompareElements(tKey, pNext->tKey_))
				{
					pKey = pNext;
				}
				else
				{
					return pNext;
				}
			}
		}
		else if (pParent != NULL)
		{
			// No node matched the key exactly, so pick the first node with 
			// a key greater than the given key
			int nCompare = KTraits::CompareElementsOrdered(tKey, pParent->tKey_);
			if(nCompare < 0)
			{
				pKey = pParent;
			}
			else
			{
				ASSERT(nCompare > 0);
				pKey = Successor(pParent);
			}
		}

		return(pKey);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::SwapNode(Node* pDest, Node* pSrc)
	{
		ASSERT(pDest != NULL);
		ASSERT(pSrc != NULL);

		pDest->pParent_ = pSrc->pParent_;
		if (pSrc->pParent_->pLeft_ == pSrc)
		{
			pSrc->pParent_->pLeft_ = pDest;
		}
		else
		{
			pSrc->pParent_->pRight_ = pDest;
		}

		pDest->pRight_ = pSrc->pRight_;
		pDest->pLeft_ = pSrc->pLeft_;
		pDest->iColor_ = pSrc->iColor_;
		pDest->pRight_->pParent_ = pDest;
		pDest->pLeft_->pParent_ = pDest;

		if (pRoot_ == pSrc)
		{
			pRoot_ = pDest;
		}
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::InsertImpl(KINARGTYPE tKey, VINARGTYPE value) throw(...)
	{
		Node* pNew = NewNode(tKey, value);

		Node* pY = NULL;
		Node* pX = pRoot_;

		while (!IsNil(pX))
		{
			pY = pX;
			if(KTraits::CompareElementsOrdered(tKey, pX->tKey_) <= 0)
				pX = pX->pLeft_;
			else
				pX = pX->pRight_;
		}

		pNew->pParent_ = pY;
		if (pY == NULL)
		{
			pRoot_ = pNew;
		}
		else if(KTraits::CompareElementsOrdered(tKey, pY->tKey_) <= 0)
			pY->pLeft_ = pNew;
		else
			pY->pRight_ = pNew;

		return pNew;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::RBDeleteFixup(Node* pNode)
	{
		ASSERT(pNode != NULL);

		Node* pX = pNode;
		Node* pW = NULL;

		while ((pX != pRoot_) && (pX->iColor_ == Node::RB_BLACK))
		{
			if (pX == pX->pParent_->pLeft_)
			{
				pW = pX->pParent_->pRight_;
				if (pW->iColor_ == Node::RB_RED)
				{
					pW->iColor_ = Node::RB_BLACK;
					pW->pParent_->iColor_ = Node::RB_RED;
					LeftRotate(pX->pParent_);
					pW = pX->pParent_->pRight_;
				}
				if (pW->pLeft_->iColor_ == Node::RB_BLACK && pW->pRight_->iColor_ == Node::RB_BLACK)
				{
					pW->iColor_ = Node::RB_RED;
					pX = pX->pParent_;
				}
				else
				{
					if (pW->pRight_->iColor_ == Node::RB_BLACK)
					{
						pW->pLeft_->iColor_ = Node::RB_BLACK;
						pW->iColor_ = Node::RB_RED;
						RightRotate(pW);
						pW = pX->pParent_->pRight_;
					}
					pW->iColor_ = pX->pParent_->iColor_;
					pX->pParent_->iColor_ = Node::RB_BLACK;
					pW->pRight_->iColor_ = Node::RB_BLACK;
					LeftRotate(pX->pParent_);
					pX = pRoot_;
				}
			}
			else
			{
				pW = pX->pParent_->pLeft_;
				if (pW->iColor_ == Node::RB_RED)
				{
					pW->iColor_ = Node::RB_BLACK;
					pW->pParent_->iColor_ = Node::RB_RED;
					RightRotate(pX->pParent_);
					pW = pX->pParent_->pLeft_;
				}
				if (pW->pRight_->iColor_ == Node::RB_BLACK && pW->pLeft_->iColor_ == Node::RB_BLACK)
				{
					pW->iColor_ = Node::RB_RED;
					pX = pX->pParent_;
				}
				else
				{
					if (pW->pLeft_->iColor_ == Node::RB_BLACK)
					{
						pW->pRight_->iColor_ = Node::RB_BLACK;
						pW->iColor_ = Node::RB_RED;
						LeftRotate(pW);
						pW = pX->pParent_->pLeft_;
					}
					pW->iColor_ = pX->pParent_->iColor_;
					pX->pParent_->iColor_ = Node::RB_BLACK;
					pW->pLeft_->iColor_ = Node::RB_BLACK;
					RightRotate(pX->pParent_);
					pX = pRoot_;
				}
			}
		}

		pX->iColor_ = Node::RB_BLACK;
	}


	template< typename Key, typename Value, class KTraits, class VTraits >
	bool RBTree< Key, Value, KTraits, VTraits >::RBDelete(Node* pZ) throw()
	{
		if (pZ == NULL)
			return false;

		Node* pY = NULL;
		Node* pX = NULL;
		if (IsNil(pZ->pLeft_) || IsNil(pZ->pRight_))
			pY = pZ;
		else
			pY = Successor(pZ);

		if (!IsNil(pY->pLeft_))
			pX = pY->pLeft_;
		else
			pX = pY->pRight_;

		pX->pParent_ = pY->pParent_;

		if (IsNil(pY->pParent_))
			pRoot_ = pX;
		else if (pY == pY->pParent_->pLeft_)
			pY->pParent_->pLeft_ = pX;
		else
			pY->pParent_->pRight_ = pX;

		if (pY->iColor_ == Node::RB_BLACK)
			RBDeleteFixup(pX);

		if (pY != pZ)
			SwapNode(pY, pZ);

		if (pRoot_ != NULL)
			SetNil(&pRoot_->pParent_);

		FreeNode(pZ);

		return true;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::Minimum(Node* pNode) const throw()
	{
		if (pNode == NULL || IsNil(pNode))
		{
			return NULL;
		}

		Node* pMin = pNode;
		while (!IsNil(pMin->pLeft_))
		{
			pMin = pMin->pLeft_;
		}

		return pMin;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::Maximum(Node* pNode) const throw()
	{
		if (pNode == NULL || IsNil(pNode))
		{
			return NULL;
		}

		Node* pMax = pNode;
		while (!IsNil(pMax->pRight_))
		{
			pMax = pMax->pRight_;
		}

		return pMax;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::Predecessor(Node* pNode) const throw()
	{
		if(pNode == NULL)
		{
			return(NULL);
		}
		if(!IsNil(pNode->pLeft_))
		{
			return(Maximum(pNode->pLeft_));
		}

		Node* pParent = pNode->pParent_;
		Node* pLeft = pNode;
		while(!IsNil(pParent) && (pLeft == pParent->pLeft_))
		{
			pLeft = pParent;
			pParent = pParent->pParent_;
		}

		if (IsNil(pParent))
		{
			pParent = NULL;
		}
		return(pParent);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::Successor(Node* pNode) const throw()
	{
		if (pNode == NULL)
		{
			return NULL;
		}
		if (!IsNil(pNode->pRight_))
		{
			return Minimum(pNode->pRight_);
		}

		Node* pParent = pNode->pParent_;
		Node* pRight = pNode;
		while (!IsNil(pParent) && (pRight == pParent->pRight_))
		{
			pRight = pParent;
			pParent = pParent->pParent_;
		}

		if (IsNil(pParent))
		{
			pParent = NULL;
		}
		return pParent;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBTree< Key, Value, KTraits, VTraits >::Node* RBTree< Key, Value, KTraits, VTraits >::RBInsert(KINARGTYPE tKey, VINARGTYPE value) throw(...)
	{
		Node* pNewNode = InsertImpl(tKey, value);

		Node* pX = pNewNode;
		pX->iColor_ = Node::RB_RED;
		Node* pY = NULL;
		while (pX != pRoot_ && pX->pParent_->iColor_ == Node::RB_RED)
		{
			if (pX->pParent_ == pX->pParent_->pParent_->pLeft_)
			{
				pY = pX->pParent_->pParent_->pRight_;
				if (pY != NULL && pY->iColor_ == Node::RB_RED)
				{
					pX->pParent_->iColor_ = Node::RB_BLACK;
					pY->iColor_ = Node::RB_BLACK;
					pX->pParent_->pParent_->iColor_ = Node::RB_RED;
					pX = pX->pParent_->pParent_;
				}
				else
				{
					if (pX == pX->pParent_->pRight_)
					{
						pX = pX->pParent_;
						LeftRotate(pX);
					}
					pX->pParent_->iColor_ = Node::RB_BLACK;
					pX->pParent_->pParent_->iColor_ = Node::RB_RED;
					RightRotate(pX->pParent_->pParent_);
				}
			}
			else
			{
				pY = pX->pParent_->pParent_->pLeft_;
				if (pY != NULL && pY->iColor_ == Node::RB_RED)
				{
					pX->pParent_->iColor_ = Node::RB_BLACK;
					pY->iColor_ = Node::RB_BLACK;
					pX->pParent_->pParent_->iColor_ = Node::RB_RED;
					pX = pX->pParent_->pParent_;
				}
				else
				{
					if (pX == pX->pParent_->pLeft_)
					{
						pX = pX->pParent_;
						RightRotate(pX);
					}
					pX->pParent_->iColor_ = Node::RB_BLACK;
					pX->pParent_->pParent_->iColor_ = Node::RB_RED;
					LeftRotate(pX->pParent_->pParent_);
				}
			}
		}

		pRoot_->iColor_ = Node::RB_BLACK;
		SetNil(&pRoot_->pParent_);

		return(pNewNode);
	}

#ifdef _DEBUG
	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::VerifyIntegrity(const Node *pNode, int nCurrBlackDepth, int &nBlackDepth) const throw()
	{
		bool bCheckForBlack = false;
		bool bLeaf = true;

		if (pNode->iColor_ == Node::RB_RED) 
			bCheckForBlack = true;
		else
			nCurrBlackDepth++;

		ASSERT(pNode->pLeft_ != NULL);
		if (!IsNil(pNode->pLeft_))
		{
			bLeaf = false;
			if (bCheckForBlack)
			{
				ASSERT(pNode->pLeft_->iColor_ == Node::RB_BLACK);
			}

			VerifyIntegrity(pNode->pLeft_, nCurrBlackDepth, nBlackDepth);
		}

		ASSERT(pNode->pRight_ != NULL);
		if (!IsNil(pNode->pRight_))
		{
			bLeaf = false;
			if (bCheckForBlack)
			{
				ASSERT(pNode->pRight_->iColor_ == Node::RB_BLACK);
			}

			VerifyIntegrity(pNode->pRight_, nCurrBlackDepth, nBlackDepth);
		}

		ASSERT(pNode->pParent_ != NULL);
		ASSERT((IsNil(pNode->pParent_)) ||
			(pNode->pParent_->pLeft_ == pNode) ||
			(pNode->pParent_->pRight_ == pNode));

		if (bLeaf) 
		{
			if (nBlackDepth == 0)
			{
				nBlackDepth = nCurrBlackDepth;
			}
			else 
			{
				ASSERT(nBlackDepth == nCurrBlackDepth);
			}
		}
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	void RBTree< Key, Value, KTraits, VTraits >::VerifyIntegrity() const throw()
	{
		if ((pRoot_ == NULL) || (IsNil(pRoot_)))
			return;

		ASSUME(pRoot_->iColor_ == Node::RB_BLACK);
		int nBlackDepth = 0;
		VerifyIntegrity(pRoot_, 0, nBlackDepth);
	}
#endif // _DEBUG

	template< typename Key, typename Value, class KTraits = ElementTraits< Key >, class VTraits = ElementTraits< Value > >
	class RBMap :
		public RBTree< Key, Value, KTraits, VTraits >
	{
	public:
		explicit RBMap(size_t nBlockSize = 10) throw();
		~RBMap() throw();

		bool Find(KINARGTYPE tKey, VOUTARGTYPE value) const throw(...);
		const Pair* Find(KINARGTYPE tKey) const throw();
		Pair* Find(KINARGTYPE tKey) throw();
		Position SetAt(KINARGTYPE tKey, VINARGTYPE value) throw(...);
		bool RemoveKey(KINARGTYPE tKey) throw();
	};

	template< typename Key, typename Value, class KTraits, class VTraits >
	RBMap< Key, Value, KTraits, VTraits >::RBMap(size_t nBlockSize) throw() :
	RBTree< Key, Value, KTraits, VTraits >(nBlockSize)
	{
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	RBMap< Key, Value, KTraits, VTraits >::~RBMap() throw()
	{
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	const typename RBMap< Key, Value, KTraits, VTraits >::Pair* RBMap< Key, Value, KTraits, VTraits >::Find(KINARGTYPE tKey) const throw()
	{
		return _Find(tKey);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	typename RBMap< Key, Value, KTraits, VTraits >::Pair* RBMap< Key, Value, KTraits, VTraits >::Find(KINARGTYPE tKey) throw()
	{
		return _Find(tKey);
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	bool RBMap< Key, Value, KTraits, VTraits >::Find(KINARGTYPE tKey, VOUTARGTYPE value) const throw(...)
	{
		const Pair* pLookup = _Find(tKey);
		if(pLookup == NULL)
			return false;

		value = pLookup->tValue_;
		return true;
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	Position RBMap< Key, Value, KTraits, VTraits >::SetAt(KINARGTYPE tKey, VINARGTYPE value) throw(...)
	{
		Pair* pNode = _Find(tKey);
		if(pNode == NULL)
		{
			return(RBInsert(tKey, value));
		}
		else
		{
			pNode->tValue_ = value;

			return(pNode);
		}
	}

	template< typename Key, typename Value, class KTraits, class VTraits >
	bool RBMap< Key, Value, KTraits, VTraits >::RemoveKey(KINARGTYPE tKey) throw()
	{
		Position pos = Find(tKey);
		if(pos != NULL)
		{
			RemoveAt(pos);

			return(true);
		}
		else
		{
			return(false);
		}
	}

#pragma pop_macro("new")
#pragma pop_macro("max")
};

/*
void
CollectionPerformanceTest(int iNumElements)
{
	typedef	U64	KeyType;
	//typedef	J9::StringW	KeyType;
	typedef	J9::StringW	ValueType;

	typedef	std::map<KeyType, ValueType>				STLMap;
	typedef stdext::hash_map<KeyType, ValueType>		STLHashMap;
	typedef J9::AtlMap<KeyType, ValueType>				AtlMap;
	typedef	J9::RBMap<KeyType, ValueType>				RBMap;
	typedef	std::vector<std::pair<KeyType, ValueType>>	TestData;
	STLMap		cMap;
	STLHashMap	cHashMap;
	AtlMap		cAtlMap;
	RBMap		cRBMap;
	TestData	cTestData;

	for (int i = 0; i < iNumElements; i++)
	{
		U64	iKey = J9::Random::cGlobal.GetU32();
		iKey = iKey << 32;
		iKey |= J9::Random::cGlobal.GetU32();
		J9::StringW	sValue = J9::StringUtil::Format(_W("%I64u"), iKey);
		cTestData.push_back(std::make_pair(iKey, sValue));
		//cTestData.push_back(std::make_pair(sValue, sValue));
	}

	TestData::iterator	cIter;
	TestData::iterator	cIterEnd = cTestData.end();

	DWORD dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		cMap.insert(std::make_pair(cIter->first, cIter->second));
	}
	LOG(_W("stl map insert %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		cHashMap.insert(std::make_pair(cIter->first, cIter->second));
	}
	LOG(_W("stl hash_map insert %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		cAtlMap.SetAt(cIter->first, cIter->second);
	}
	LOG(_W("atl map insert %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		cRBMap.SetAt(cIter->first, cIter->second);
	}
	LOG(_W("rb map insert %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (int i = 0; i < 1000000/iNumElements; ++i)
	{
		for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
		{
			STLMap::iterator	cMapIter = cMap.find(cIter->first);
			if (cMapIter == cMap.end())
				LOG(_W("error can not find %s\n"), cIter->second.c_str());
		}
	}
	LOG(_W("stl map find %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (int i = 0; i < 1000000/iNumElements; ++i)
	{
		for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
		{
			STLHashMap::iterator	cMapIter = cHashMap.find(cIter->first);
			if (cMapIter == cHashMap.end())
				LOG(_W("error can not find %s\n"), cIter->second.c_str());
		}
	}
	LOG(_W("stl hash_map find %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (int i = 0; i < 1000000/iNumElements; ++i)
	{
		for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
		{
			AtlMap::Pair*	pPair = cAtlMap.Find(cIter->first);
			if (pPair == NULL)
				LOG(_W("error can not find %s\n"), cIter->second.c_str());
		}
	}
	LOG(_W("atl map find %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (int i = 0; i < 1000000/iNumElements; ++i)
	{
		for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
		{
			RBMap::Pair*	pPair = cRBMap.Find(cIter->first);
			if (pPair == NULL)
				LOG(_W("error can not find %s\n"), cIter->second.c_str());
		}
	}
	LOG(_W("rb map find %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		STLMap::iterator	cMapIter = cMap.find(cIter->first);
		if (cMapIter != cMap.end())
			cMap.erase(cMapIter);
	}
	LOG(_W("stl map erase %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		STLHashMap::iterator	cMapIter = cHashMap.find(cIter->first);
		if (cMapIter != cHashMap.end())
			cHashMap.erase(cMapIter);
	}
	LOG(_W("stl hash_map erase %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		cAtlMap.RemoveKey(cIter->first);
	}
	LOG(_W("atl map erase %u(ms)\r\n"), timeGetTime() - dwStart);

	dwStart = timeGetTime();
	for (cIter = cTestData.begin(); cIter != cIterEnd; ++cIter)
	{
		cRBMap.RemoveKey(cIter->first);
	}
	LOG(_W("rb map erase %u(ms)\r\n"), timeGetTime() - dwStart);
}
*/

#endif//__J9_CONTAINERS_H__