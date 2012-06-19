// $Id: File.h 8980 2011-07-15 05:18:21Z donghyuk.lee $

#ifndef __J9_FILE_H__
#define __J9_FILE_H__

////////////////////////////////////////////////////////////////////////////////
/// \brief Just9 Library
///
/// ������Ʈ�� �⺻�� �Ǵ� ��ä���� ��Ƴ��� ���ӽ����̽� �̴�.
///
/// \author boro
/// \date 2005/04/22
/// \warning �����̷���
////////////////////////////////////////////////////////////////////////////////
namespace J9
{
	////////////////////////////////////////////////////////////////////////////////
	/// \brief ������ ���� ����� ��Ÿ���� ������
	///
	/// �� ���������� ���ÿ� ���� �� �ִ�.
	////////////////////////////////////////////////////////////////////////////////
	enum FileOpenFlag
	{
		FileOpenRead	= 0x01,	///< �б������ ����.
		FileOpenWrite	= 0x02, ///< ��������� ����.
		FileOpenCreate	= 0x04, ///< ������ �������� �ʴ� ��� �����Ѵ�.
		FileOpenNoTrunc	= 0x08, ///< ������ �̹� �����ϴ� ��� ���� ������ �����Ѵ�.
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief Seek�� ������ Ư�� ��ġ�� �̵��ϴ� ����� ��Ÿ���� ������
	///
	/// ������ �������� ���������� ���Ǿ�� �Ѵ�.
	////////////////////////////////////////////////////////////////////////////////
	enum FileSeekMethod
	{
		FileSeekBegin,			///< ������ �� �պκп��� ���� offset��ŭ �̵�
		FileSeekCur,			///< ���� ��ġ�� ���� offset��ŭ �̵�
		FileSeekEnd,			///< ������ ������ ���� offset��ŭ �̵�
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		������ IO�� ����ϴ� File�� base interface.
	///
	///				io class�� �����ϱ� ���ؼ��� �� Ŭ������ ��� �ް�
	///				�ʿ��� ���� �Լ��� �����ϸ� �ȴ�.
	/// \author		boro
	/// \date		2005/04/22
	////////////////////////////////////////////////////////////////////////////////
	class _IOBase
	{
	public:
		virtual			~_IOBase()								{}
		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ ����.
		///
		/// \param[in]	rName	File Name, const StringW&
		/// \param[in]	iFlag	Open Falg, U32
		/// \return		������ �������� true, ���������� false
		/// \see FileOpenFlag
		////////////////////////////////////////////////////////////////////////////////
		virtual bool	Open(const WChar* pName, U32 iFlag)		{ UNUSED(pName); UNUSED(iFlag); ASSERT(0); return false; }
		virtual bool	Open(const AChar* pName, U32 iFlag)		{ UNUSED(pName); UNUSED(iFlag); ASSERT(0); return false; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ ���� �ִ����� �����Ѵ�.
		///
		/// \return		������ �������� true, ���������� false
		////////////////////////////////////////////////////////////////////////////////
		virtual bool	IsOpened() const						{ ASSERT(0); return false; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief �����ִ� ������ �ݴ´�.
		////////////////////////////////////////////////////////////////////////////////
		virtual void	Close()									{ ASSERT(0); }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ �д´�.
		///
		/// \param[out]	p	���� ������ ������ buffer.
		/// \param[in]	i	������ ũ��
		/// \return		������ �о���� ������ ũ��
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	Read(Size i, void* p)					{ UNUSED(p); UNUSED(i); ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief ���Ͽ� ����.
		///
		/// \param[in]	p  ������ ������ ����ִ� buffer.
		/// \param[in]	i	������ ũ��
		/// \return		������ ����� ������ ũ��
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	Write(Size i, const void* p)			{ UNUSED(p); UNUSED(i); ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ ������ ��ũ�� ������ ������ �Ѵ�.
		////////////////////////////////////////////////////////////////////////////////
		virtual void	Flush()			{ ASSERT(0); }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief Ư���� ��ġ�� offset�� �̵���Ų��.
		///
		/// \param[in]	iSeek	�־��� ��ġ�� ������
		/// \param[in]	iPos	���������κ����� offset
		/// \see FileSeekMethod
		////////////////////////////////////////////////////////////////////////////////
		virtual void	Seek(FileSeekMethod iSeek, int iOffset)	{ UNUSED(iSeek); UNUSED(iOffset); ASSERT(0); }

		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ offset�� ���Ѵ�.
		///
		/// \return		������ offset
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	GetPos() const							{ ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ ũ�⸦ ���Ѵ�.
		///
		/// \return		������ ũ��.
		////////////////////////////////////////////////////////////////////////////////
		virtual Size	GetSize() const							{ ASSERT(0); return 0; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief ������ �����͸� ���Ѵ�.
		///
		/// \return		������ ũ��.
		////////////////////////////////////////////////////////////////////////////////
		virtual const U8*		GetPointer(Size iOffset = 0) const	{ UNUSED(iOffset); ASSERT(0); return NULL; }
		////////////////////////////////////////////////////////////////////////////////
		/// \brief		������ ���� �＼�� �� �� �ִ� �������� ���θ� �����Ѵ�.
		///				������ �������� ���� ���¿��� ȣ���ϸ� ����� �� �����
		///				�˾Ƴ� �� ���� ���� �����Ƿ�, ������ ���� �� ȣ���ϵ��� �Ѵ�
		///
		/// \return		���� �＼�� �� �� ������ true
		////////////////////////////////////////////////////////////////////////////////
		virtual bool			IsDirectFile() const				{ ASSERT(0); return false; }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		c standard library�� ����� ������ IO
	///
	///				_IOBase �������̽��� ����Ͽ� c standard library�� ������ Ŭ���� �̴�.
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
	/// \brief		Win32 File Mapping�� ����� ������ IO
	///
	///				_IOBase �������̽��� ����Ͽ� Win32 File Mapping�� ������ Ŭ���� �̴�.
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
		// �����ȵȰ� �����ʾƵ� �Ǵ� �༮��, �ϴ� �� ���δ�;;;
		//virtual Size		Write(Size i, const void* p);
		//virtual void		Flush();

		virtual void		Seek(FileSeekMethod iSeek, int iOffset);

		virtual Size		GetPos() const;
		virtual Size		GetSize() const;
		virtual const U8*	GetPointer(Size iOffset = 0) const;
		virtual bool		IsDirectFile() const;

	private:
		// �ϴ� StandardIO�� �ִ°Ͱ� ���� ���� ����...
		// File.inl �� _Method �������ε� ������ε� ���°� �ʿ��ҵ�...
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
		FILE*			pFile_;	// package �� ���� file �� �ִ� ���
		J9::FileInfo*	pFileInfo_;
		U8*				pBase_;	// package �ȿ� �����ϴ� ���
		Size			iPos_;	// current position in the io
		Size			iSize_;
	};

	struct FileOpenProcess
	{
		////////////////////////////////////////////////////////////////////////////////
		/// \brief		������ open�� �Ŀ� �Ҹ�.
		///
		///				������ ���µ� �� traits�� ���� �ؾ��� �۾����� �����ϴ� �Լ�
		///				����� �о�ٰų� ����ϴ� ���� �۾��� �Ѵ�.
		///	\param[out]	pIO		����IO
		/// \param[in]	iFlag	������ ���µ� ����� flag
		////////////////////////////////////////////////////////////////////////////////
		static bool	DoPostProcess(_IOBase* pIO, U32 iFlag)	{ UNUSED(pIO); UNUSED(iFlag); return true; }
	};

	struct FileOpenProcess_Binary
	{
		static bool	DoPostProcess(_IOBase* pIO, U32 iFlag)	{ UNUSED(pIO); UNUSED(iFlag); return true; }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// \brief		stream�� ���� �־����� ��ü�� unicode text format���� �а� ���� traits
	///
	///				����� �Է����� �־����� ��ü�� �����Ŀ� �ٽ� �о���� ��
	///				�� ��ü ������ �����ϱ� ���� delimeter���� ���� ���� �ʾƼ�
	///				StringW ��ü�� �����ϵ��� �Ѵ�. StringW ��ü ���� multi line�� ��
	///				�� �ְ�, ���� �ϳ��� StringW�� ������ �� �ִ� ������ �����Ƿ�
	///				StringW ���常 �����Ѵ�.
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
	// ���������� PACKAGE�� ������ ����.
	typedef PackageUnicodeFile							TextPackageFile;
	typedef PackageBinaryFile							PackageFile;
#else
	typedef StandardUnicodeFile							TextPackageFile;
	typedef StandardBinaryFile							PackageFile;
#endif
};

#include "File.inl"

#endif//__J9_FILE_H__
