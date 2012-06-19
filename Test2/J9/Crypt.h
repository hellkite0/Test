// $Id: Crypt.h 3959 2009-09-25 09:50:04Z hyojin.lee $

#ifndef __J9_CRYPT_H__
#define __J9_CRYPT_H__

/////////////////////////////////////////////////////////////////////////
// MD5.cpp
// Implementation file for MD5 class
//
// This C++ Class implementation of the original RSA Data Security, Inc.
// MD5 Message-Digest Algorithm is copyright (c) 2002, Gary McNickle.
// All rights reserved.  This software is a derivative of the "RSA Data
//  Security, Inc. MD5 Message-Digest Algorithm"
//
// You may use this software free of any charge, but without any
// warranty or implied warranty, provided that you follow the terms
// of the original RSA copyright, listed below.
//
// Original RSA Data Security, Inc. Copyright notice
/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
// These notices must be retained in any copies of any part of this
// documentation and/or software.
/////////////////////////////////////////////////////////////////////////

namespace J9
{
	class Crypt
	{
	public:
		static J9::StringA	PrintMD5(U8 iMD5Digest[16]);
		static J9::StringA	MD5String(const char* pString);
		static J9::StringA	MakePassword(const J9::StringA& sPassword);
		static bool			ComparePassword(const J9::StringA& sPassword, const J9::StringA& sCryptedPwd);

		Crypt()	{ Init(); }
		void	Update(U8* pInput, U32 iInputLen);
		void	Finalize();

	private:
		U8*		Digest() { return iDigests_; }
		void	Init();
		void	Transform(U8* pBlock);
		void	Encode(U8* pDest, U32* pSrc, U32 iLength);
		void	Decode(U32* pDest, U8* pSrc, U32 iLength);

		inline U32		RotateLeft(U32 x, U32 n)	{ return ((x << n) | (x >> (32 - n))); }
		inline U32		F(U32 x, U32 y, U32 z)		{ return ((x & y) | (~x & z)); }
		inline U32		G(U32 x, U32 y, U32 z)		{ return ((x & z) | (y & ~z)); }
		inline U32		H(U32 x, U32 y, U32 z)		{ return (x ^ y ^ z); }
		inline U32		I(U32 x, U32 y, U32 z)		{ return (y ^ (x | ~z)); }
		inline	void	FF(U32& a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac)	{ a += F(b, c, d) + x + ac; a = RotateLeft(a, s); a += b; }
		inline	void	GG(U32& a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac)	{ a += G(b, c, d) + x + ac; a = RotateLeft(a, s); a += b; }
		inline	void	HH(U32& a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac)	{ a += H(b, c, d) + x + ac; a = RotateLeft(a, s); a += b; }
		inline	void	II(U32& a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac)	{ a += I(b, c, d) + x + ac; a = RotateLeft(a, s); a += b; }

	private:
		U32		iStates_[4];
		U32		iCounts_[2];
		U8		iBuffers_[64];
		U8		iDigests_[16];
		//U8		iFinalized_;

	};

	namespace Compression
	{
		bool Compress(unsigned char* pCompressed, U32* pCompressedLength, unsigned char* pImage, U32 iImageLength);
		bool Decompress(unsigned char* pUncompressed, U32* pUncompressedLength, unsigned char* pImage, U32 iImageLength);
		U32	GetCompressBound(U32 iSourceSize);
		U32 GetCRC32(void* pImage, U32 iImageLen);
	}

	#define NUM_SUBKEYS  18
	#define NUM_S_BOXES  4
	#define NUM_ENTRIES  256

	#define MAX_STRING   256
	#define MAX_PASSWD   56  // 448bits

	class Blowfish
	{
	#ifdef __BIG_ENDIAN__
		struct WordByte
		{
			U32 i0_:8;
			U32	i1_:8;
			U32	i2_:8;
			U32	i3_:8;
		};
	#endif

	#ifdef __LITTLE_ENDIAN__
		struct WordByte
		{
			U32 i3_:8;
			U32 i2_:8;
			U32 i1_:8;
			U32 i0_:8;
		};
	#endif

		union Word
		{
			U32 iWord;
			WordByte cByte;
		};

		struct DWord
		{
			Word cWord0;
			Word cWord1;
		};

	public:
		Blowfish();
		~Blowfish();

		void Reset();
		void SetPassword(char* pPasswd);
		void Encrypt(void* p, U32 iNBytes);
		void Decrypt(void *p, U32 iNBytes);

		//S32 Test();
		//double Speed();

	private:
		void GenerateSubKeys(char* pPasswd);
		inline void _Encrypt(Word* p1,Word* p2);
		inline void _Decrypt(Word* p1,Word* p2);

	private:
		U32	iPA_[NUM_SUBKEYS];
		U32	iSB_[NUM_S_BOXES][NUM_ENTRIES];
	};
};

#endif // __J9_CRYPT_H__
