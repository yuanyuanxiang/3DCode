#pragma once

#ifndef DATATYPES_H
	#define DATATYPES_H		// 包含了基本数据结构
#endif

/** 
* @file DataTypes.h
* @brief 本文档定义了程序可能用到的数据类型
* @date 2016/8/24 注
*/

#ifndef _MSC_VER
#define ANDROID_CPP
#define LINUX_CPP
#endif

#include <math.h>
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned char uchar;
typedef BYTE *LPBYTE;
typedef unsigned int uint;
#define MAX_MODULESIZE		177		// QR码最大尺寸
typedef BYTE qrMat[MAX_MODULESIZE];	// QR码比特矩阵

#define COLORED_QR 1 // 是否彩色QR码

#define TRY_HARD 0 // 某些很难解析的二维码需要开启此开关

#if defined(ANDROID_CPP)

#include <stdio.h>

#ifndef NO_ICONV
#define NO_ICONV
#endif

#ifndef ASSERT 
#include <assert.h>
#define ASSERT assert
#endif

#endif

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))					/**< 最大值 */
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))					/**< 最小值 */
#endif

#define MAX_CODEUTF8NUM		1024										/**< 最多编码utf8文字字数 */

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)					/**< 计算每行字节数(4字节对齐) */

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }/**< 安全删除指针p  */
#endif

#ifndef RGB
/// RGB
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

#ifndef ZeroMemory
/// 置零
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif

#define PI 3.141592653f /**< PI */
#define QR_MARGIN	4	/**< QR边界 */

#ifdef ANDROID_CPP		/**< 如果在安装下编译，需要包含某些文件 */

#include "stdlib.h"
#include "string.h"

#define TRACE printf

typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long		COLORREF;
typedef unsigned long		ULONG_PTR;
typedef ULONG_PTR			DWORD_PTR;
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))

#endif // ANDROID_CPP

/** 
* @class float2
* @brief 二维浮点坐标(x, y)
*/
class float2
{
public:
	float x, y;
public:
	float2() : x(0), y(0)
	{
	}
	float2(float a) : x(a), y(a)
	{
	}
	float2(float a, float b) : x(a), y(b)
	{
	}
	inline operator float() { return sqrt(x * x + y * y); }
	// =
	float2 operator = (const float2& point)
	{
		x = point.x;
		y = point.y;
		return *this;
	}
	float2 operator = (const float val)
	{
		x = val;
		y = val;
		return *this;
	}
	// -
	float2 operator - ()
	{
		x = -x;
		y = -y;
		return *this;
	}
	// ==
	bool operator == (const float2 & point)
	{
		return (x == point.x && y == point.y);
	}
	bool operator == (const float val)
	{
		return (x == val && y == val);
	}
	// !=
	bool operator != (const float2 & point)
	{
		return (x != point.x || y != point.y);
	}
	bool operator != (const float val)
	{
		return (x != val || y != val);
	}
	// += -= *= /=
	float2 operator += (const float2 & pos)
	{
		x += pos.x;
		y += pos.y;
		return *this;
	}
	float2 operator -= (const float2 & pos)
	{
		x -= pos.x;
		y -= pos.y;
		return *this;
	}
	float2 operator *= (const float2 & pos)
	{
		x *= pos.x;
		y *= pos.y;
		return *this;
	}
	float2 operator /= (const float2 & pos)
	{
		x /= pos.x;
		y /= pos.y;
		return *this;
	}
	float2 operator += (const float val)
	{
		x += val;
		y += val;
		return *this;
	}
	float2 operator -= (const float val)
	{
		x -= val;
		y -= val;
		return *this;
	}
	float2 operator *= (const float val)
	{
		x *= val;
		y *= val;
		return *this;
	}
	float2 operator /= (const float val)
	{
		x /= val;
		y /= val;
		return *this;
	}
	// 浮点运算(1)
	friend float2 operator + (const float2 & pos, const float val)
	{
		return float2(pos.x + val, pos.y + val);
	}
	friend float2 operator - (const float2 & pos, const float val)
	{
		return float2(pos.x - val, pos.y - val);
	}
	friend float2 operator * (const float2 & pos, const float val)
	{
		return float2(pos.x * val, pos.y * val);
	}
	friend float2 operator / (const float2 & pos, const float val)
	{
		return float2(pos.x / val, pos.y / val);
	}
	// 浮点运算(2)
	friend float2 operator + (const float val, const float2 & pos)
	{
		return float2(val + pos.x, val + pos.y);
	}
	friend float2 operator - (const float val, const float2 & pos)
	{
		return float2(val - pos.x, val - pos.y);
	}
	friend float2 operator * (const float val, const float2 & pos)
	{
		return float2(val * pos.x, val * pos.y);
	}
	friend float2 operator / (const float val, const float2 & pos)
	{
		return float2(val / pos.x, val / pos.y);
	}
	// 四则运算
	friend float2 operator + (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x + pos2.x, pos1.y + pos2.y);
	}
	friend float2 operator - (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x - pos2.x, pos1.y - pos2.y);
	}
	friend float2 operator * (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x * pos2.x, pos1.y * pos2.y);
	}
	friend float2 operator / (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x / pos2.x, pos1.y / pos2.y);
	}
	friend float2 abs(const float2 & point)
	{
		return float2(abs(point.x), abs(point.y));
	}
};


