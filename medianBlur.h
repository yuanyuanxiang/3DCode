/**
* @file medianBlur.h													*
* @brief 中值滤波														*
* 袁沅祥，摘自OpenCV(下述文件)											\n
* ..\sources\modules\imgproc\src\smooth.cpp								\n
* ..\sources\modules\core\src\system.cpp								\n
* 2016年10月9日
*/

#pragma once

#include "DataTypes.h"

#include "stdafx.h"
#include <assert.h>
#include <memory.h>
//#include <intrin.h>

/// 设置为1，来开启SSE2指令
#define CV_SSE2 1

/*********************************** CPU capabilities ***********************************/

#define CV_CPU_NONE    0
#define CV_CPU_MMX     1
#define CV_CPU_SSE     2
#define CV_CPU_SSE2    3
#define CV_CPU_SSE3    4
#define CV_CPU_SSSE3   5
#define CV_CPU_SSE4_1  6
#define CV_CPU_SSE4_2  7
#define CV_CPU_POPCNT  8
#define CV_CPU_AVX    10
#define CV_CPU_AVX2   11
#define CV_HARDWARE_MAX_FEATURE 255

typedef unsigned short ushort;

const uchar icvSaturate8u_cv[] =
{
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255
};

#define CV_FAST_CAST_8U(t)  (assert(-256 <= (t) && (t) <= 512), icvSaturate8u_cv[(t)+256])

/************************************************************************/
/* System.cpp（201-306行）                                              */
/************************************************************************/

struct HWFeatures
{
	enum { MAX_FEATURE = CV_HARDWARE_MAX_FEATURE };

	HWFeatures(void)
	{
		memset( have, 0, sizeof(have) );
		x86_family = 0;
	}

	static HWFeatures initialize(void)
	{
		HWFeatures f;
		int cpuid_data[4] = { 0, 0, 0, 0 };

#if defined _MSC_VER && (defined _M_IX86 || defined _M_X64)
		__cpuid(cpuid_data, 1);
#elif defined __GNUC__ && (defined __i386__ || defined __x86_64__)
#ifdef __x86_64__
		asm __volatile__
			(
			"movl $1, %%eax\n\t"
			"cpuid\n\t"
			:[eax]"=a"(cpuid_data[0]),[ebx]"=b"(cpuid_data[1]),[ecx]"=c"(cpuid_data[2]),[edx]"=d"(cpuid_data[3])
			:
		: "cc"
			);
#else
		asm volatile
			(
			"pushl %%ebx\n\t"
			"movl $1,%%eax\n\t"
			"cpuid\n\t"
			"popl %%ebx\n\t"
			: "=a"(cpuid_data[0]), "=c"(cpuid_data[2]), "=d"(cpuid_data[3])
			:
		: "cc"
			);
#endif
#endif

		f.x86_family = (cpuid_data[0] >> 8) & 15;
		if( f.x86_family >= 6 )
		{
			f.have[CV_CPU_MMX]    = (cpuid_data[3] & (1 << 23)) != 0;
			f.have[CV_CPU_SSE]    = (cpuid_data[3] & (1<<25)) != 0;
			f.have[CV_CPU_SSE2]   = (cpuid_data[3] & (1<<26)) != 0;
			f.have[CV_CPU_SSE3]   = (cpuid_data[2] & (1<<0)) != 0;
			f.have[CV_CPU_SSSE3]  = (cpuid_data[2] & (1<<9)) != 0;
			f.have[CV_CPU_SSE4_1] = (cpuid_data[2] & (1<<19)) != 0;
			f.have[CV_CPU_SSE4_2] = (cpuid_data[2] & (1<<20)) != 0;
			f.have[CV_CPU_POPCNT] = (cpuid_data[2] & (1<<23)) != 0;
			f.have[CV_CPU_AVX]    = (((cpuid_data[2] & (1<<28)) != 0)&&((cpuid_data[2] & (1<<27)) != 0));//OS uses XSAVE_XRSTORE and CPU support AVX
		}

#if defined _MSC_VER && (defined _M_IX86 || defined _M_X64)
		__cpuidex(cpuid_data, 7, 0);
#elif defined __GNUC__ && (defined __i386__ || defined __x86_64__)
#ifdef __x86_64__
		asm __volatile__
			(
			"movl $7, %%eax\n\t"
			"movl $0, %%ecx\n\t"
			"cpuid\n\t"
			:[eax]"=a"(cpuid_data[0]),[ebx]"=b"(cpuid_data[1]),[ecx]"=c"(cpuid_data[2]),[edx]"=d"(cpuid_data[3])
			:
		: "cc"
			);
#else
		// We need to preserve ebx since we are compiling PIC code.
		// This means we cannot use "=b" for the 2nd output register.
		asm volatile
			(
			"pushl %%ebx\n\t"
			"movl $7,%%eax\n\t"
			"movl $0,%%ecx\n\t"
			"cpuid\n\t"
			"movl %%ebx,%1\n\t"
			"popl %%ebx\n\t"
			: "=a"(cpuid_data[0]), "=r"(cpuid_data[1]), "=c"(cpuid_data[2]), "=d"(cpuid_data[3])
			:
		: "cc"
			);
#endif
#endif

		if( f.x86_family >= 6 )
		{
			f.have[CV_CPU_AVX2] = (cpuid_data[1] & (1<<5)) != 0;
		}

		return f;
	}

