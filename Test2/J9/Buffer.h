// $Id: Buffer.h 8706 2011-06-16 13:00:42Z kyuhun.lee $

#ifndef __J9_BUFFER_H__
#define __J9_BUFFER_H__

namespace J9
{
	template<typename Traits>
	class _Buffer
	{
		SET_CLASS_HEADER('ffub');

		template<typename IO, typename Type>
		friend struct TypeTraits_Binary_Custom;

	public:
		_Buffer(Size i = 0) : cBuffer_(i), iReadOffset_(0), iWriteOffset_(0)	{}
		_Buffer(const U8* p, Size i) : cBuffer_(p, p + i), iReadOffset_(0), iWriteOffset_(i)	{}
		virtual ~_Buffer()	{};

		bool		IsEmpty() const					{ return cBuffer_.empty(); }

		Size		GetSize() const					{ return cBuffer_.size(); }
		void		Resize(Size i)					{ cBuffer_.resize(i); }
		void		Clear()							{ std::vector<U8>	cSwap; cBuffer_.swap(cSwap); iReadOffset_ = 0; iWriteOffset_ = 0; }

		void		SetReadOffset(Size iOffset)		{ iReadOffset_ = iOffset; }
		Size		GetReadOffset() const			{ return iReadOffset_; }
		void		SetWriteOffset(Size iOffset)	{ iWriteOffset_ = iOffset; }
		Size		GetWriteOffset() const			{ return iWriteOffset_; }
		const U8*	GetPointer() const				{ return &cBuffer_[0]; }
		const U8*	GetPointer(Size i) const		{ return &cBuffer_[i]; }
		U8*			GetWritePointer()				{ return &cBuffer_[iWriteOffset_]; }
		const U8*	GetReadPointer()				{ return &cBuffer_[iReadOffset_]; }
		Size		GetReadRemain() const			{ return cBuffer_.size() - iReadOffset_; }
		inline void AddWriteOffset(Size iSize)		{ iWriteOffset_ += iSize; }
		inline void	AddReadOffset(Size iSize)		{ iReadOffset_ += iSize; }

		inline bool CanRead(Size iCount)			{ return (iReadOffset_ + iCount <= cBuffer_.size());}

		Size	Read(Size iCount, void* p)
		{
			ASSERT(iReadOffset_ + iCount <= cBuffer_.size());
			::memcpy(p, &cBuffer_[0] + iReadOffset_, iCount);
			iReadOffset_ += iCount;
			return iCount;
		}

		Size	Read(Size iCount, void* p, Size iOffset)
		{
			ASSERT(iOffset + iCount <= cBuffer_.size());
			::memcpy(p, &cBuffer_[0] + iOffset, iCount);
			return iCount;
		}

		Size	Write(Size iCount, const void* p)
		{
			ASSERT(iWriteOffset_ + iCount <= cBuffer_.size());
			::memcpy(&cBuffer_[0] + iWriteOffset_, p, iCount);
			iWriteOffset_ += iCount;
			return iCount;
		}

		Size	Write(Size iCount, const void* p, Size iOffset)
		{
			ASSERT(iOffset + iCount <= cBuffer_.size());
			::memcpy(&cBuffer_[0] + iOffset, p, iCount);
			return iCount;
		}

		template<typename T>
		void	Read(T& r)
		{
			Traits::Read(this, r);
		}

		template<typename T>
		void	Write(const T& r)
		{
			Traits::Write(this, r);
		}

		template<typename T>
		_Buffer&	operator>>(T& r)
		{
			Traits::Read(this, r);
			return *this;
		}

		template<typename T>
		_Buffer& operator<<(const T& r)
		{
			Traits::Write(this, r);
			return *this;
		}

	protected:
		std::vector<U8>	cBuffer_;
		Size			iReadOffset_;
		Size			iWriteOffset_;
	};

	typedef _Buffer<TypeTraits_Binary>
	Buffer;
};

#endif//__J9_BUFFER_H__