/** 
* @class float3
* @brief 三维浮点坐标(x, y. z)
*/
class float3
{
public:
	float x, y, z;
public:
	float3() : x(0), y(0), z(0)
	{
	}
	float3(float a, float b, float c) : x(a), y(b), z(c)
	{
	}
	float3(const float3 & other) : x(other.x), y(other.y), z(other.z)
	{
	}
	float3(COLORREF ref) : x(GetRValue(ref)), y(GetGValue(ref)), z(GetBValue(ref))
	{
	}
	// 转化为灰度值
	inline float ToGray() { return 0.299f * x + 0.587f * y + 0.114f * z; }
	// 强制转换函数,同ToGray
	inline operator float() { return 0.299f * x + 0.587f * y + 0.114f * z; }
	// =
	float3 operator = (const float3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	float3 operator = (const float val)
	{
		x = val;
		y = val;
		z = val;
		return *this;
	}
	// -
	float3 operator - ()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	// ==
	bool operator == (const float3& other)
	{
		return (x == other.x && y == other.y && z == other.z);
	}
	bool operator == (const float val)
	{
		return (x == val && y == val && z == val);
	}
	// !=
	bool operator != (const float3& other)
	{
		return (x != other.x || y != other.y || z != other.z);
	}
	bool operator != (const float val)
	{
		return (x != val || y != val || z != val);
	}
	// += -= *= /=
	float3 operator += (const float3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	float3 operator -= (const float3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	float3 operator *= (const float3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}
	float3 operator /= (const float3& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}
	float3 operator += (const float other)
	{
		x += other;
		y += other;
		z += other;
		return *this;
	}
	float3 operator -= (const float other)
	{
		x -= other;
		y -= other;
		z -= other;
		return *this;
	}
	float3 operator *= (const float other)
	{
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}
	float3 operator /= (const float other)
	{
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}
	// 与浮点运算(1)
	friend float3 operator + (const float3 & point, float val)
	{
		return float3(point.x + val, point.y + val, point.z + val);
	}
	friend float3 operator - (const float3 & point, float val)
	{
		return float3(point.x - val, point.y - val, point.z - val);
	}
	friend float3 operator * (const float3 & point, float val)
	{
		return float3(point.x * val, point.y * val, point.z * val);
	}
	friend float3 operator / (const float3 & point, float val)
	{
		return float3(point.x / val, point.y / val, point.z / val);
	}
	// 与浮点运算(2)
	friend float3 operator + (float val, const float3 & point)
	{
		return float3(val + point.x, val + point.y, val + point.z);
	}
	friend float3 operator - (float val, const float3 & point)
	{
		return float3(val - point.x, val - point.y, val - point.z);
	}
	friend float3 operator * (float val, const float3 & point)
	{
		return float3(val * point.x, val * point.y, val * point.z);
	}
	friend float3 operator / (float val, const float3 & point)
	{
		return float3(val / point.x, val / point.y, val / point.z);
	}
	// 四则运算
	friend float3 operator + (const float3 & P, const float3 & Q)
	{
		return float3(P.x + Q.x, P.y + Q.y, P.z + Q.z);
	}
	friend float3 operator - (const float3 & P, const float3 & Q)
	{
		return float3(P.x - Q.x, P.y - Q.y, P.z - Q.z);
	}
	friend float3 operator * (const float3 & P, const float3 & Q)
	{
		return float3(P.x * Q.x, P.y * Q.y, P.z * Q.z);
	}
	friend float3 operator / (const float3 & P, const float3 & Q)
	{
		return float3(P.x / Q.x, P.y / Q.y, P.z / Q.z);
	}
	friend float3 abs(const float3 & point)
	{
		return float3(abs(point.x), abs(point.y), abs(point.z));
	}
};

/// RGB转灰度(@see http://blog.csdn.net/cool1949/article/details/6649429)
#define RGB2GRAY(R, G, B) (  ((R)*38 + (G)*75 + (B)*15) >> 7 )

/// RGB宏：COLORREF转为灰度值
#define RgbColorRef2Gray(ref)			( (GetRValue(ref)*38 + GetGValue(ref)*75 + GetBValue(ref)*15) >> 7 )

#define BIT_SET(a, b) ((a) |= (1<<(b)))			/**< 设置a第b位为1 */
#define BIT_CLEAR(a, b) ((a) &= ~(1<<(b)))		/**< 设置a第b位为0 */
#define BIT_FLIP(a, b) ((a) ^= (1<<(b)))		/**< 开关a的b位 */
#define BIT_CHECK(a, b) ((a) & (1<<(b)))		/**< 检查a的b位 */

/** 
* @brief 交换两个整数
* @note 采用异或速度慢
* @see http://blog.csdn.net/anchor89/article/details/5979290
*/
template <class T> inline void SWAP(T & a, T & b)
{
	T c(a); a = b; b = c;
}

/// 如果a的i位为1，则设置b的j位为1
#define BIT_CHECK_SET(a, i, b, j) if(BIT_CHECK(a, i)) BIT_SET(b, j)

/// 随机设置b的j位为1，使1以概率p出现
#define BIT_RANDOM_SET(p, b, j) if(rand() <= p * RAND_MAX) BIT_SET(b, j)

/** 
* @class CMyRect
* @brief 矩形类
*/
class CMyRect
{
public:
	CMyRect() : left(0), top(0), right(0), bottom(0)
	{
	}
	CMyRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b)
	{
	}
	CMyRect(int nC) : left(nC), top(nC), right(nC), bottom(nC)
	{
	}
	~CMyRect()
	{
	}
	inline int Width() const 
	{
		return right - left;
	}
	inline int Height() const 
	{
		return bottom - top;
	}
	bool operator == (const CMyRect & rect) const 
	{
		return (left == rect.left && right == rect.right
			&& top == rect.top && bottom == rect.bottom);
	}
	inline bool IsEmpty() const 
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}
	// 转置
	inline CMyRect t() const
	{
		return CMyRect(top, left, bottom, right);
	}
public:
	int left, top, right, bottom;
};

/// 图标矩形
typedef CMyRect CLogoRect;

/// 感兴趣区域
typedef CMyRect RoiRect;

/** 
* @class CPixelPoint
* @brief 点类
*/
class CPixelPoint
{
public:
	CPixelPoint() : x(0), y(0)
	{
	}
	CPixelPoint(int xPos, int yPos) : x(xPos), y(yPos)
	{
	}
	~CPixelPoint()
	{
	}
	// 根据x坐标比较两个点大小(P1>P2)
	friend bool operator > (const CPixelPoint &P1, const CPixelPoint &P2)
	{
		return (P1.x > P2.x);
	}
public:
	int x, y;
};

/** 
* @class rgb
* @brief 彩色(r, g, b)
*/
template <typename Type>
class rgb
{
public:
	Type r, g, b, a; // a保留，使4字节对齐
	rgb() : r(0), g(0), b(0), a(0), v(0)
	{
	}
	rgb(Type R, Type G, Type B) : r(R), g(G), b(B), a(0)
	{
		v = ToInt();
	}
	rgb(const rgb & in) : r(in.r), g(in.g), b(in.b)
	{
		v = ToInt();
	}
	// 初始化大小度量值(3个乘法)
	inline void Init() { v = ToInt(); }
	// 交换两个rgb像素
	inline void Swap(rgb<Type> & other) 
	{
		SWAP(r, other.r);
		SWAP(g, other.g);
		SWAP(b, other.b);
		SWAP(v, other.v);
	}
	// 比较两个rgb像素的大小
	bool operator < (const rgb<Type> & other)
	{
		return v < other.v;
	}
private:
	int v; // 用于度量大小的值
	// 转换为Int用于比较大小(3个乘法)
	inline int ToInt() const { return 299 * BYTE(r) + 587 * BYTE(g) + 114 * BYTE(b); }