	int x86_family;
	bool have[MAX_FEATURE+1];
};

static HWFeatures  featuresEnabled = HWFeatures::initialize(), featuresDisabled = HWFeatures();
static HWFeatures* currentFeatures = &featuresEnabled;

inline bool CheckHardwareSupport(int feature)
{
	assert( 0 <= feature && feature <= CV_HARDWARE_MAX_FEATURE );
	return currentFeatures->have[feature];
}

/************************************************************************/
/* Smooth.cpp(1305-1437行)                                              */
/************************************************************************/

struct MinMax8u
{
	typedef uchar value_type;
	typedef int arg_type;
	enum { SIZE = 1 };
	arg_type load(const uchar* ptr) { return *ptr; }
	void store(uchar* ptr, arg_type val) { *ptr = (uchar)val; }
	void operator()(arg_type& a, arg_type& b) const
	{
		int t = CV_FAST_CAST_8U(a - b);
		b += t; a -= t;
	}
};

struct MinMax16u
{
	typedef ushort value_type;
	typedef int arg_type;
	enum { SIZE = 1 };
	arg_type load(const ushort* ptr) { return *ptr; }
	void store(ushort* ptr, arg_type val) { *ptr = (ushort)val; }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = min(a, b);
		b = max(b, t);
	}
};

struct MinMax16s
{
	typedef short value_type;
	typedef int arg_type;
	enum { SIZE = 1 };
	arg_type load(const short* ptr) { return *ptr; }
	void store(short* ptr, arg_type val) { *ptr = (short)val; }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = min(a, b);
		b = max(b, t);
	}
};

struct MinMax32f
{
	typedef float value_type;
	typedef float arg_type;
	enum { SIZE = 1 };
	arg_type load(const float* ptr) { return *ptr; }
	void store(float* ptr, arg_type val) { *ptr = val; }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = min(a, b);
		b = max(b, t);
	}
};

#if CV_SSE2

