// $Id: File.h 8980 2011-07-15 05:18:21Z donghyuk.lee $

#ifndef __J9_FILE_H__
#define __J9_FILE_H__

////////////////////////////////////////////////////////////////////////////////
/// \brief Just9 Library
///
/// 프로젝트의 기본이 되는 객채들을 모아놓은 네임스페이스 이다.
///
/// \author boro
/// \date 2005/04/22
/// \warning 워닝이랜다
////////////////////////////////////////////////////////////////////////////////
namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief 파일을 여는 방법을 나타내는 열거형
	///
	/// 각 열거형들은 동시에 쓰일 수 있다.
	////////////////////////////////////////////////////////////////////////////////
	enum FileOpenFlag
	{
		FileOpenRead	= 0x01,	///< 읽기용으로 연다.
		FileOpenWrite	= 0x02, ///< 쓰기용으로 연다.
		FileOpenCreate	= 0x04, ///< 파일이 존재하지 않는 경우 생성한다.
		FileOpenNoTrunc	= 0x08, ///< 파일이 이미 존재하는 경우 기존 내용을 유지한다.
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief Seek로 파일의 특정 위치로 이동하는 방법을 나타내는 열거형
	///
	/// 각각의 열거형은 독립적으로 사용되어야 한다.
	////////////////////////////////////////////////////////////////////////////////
	enum FileSeekMethod
	{
		FileSeekBegin,			///< 파일의 맨 앞부분에서 부터 offset만큼 이동
		FileSeekCur,			///< 현재 위치로 부터 offset만큼 이동
		FileSeekEnd,			///< 파일의 끝에서 부터 offset만큼 이동
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		저수준 IO를 담당하는 File의 base interface.
	///
	///				io class를 구현하기 위해서는 이 클래스를 상속 받고
	///				필요한 가상 함수를 구현하면 된다.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	class _IOBase
	{
	public:
		virtual			~_IOBase()								{}
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 파일을 연다.
		///
		/// \param[in]	rName	File Name, const StringW&
		/// \param[in]	iFlag	Open Falg, U32
		/// \return		파일이 열렸으면 true, 실패했으면 false
		/// \see FileOpenFlag
		////////////////////////////////////////////////////////////////////////////////
		virtual bool	Open(const WChar* pName, U32 iFlag)		{ UNUSED(pName); UNUSED(iFlag); ASSERT(0); return false; }
		virtual bool	Open(const AChar* pName, U32 iFlag)		{ UNUSED(pName); UNUSED(iFlag); ASSERT(0); return false; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 파일이 열려 있는지를 리턴한다.
		///
		/// \return		파일이 열렸으면 true, 실패했으면 false
		////////////////////////////////////////////////////////////////////////////////
		virtual bool	IsOpened() const						{ ASSERT(0); return false; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 열려있는 파일을 닫는다.
		////////////////////////////////////////////////////////////////////////////////
		virtual void	Close()									{ ASSERT(0); }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief 파일을 읽는다.
		///
		/// \param[out]	p	읽은 내용을 저장할 buffer.
		/// \param[in]	i	버퍼의 크기
		/// \return		실제로 읽어들인 내용의 크기
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	Read(Size i, void* p)					{ UNUSED(p); UNUSED(i); ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 파일에 쓴다.
		///
		/// \param[in]	p  저장할 내용이 들어있는 buffer.
		/// \param[in]	i	버퍼의 크기
		/// \return		실제로 기록한 내용의 크기
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	Write(Size i, const void* p)			{ UNUSED(p); UNUSED(i); ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 버퍼의 내용을 디스크에 강제로 쓰도록 한다.
		////////////////////////////////////////////////////////////////////////////////
		virtual void	Flush()			{ ASSERT(0); }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief 특정한 위치로 offset을 이동시킨다.
		///
		/// \param[in]	iSeek	주어진 위치의 기준점
		/// \param[in]	iPos	기준점으로부터의 offset
		/// \see FileSeekMethod
		////////////////////////////////////////////////////////////////////////////////
		virtual void	Seek(FileSeekMethod iSeek, int iOffset)	{ UNUSED(iSeek); UNUSED(iOffset); ASSERT(0); }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief 현재의 offset를 구한다.
		///
		/// \return		현재의 offset
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	GetPos() const							{ ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 파일의 크기를 구한다.
		///
		/// \return		파일의 크기.
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	GetSize() const							{ ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief 파일의 포인터를 구한다.
		///
		/// \return		파일의 크기.
		////////////////////////////////////////////////////////////////////////////////
		virtual const U8*		GetPointer(Size iOffset = 0) const	{ UNUSED(iOffset); ASSERT(0); return NULL; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief		파일이 직접 억세스 할 수 있는 파일인지 여부를 리턴한다.
		///				파일이 열려있지 않은 상태에서 호출하면 제대로 된 결과를
		///				알아낼 수 없을 때가 있으므로, 파일을 열고난 후 호출하도록 한다
		///
		/// \return		직접 억세스 할 수 있으면 true
		////////////////////////////////////////////////////////////////////////////////
		virtual bool			IsDirectFile() const				{ ASSERT(0); return false; }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		c standard library를 사용해 구현한 IO
	///
	///				_IOBase 인터페이스를 상속하여 c standard library로 구현한 클래스 이다.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	class StandardIO : public _IOBase
	{
	public:
		StandardIO() : p_(NULL) {}
		virtual	~StandardIO()	{ Close(); }

		virtual bool	Open(const WChar* pName, U32 iFlag);
		virtual bool	Open(const AChar* pName, U32 iFlag);
		virtual bool	IsOpened() const;
		virtual void	Close();

		virtual Size	Read(Size i, void* p);
		virtual Size	Write(Size i, const void* p);
		virtual void	Flush();

		virtual void	Seek(FileSeekMethod iSeek, int iOffset);

		virtual Size	GetPos() const;
		virtual Size	GetSize() const;
		virtual bool	IsDirectFile() const;

	private:
		FILE*	p_;
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		Win32 File Mapping을 사용해 구현한 IO
	///
	///				_IOBase 인터페이스를 상속하여 Win32 File Mapping로 구현한 클래스 이다.
	/// \author		boro
	/// \date		2006/10/09
	////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
	class MappedIO : public _IOBase
	{
	public:
		MappedIO();
		virtual	~MappedIO()	{ CloseMapping(); }

		virtual bool	IsOpened() const;
		virtual Size	GetSize() const;

		bool			OpenMapping(const WChar* pName, U32 iFlag);
		bool			OpenMapping(const AChar* pName, U32 iFlag);
		void			CloseMapping();
		const U8*		OpenPartial(U32& rOffset, U32 iSize);
		void			ClosePartial(const U8* pView);

	private:
		static void	CheckAllocationGranularity();

	private:
		HANDLE					hFile_;
		HANDLE					hMapping_;
		std::vector<const U8*>	pViews_;

		static U32	iAllocationGranularity;
	};
#endif

	struct FileInfo;
	class PackageIO : public _IOBase
	{
	public:
		PackageIO();
		virtual ~PackageIO() { Close(); }

		virtual bool		Open(const WChar* pName, U32 iFlag);
		virtual bool		Open(const AChar* pName, U32 iFlag);
		virtual bool		IsOpened() const;
		virtual void		Close();

		virtual Size		Read(Size i, void* p);
		// 구현안된건 쓰지않아도 되는 녀석들, 일단 걍 죽인다;;;
		//virtual Size		Write(Size i, const void* p);
		//virtual void		Flush();

		virtual void		Seek(FileSeekMethod iSeek, int iOffset);

		virtual Size		GetPos() const;
		virtual Size		GetSize() const;
		virtual const U8*	GetPointer(Size iOffset = 0) const;
		virtual bool		IsDirectFile() const;

	private:
		// 일단 StandardIO에 있는것과 대충 같은 내용...
		// File.inl 로 _Method 형식으로든 어떤식으로든 빼는게 필요할듯...
		inline void SeekFile(FileSeekMethod iSeek, int iOffset)
		{
			if (!IsFileOpened())
				return;

			S32	iOrigin;
			switch (iSeek)
			{
			case FileSeekBegin: iOrigin = SEEK_SET;	break;
			case FileSeekCur:	iOrigin = SEEK_CUR;	break;
			case FileSeekEnd:	iOrigin = SEEK_END;	break;
			default:
				// default seek_cur
				ASSERT(0);
				iOrigin = SEEK_CUR;
				break;
			}

			VERIFY(::fseek(pFile_, _sc<long>(iOffset), iOrigin) == 0);
		}
		inline Size GetFileSize() const
		{
			if (!IsFileOpened())
			{
				ASSERT(0);
				return 0;
			}

			Size	iSave, iSize;

			iSave = GetPos();

			_cc<PackageIO*>(this)->Seek(FileSeekEnd, 0);
			iSize = GetFilePos();

			_cc<PackageIO*>(this)->Seek(FileSeekBegin, _sc<int>(iSave));

			return iSize;
		}

		inline Size GetFilePos() const
		{
			if (!IsOpened())
			{
				ASSERT(0);
				return 0;
			}

			fpos_t	iPos;
			VERIFY(::fgetpos(pFile_, &iPos) == 0);
			return _sc<Size>(iPos);
		}
		inline Size ReadFile(Size i, void* p)
		{
			return ::fread(p, 1, i, pFile_);
		}
		inline void CloseFile()
		{
			VERIFY(::fflush(pFile_) == 0);
			VERIFY(::fclose(pFile_) == 0);

			pFile_ = NULL;
		}
		inline bool IsFileOpened() const { return (pFile_ != NULL); }
		bool OpenFile(const WChar* pName, U32 iFlag);
		bool OpenFile(const AChar* pName, U32 iFlag);

	private:
		FILE*			pFile_;	// package 에 없고 file 로 있는 경우
		J9::FileInfo*	pFileInfo_;
		U8*				pBase_;	// package 안에 존재하는 경우
		Size			iPos_;	// current position in the io
		Size			iSize_;
	};

	struct FileOpenProcess
	{
		////////////////////////////////////////////////////////////////////////////////
		/// \brief		파일이 open된 후에 불림.
		///
		///				파일이 오픈된 후 traits에 따라 해야할 작업들을 구현하는 함수
		///				헤더를 읽어낸다거나 기록하는 등의 작업을 한다.
		///	\param[out]	pIO		파일IO
		/// \param[in]	iFlag	파일을 여는데 사용한 flag
		////////////////////////////////////////////////////////////////////////////////
		static bool	DoPostProcess(_IOBase* pIO, U32 iFlag)	{ UNUSED(pIO); UNUSED(iFlag); return true; }
	};

	struct FileOpenProcess_Binary
	{
		static bool	DoPostProcess(_IOBase* pIO, U32 iFlag)	{ UNUSED(pIO); UNUSED(iFlag); return true; }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		stream을 통해 주어지는 객체를 unicode text format으로 읽고 쓰는 traits
	///
	///				현재는 입력으로 주어지는 객체를 저장후에 다시 읽어들일 때
	///				각 객체 단위를 구분하기 위한 delimeter등이 정해 지지 않아서
	///				StringW 객체만 저장하도록 한다. StringW 객체 또한 multi line이 될
	///				수 있고, 실제 하나의 StringW을 구분할 수 있는 기준이 없으므로
	///				StringW 저장만 지원한다.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	struct FileOpenProcess_UnicodeText
	{
		static bool	DoPostProcess(_IOBase* pIO, U32 iFlag);
	};

	template<typename _IO, typename _StreamTraits, typename _OpenProcess>
	class _File : public _IO
	{
	public:
		typedef _StreamTraits	StreamTraits;
		typedef _OpenProcess	OpenProcess;

	public:
		virtual bool	Open(const WChar* pName, U32 iFlag)
		{
			if (!_IO::Open(pName, iFlag))
				return false;

			return OpenProcess::DoPostProcess(this, iFlag);
		}

		virtual bool	Open(const AChar* pName, U32 iFlag)
		{
			if (!_IO::Open(pName, iFlag))
				return false;

			return OpenProcess::DoPostProcess(this, iFlag);
		}

		template<typename T>
		_File&			operator>>(T& r)
		{
			StreamTraits::Read(this, r);

			return *this;
		}

		template<typename T>
		_File&			operator<<(const T& r)
		{
			StreamTraits::Write(this, r);

			return *this;
		}
	};

	typedef _File<_IOBase, TypeTraits, FileOpenProcess>
	DummyFile;
	typedef _File<StandardIO, TypeTraits_Binary, FileOpenProcess_Binary>
	StandardBinaryFile;
	typedef _File<StandardIO, TypeTraits_UnicodeText, FileOpenProcess_UnicodeText>
	StandardUnicodeFile;
	typedef _File<MappedIO, TypeTraits_Binary, FileOpenProcess_Binary>
	MappedBinaryFile;

	typedef _File<PackageIO, TypeTraits_UnicodeText, FileOpenProcess_UnicodeText>
	PackageUnicodeFile;
	typedef _File<PackageIO, TypeTraits_Binary, FileOpenProcess_Binary>
	PackageBinaryFile;

	typedef StandardBinaryFile							File;
	typedef StandardUnicodeFile							TextFile;
	typedef MappedBinaryFile							MappedFile;
#ifdef _PACKAGE
	// 서버에서는 PACKAGE를 쓸일이 없다.
	typedef PackageUnicodeFile							TextPackageFile;
	typedef PackageBinaryFile							PackageFile;
#else
	typedef StandardUnicodeFile							TextPackageFile;
	typedef StandardBinaryFile							PackageFile;
#endif
};

#include "File.inl"

#endif//__J9_FILE_H__