	//////////////////////////////////////////////////////////////////////////
	// friend
	// 对rgb结构进行插入排序
	friend void Sort(rgb<Type> *pArray, int Num)
	{
		for (int i = 1; i < Num; ++i)
		{
			for (int j = i; j > 0 && pArray[j] < pArray[j-1]; --j)
			{
				pArray[j].Swap(pArray[j-1]);
			}
		}
	}
};


/**
* @struct BarCodeInfo
* @brief 二维码信息结构体(6个成员)
* details 纠错等级,二维码版本,掩码版本,字符、字符长度,每模块大小.
*/
struct BarCodeInfo
{
	int m_nEcLevel;				/**< 纠错等级 */
	int m_nVersion;				/**< 二维码版本 */
	int m_nMaskingNo;			/**< 掩码版本 */
	int m_nStrLen;				/**< 字符长度 */
	float m_fModuleSize;		/**< 每模块大小 */
	char m_pData[MAX_CODEUTF8NUM];/**< 二维码数据 */

	BarCodeInfo()
	{
		memset(this, 0, sizeof(BarCodeInfo));
	}
};


/** 
* @brief 判断索引(i, j)不在矩形内
* @see http://www.cnblogs.com/archimedes/p/writing-efficient-c-and-code-optimization.html
*/
inline BOOL IndexNotInRect(int i, int j, const CLogoRect &rect)
{
	return ( (unsigned(i - rect.top) < (unsigned)rect.Height()) 
		&&   (unsigned(j - rect.left) < (unsigned)rect.Width()) ) ? FALSE : TRUE;
}


/// 两点间的距离
inline float Distance(const float2 & P, const float2 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	return sqrtf(dx * dx + dy * dy);
}


/// 两点间的距离
inline float Distance(const float3 & P, const float3 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	float dz = (P.z - Q.z);
	return sqrtf(dx * dx + dy * dy + dz * dz);
}


/// 两点间的距离平方
inline float _Distance(const float2 & P, const float2 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	return dx * dx + dy * dy;
}


/// 两点间的距离平方
inline float _Distance(const float3 & P, const float3 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	float dz = (P.z - Q.z);
	return dx * dx + dy * dy + dz * dz;
}

inline float _Distance(const float & P, const float & Q)
{
	return abs(P - Q);
}

/// 模
inline float fabs(const float2 & pos)
{
	return sqrt(pos.x * pos.x + pos.y * pos.y);
}

/// 模
inline float fabs(const float3 & pos)
{
	return sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}

// 将矩形按倍数放大
void ZoomRect(CLogoRect &rect, float rate = 1.f);

// 按方向(dx, dy)移动矩形
void MoveRect(CLogoRect &rect, float dx = 0.f, float dy = 0.f);

// 对数据进行掩码操作
void XOR(int *nCodes, const int *nMaskingNo, int nLength);

#define INV_SIGN_BIT	0x7fffffff // 用来反转符号位

#define USE_ASM			0			// 是否使用汇编代码

// float快速求绝对值
inline float FastAbs(float fNum)
{
#if USE_ASM
	float fOut;
	__asm
	{
		MOV EAX, fNum;
		AND EAX, INV_SIGN_BIT;
		MOV fOut, EAX;
	}
	return fOut;
#else
	int *temp = (int*)&fNum;
	int out = *temp & INV_SIGN_BIT;
	return *((float*)&out);
#endif
}

// int快速求绝对值
inline int FastAbs(int iNum)
{
#if USE_ASM
	int iOut = 0;
	__asm  
	{  
		MOV EAX, iNum;
		MOV EDX, EAX;
		SAR EDX, 31;
		XOR EAX, EDX;
		SUB EAX, EDX;
		MOV iOut, EAX;
	}  
#else
	int iOut = iNum;
	int temp = iNum;
	temp >>= 31;
	iOut ^= temp;
	iOut -= temp;
#endif
	return iOut;
}