struct MinMaxVec8u
{
	typedef uchar value_type;
	typedef __m128i arg_type;
	enum { SIZE = 16 };
	arg_type load(const uchar* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
	void store(uchar* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = _mm_min_epu8(a, b);
		b = _mm_max_epu8(b, t);
	}
};


struct MinMaxVec16u
{
	typedef ushort value_type;
	typedef __m128i arg_type;
	enum { SIZE = 8 };
	arg_type load(const ushort* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
	void store(ushort* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = _mm_subs_epu16(a, b);
		a = _mm_subs_epu16(a, t);
		b = _mm_adds_epu16(b, t);
	}
};


struct MinMaxVec16s
{
	typedef short value_type;
	typedef __m128i arg_type;
	enum { SIZE = 8 };
	arg_type load(const short* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
	void store(short* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = _mm_min_epi16(a, b);
		b = _mm_max_epi16(b, t);
	}
};


struct MinMaxVec32f
{
	typedef float value_type;
	typedef __m128 arg_type;
	enum { SIZE = 4 };
	arg_type load(const float* ptr) { return _mm_loadu_ps(ptr); }
	void store(float* ptr, arg_type val) { _mm_storeu_ps(ptr, val); }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = _mm_min_ps(a, b);
		b = _mm_max_ps(b, t);
	}
};


#else

typedef MinMax8u MinMaxVec8u;
typedef MinMax16u MinMaxVec16u;
typedef MinMax16s MinMaxVec16s;
typedef MinMax32f MinMaxVec32f;

#endif

/************************************************************************/
/* 中值滤波(滤波核大小为3 X 3)											*/
/* smooth.cpp(参考了1439-1712行)										*/
/************************************************************************/

template<class Op, class VecOp>
static void
	medianBlur_SortNet( uchar *pSrc, int nWidth, int nHeight, int nStep, int nChannel)
{
	typedef typename Op::value_type T;
	typedef typename Op::arg_type WT;
	typedef typename VecOp::arg_type VT;

	const T* src = (const T*)pSrc;
	uchar *pDst = new uchar[nHeight * nStep];
	T* dst = (T*)pDst;
	int sstep = (int)(nStep/sizeof(T));
	int dstep = (int)(nStep/sizeof(T));
	int i, j, cn = nChannel;
	Op op;
	VecOp vop;
	volatile bool useSIMD = CheckHardwareSupport(CV_CPU_SSE2);

	if( nWidth == 1 || nHeight == 1 )
	{
		int len = nWidth + nHeight - 1;
		int sdelta = nHeight == 1 ? cn : sstep;
		int sdelta0 = nHeight == 1 ? 0 : sstep - cn;
		int ddelta = nHeight == 1 ? cn : dstep;

		for( i = 0; i < len; i++, src += sdelta0, dst += ddelta )
			for( j = 0; j < cn; j++, src++ )
			{
				WT p0 = src[i > 0 ? -sdelta : 0];
				WT p1 = src[0];
				WT p2 = src[i < len - 1 ? sdelta : 0];

				op(p0, p1); op(p1, p2); op(p0, p1);
				dst[j] = (T)p1;
			}
			return;
	}

	nWidth *= cn;
	for( i = 0; i < nHeight; i++, dst += dstep )
	{
		const T* row0 = src + max(i - 1, 0)*sstep;
		const T* row1 = src + i*sstep;
		const T* row2 = src + min(i + 1, nHeight-1)*sstep;
		int limit = useSIMD ? cn : nWidth;

		for(j = 0;; )
		{
			for( ; j < limit; j++ )
			{
				int j0 = j >= cn ? j - cn : j;
				int j2 = j < nWidth - cn ? j + cn : j;
				WT p0 = row0[j0], p1 = row0[j], p2 = row0[j2];
				WT p3 = row1[j0], p4 = row1[j], p5 = row1[j2];
				WT p6 = row2[j0], p7 = row2[j], p8 = row2[j2];

				op(p1, p2); op(p4, p5); op(p7, p8); op(p0, p1);
				op(p3, p4); op(p6, p7); op(p1, p2); op(p4, p5);
				op(p7, p8); op(p0, p3); op(p5, p8); op(p4, p7);
				op(p3, p6); op(p1, p4); op(p2, p5); op(p4, p7);
				op(p4, p2); op(p6, p4); op(p4, p2);
				dst[j] = (T)p4;
			}

			if( limit == nWidth )
				break;

			for( ; j <= nWidth - VecOp::SIZE - cn; j += VecOp::SIZE )
			{
				VT p0 = vop.load(row0+j-cn), p1 = vop.load(row0+j), p2 = vop.load(row0+j+cn);
				VT p3 = vop.load(row1+j-cn), p4 = vop.load(row1+j), p5 = vop.load(row1+j+cn);
				VT p6 = vop.load(row2+j-cn), p7 = vop.load(row2+j), p8 = vop.load(row2+j+cn);

				vop(p1, p2); vop(p4, p5); vop(p7, p8); vop(p0, p1);
				vop(p3, p4); vop(p6, p7); vop(p1, p2); vop(p4, p5);
				vop(p7, p8); vop(p0, p3); vop(p5, p8); vop(p4, p7);
				vop(p3, p6); vop(p1, p4); vop(p2, p5); vop(p4, p7);
				vop(p4, p2); vop(p6, p4); vop(p4, p2);
				vop.store(dst+j, p4);
			}

			limit = nWidth;
		}
	}
	memcpy(pSrc, pDst, nHeight * nStep);
	SAFE_DELETE(pDst);
}