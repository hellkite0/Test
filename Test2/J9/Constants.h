// $Id$

#ifndef __J9_CONSTANTS_H__
#define __J9_CONSTANTS_H__

static const S8		S8_MIN						= (SCHAR_MIN);
static const S8		S8_MAX						= (SCHAR_MAX);

static const U8		U8_MIN						= (0);
static const U8		U8_MAX						= (UCHAR_MAX);

static const S16	S16_MIN						= (SHRT_MIN);
static const S16	S16_MAX						= (SHRT_MAX);

static const U16	U16_MIN						= (0);
static const U16	U16_MAX						= (USHRT_MAX);

static const S32	S32_MIN						= (INT_MIN);
static const S32	S32_MAX						= (INT_MAX);

static const U32	U32_MIN						= (0);
static const U32	U32_MAX						= (UINT_MAX);

static const S64	S64_MIN						= (0x8000000000000000);
static const S64	S64_MAX						= (0x7fffffffffffffff);

static const U64	U64_MIN						= (0x0000000000000000);
static const U64	U64_MAX						= (0xffffffffffffffff);

static const U64	U48_MIN						= (0x0000000000000000);
static const U64	U48_MAX						= (0x0000ffffffffffff);

static const F32	F32_MIN						= (1.175494351e-38F);
static const F32	F32_MAX						= (3.402823466e+38F);

static const F32	F32_EPSILON					= (1.192092896e-07F);
#ifdef J9_SOL_USE_DIRECTX
// DirectX를 사용할 경우 수치를 통일하기 위해 D3DX의 값을 그대로 사용한다
static const F32	F32_PI						= D3DX_PI;
static const F32	F32_1_PI					= D3DX_1BYPI;
#else//J9_SOL_USE_DIRECTX
static const F32	F32_PI						= (3.141592654f);
static const F32	F32_1_PI					= (0.318309886f);
#endif//J9_SOL_USE_DIRECTX

static const F64	F64_MIN						= (2.2250738585072014e-308);
static const F64	F64_MAX						= (1.7976931348623158e+308);

static const F64	F64_EPSILON					= (2.2204460492503131e-016);
static const F64	F64_E						= (2.71828182845904523536);
static const F64	F64_LOG2E					= (1.44269504088896340736);
static const F64	F64_LOG10E					= (0.434294481903251827651);
static const F64	F64_LN2						= (0.693147180559945309417);
static const F64	F64_LN10					= (2.30258509299404568402);
static const F64	F64_PI						= (3.14159265358979323846);
static const F64	F64_PI_2					= (1.57079632679489661923);
static const F64	F64_PI_4					= (0.785398163397448309616);
static const F64	F64_1_PI					= (0.318309886183790671538);
static const F64	F64_2_PI					= (0.636619772367581343076);
static const F64	F64_2_SQRTPI				= (1.12837916709551257390);
static const F64	F64_SQRT2					= (1.41421356237309504880);
static const F64	F64_SQRT1_2					= (0.707106781186547524401);

static const int	TINY_NUMBER					= 64;
static const int	SMALL_NUMBER				= 256;
static const int	DEFAULT_NUMBER				= 1024;
static const int	BIG_NUMBER					= 4096;
static const int	LARGE_NUMBER				= 16384;
static const int	HUGE_NUMBER					= 65536;
static const int	GIGANTIC_NUMBER				= 262144;

static const int	KILO						= 1000;
static const int	MEGA						= 1000000;
static const int	GIGA						= 1000000000;

static const F32	MILLI						= .001f;
static const F32	MICRO						= .000001f;
static const F32	NANO						= .000000001f;


#endif//__J9_CONSTANTS_H__
