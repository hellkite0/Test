// $Id: Package.h 8959 2011-07-13 08:49:53Z hyojin.lee $

#ifndef __J9_PACKAGE_H__
#define __J9_PACKAGE_H__

#include "File.h"
#include "Mutex.h"

namespace J9
{
	class Package;
	// blow fish 는 8bytes 단위로 끊긴다, 그래서 crypt offset 추가
	struct FileInfo
	{
		StringW		sAbsPath_;
		StringW		sPath_;					// relative data file path
		U32			iSize_;					// size in package
		U32			iOffset_;				// starting offset in package
		U32			iOffsetInView_;			// offset in file view
		U32			iCRC32_;				// crc32 checksum of the final results
		const U8*	pContent_;
		Package*	pPackage_;

		U8			iCryptOffset_;			// crypted offset (original size = size - crypted offset)
		U8			iCompressed_;			// 0 if not compressed, else compression ratio
		bool		bCrypted_;				// true if crypted
		void		Debug(const StringW& r);
		void		OnOpen();
		void		OnClose();
	};

	class Package
	{
		friend class PackageManager;
		friend struct FileInfo;

	public:
		Package();
		Package(const StringW& rPath);
		~Package();

		inline void SetVerbose(bool b) { bVerbose_ = b; }
		void	SetPath(const StringW& rPath) { sPath_ = rPath; }
		bool	Write(const StringW& rPath, void* pContent, U32 iSize);
		void*	ReadFile(const StringW& rPath, U32* pSize, bool bAlign = false);
		void	AddFile(const WChar* pPath, bool bCompress = false, bool bCrypt = false);

		void	Save(const WChar* pPath);
		//bool	Load(const WChar* pPath);  // pContent 에 loading
		bool	_LoadHeader(const WChar* pPath); // cFile_ 에 loading

		void	PackList(const WChar* pDir, bool bCompress = false, bool bCrypt = false, bool bIncludeSubDir = false, bool bList = false);

		bool	HasFile(const WChar* pName);
		StringW	GetName() const;

		void Reset();

	protected:
		static void	Encrypt(void* pContent, U32 iSize);
		static void	Decrypt(void* pContent, U32 iSize);
		static void*	Compress(void* pContent, U32* pSize);
		static void*	Decompress(void* pContent, U32* pSize, U8 iRatio);

	private:
		void	AddFileList(const WChar* pPath, bool bCompress = false, bool bCrypt = false);

		void	PackList();
		void	_Pack(const WChar* pDir, bool bCompress = false, bool bCrypt = false, bool bIncludeSubDir = false, bool bList = false);
		//void	Pack(const WChar* pDir, bool bCompress = false, bool bCrypt = false, bool bIncludeSubDir = false, bool bList = false);
		bool	_Add(const WChar* pName, void* pContent, U32 iSize, bool bCompress = false, bool bCrypt = false);
		bool	Add(const WChar* pName, void* pContent, U32 iSize, bool bCompress = false, bool bCrypt = false);

		void	Unpack(const WChar* pDir);

		Ptr<Buffer>		MakeHeader();
		U32				BreakHeader(U32 iSize);

	protected:
		stdext::hash_map<StringW, Ptr<FileInfo> > cFileList_;

	private:
		stdext::hash_map<StringW, Ptr<FileInfo> > cRawList_;

		void*		pCreateContents_;

		U32			iContentsSize_;
		U32			iRawContentsSize_;
		StringW		sName_;
		StringW		sPath_;
		StringW		sPathName_;
		Size		iHeaderSize_;
		MappedFile	cFile_;
		Lock		cLock_;
		bool		bVerbose_;
	};

	#define pPkgMgr	J9::PackageManager::Instance()

	class PackageManager
	{
		friend class Package;
		DYNAMIC_SINGLETON(PackageManager);
	public:
		PackageManager();
		~PackageManager();

		void					Remove(const StringW& rName) { cPackages_.erase(rName); }
		void					Add(const StringW& rName, Ptr<Package> pPackage);
		Package*				Get(const StringW& rName)
		{
			stdext::hash_map<StringW, Ptr<Package> >::iterator	cIter = cPackages_.find(rName);
			if (cIter == cPackages_.end())
				return NULL;

			return cIter->second;
		}

		FileInfo*				GetFile(const WChar* pName);
		bool					GetFile(FileInfo* pInfo, void* pFile, U32* pSize);
		bool					HasFile(const WChar* pName);
		Size					FindFile(const J9::StringW& rFileName, std::vector<J9::StringW>& rFound, bool bRecursive = false, bool bWithPath = false);
		Size					FindFile(const J9::StringA& rFileName, std::vector<J9::StringA>& rFound, bool bRecursive = false, bool bWithPath = false);
		void					LoadPackages(const StringW& rPath);

	private:
		StringW GetPackageName(const StringW& rPath);
		StringW GetReplacedName(const WChar* pPath);

	private:
		stdext::hash_map<StringW, Ptr<Package> > cPackages_;
		stdext::hash_map<StringW, Ptr<FileInfo> > cFiles_;
	};
};

#endif //  __J9_PACKAGE_H__
