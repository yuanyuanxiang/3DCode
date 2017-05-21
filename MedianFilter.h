#include "DataTypes.h"

#pragma once

/** 
* @file MedianFilter.h
* @brief  中值滤波
* @details 包含：中值滤波、自适应中值滤波函数
* @see http://blog.csdn.net/mfcing/article/details/43817715
*/

// 中值滤波
template <typename Type> BOOL MedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nSize = 3);

// 快速中值滤波
template <typename Type> BOOL FastMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nSize = 3);

// 自适应中值滤波
template <typename Type> BOOL SapMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nMaxSize = 7);

template <typename Type> int SapGetMinMedMax(Type* pData, int nWidth, int nHeight, int nRowlen, int nChannel,
	Type *pCopy, int r, int c, rgb<Type> *temp, int nSize, int nMaxSize);

//////////////////////////////////////////////////////////////////////////

/** 
* @brief 中值滤波.
* @details 中值滤波比较耗时.
* @param[in] *pData 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nSize 滤波核尺寸
* @warning 仅限彩色图像
*/
template <typename Type> BOOL MedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nSize)
{
	// 滤波核尺寸必须为奇数
	ASSERT(nSize & 0x00000001);
	Type *pCopy = new Type[nHeight * nRowlen];
	memcpy(pCopy, pData, nHeight * nRowlen * sizeof(Type));
	const int nChannel = nRowlen / nWidth;
	ASSERT(nChannel >= 3);
	const int K = nSize >> 1;// 滤波核半径
	rgb<Type> *temp = new rgb<Type>[nSize * nSize];// 存放领域像素
	
	// 逐个像素进行滤波(边界不处理，这样便不需要作越界判断)
	// 对于滤波核为3的滤波器，循环体乘法个数不少于27
	for (int r = K; r < nHeight - K; ++r)
	{
		for (int c = K; c < nWidth - K; ++c)
		{
			int rgbCount = 0;// 当前像素(r, c)的领域像素个数
			for (int r0 = r - K; r0 <= r + K; ++r0)
			{
				int y = r0 * nRowlen;
				for (int c0 = c - K; c0 <= c + K; ++c0)
				{
					// (r0, c0)像素
					Type* Pixel = pData + y + c0 * nChannel;
					temp[rgbCount].b = *Pixel++;
					temp[rgbCount].g = *Pixel++;
					temp[rgbCount].r = *Pixel++;
					temp[rgbCount].Init();
					if (rgbCount > 0)
					{
						// 从小到大插入排序
						for (int j = rgbCount; j > 0 && temp[j] < temp[j-1]; --j)
						{
							temp[j].Swap(temp[j-1]);
						}
					}
					++rgbCount;
				}
			}
			ASSERT(rgbCount == nSize * nSize);
			rgbCount >>= 1;// 除2,取中值
			Type* Pixel = pCopy + r*nRowlen + c*nChannel;
			*Pixel++ = temp[rgbCount].b;
			*Pixel++ = temp[rgbCount].g;
			*Pixel++ = temp[rgbCount].r;
		}
	}
	SAFE_DELETE(temp);
	memcpy(pData, pCopy, nHeight * nRowlen * sizeof(Type));
	SAFE_DELETE(pCopy);
	return TRUE;
}


/** 
* @brief 快速中值滤波.
* @details 中值滤波比较耗时.
* @param[in] *pData 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nSize 滤波核尺寸
* @warning 仅限彩色图像. 当图像通道不等于4或滤波核尺寸大于3时调用MedianFilter.
* @see http://wenda.chinabaike.com/z/shenghuo/20131226/725816.html
*/
template <typename Type> BOOL FastMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nSize)
{
	const int nChannel = nRowlen / nWidth;
	ASSERT(nChannel >= 3);
	if (nChannel == 3 || nSize > 3)
		return MedianFilter(pData, nWidth, nHeight, nRowlen, nSize);
	// 针对4通道图像进行快速滤波
	const int C = 4; // 通道数
	Type *pCopy = new Type[nHeight * nRowlen];
	memcpy(pCopy, pData, nHeight * nRowlen * sizeof(Type));
	const int K = 1; // 滤波核半径

	// 指向整型数据的指针
	typedef unsigned* pINT;

	// 将ia,ib排序为 ia < ib
#define Minmax(ia, ib) if( ia > ib ) {int ic(ia); ia = ib; ib = ic;}

	// 将ia,ib排序为 ia > ib
#define Maxmin(ia, ib) if( ia < ib ) {int ic(ia); ia = ib; ib = ic;}

	// 逐个像素进行滤波(边界不处理，这样便不需要作越界判断)
	int offsety = K * nRowlen + K * C;
	Type *pLine = pData + offsety;
	for (int row = K; row < nHeight - K; ++row)
	{
		int offsetx = offsety; // 偏移量
		Type *pCenter = pLine; // 中心像素
		for (int col = K; col < nWidth - K; ++col)
		{
			Type *pCur = pCenter;
			register unsigned int p0, p1, p2, p3, p4, p5, p6, p7, p8;

			pCur -= nRowlen; // 移动指针
			p0 = *(pINT(pCur - C)); p1 = *(pINT(pCur)); p2 = *(pINT(pCur + C));
			pCur += nRowlen;
			p3 = *(pINT(pCur - C)); p4 = *(pINT(pCur)); p5 = *(pINT(pCur + C));
			pCur += nRowlen;
			p6 = *(pINT(pCur - C)); p7 = *(pINT(pCur)); p8 = *(pINT(pCur + C));

			Minmax(p0, p1); Minmax(p1, p2); // p0<p1 p0<p2 p1<p2：p0<p1<p2
			Minmax(p3, p4); Minmax(p4, p5); // p3<p4 p3<p5 p4<p5：p3<p4<p5
			Minmax(p6, p7); Minmax(p7, p8); // p6<p7 p6<p8 p7<p8：p6<p7<p8
			Maxmin(p6, p0); Maxmin(p6, p3); // p6>p0 p6>p3, 排除p0,p3
			Minmax(p2, p5); Minmax(p2, p8); // p2<p5 p2<p8, 排除p5,p8
			Minmax(p1, p4); Minmax(p4, p7); // p1<p4 p1<p7 p4<p7：p1<p4<p7, 排除p1,p7
			Minmax(p2, p4); Minmax(p4, p6); // p2<p4 p2<p6 p4<p6：p2<p4<p6, 排除p2,p6

			memcpy(pCopy + offsetx, &p4, C);
			offsetx += C;
			pCenter += C;
		}
		offsety += nRowlen;
		pLine += nRowlen;
	}
	memcpy(pData, pCopy, nHeight * nRowlen * sizeof(Type));
	SAFE_DELETE(pCopy);
	return TRUE;
}


/** 
* @brief 自适应中值滤波.
* @details 中值滤波比较耗时.
* @param[in] *pData 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nMaxSize 最大滤波核尺寸
* @warning 仅限彩色图像
* @note 该方法不如中值滤波理想.
*/
template <typename Type> BOOL SapMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nMaxSize)
{
	Type *pCopy = new Type[nHeight * nRowlen];
	memcpy(pCopy, pData, nHeight * nRowlen * sizeof(Type));
	const int nChannel = nRowlen / nWidth;
	ASSERT(nChannel >= 3);
	// 逐个像素进行滤波
	const int nSize = 3;// 起始滤波核尺寸
	rgb<Type> *temp = new rgb<Type>[nMaxSize * nMaxSize];// 存放领域像素
														 // 逐个像素进行滤波处理
	for (int r = 0; r < nHeight; ++r)
	{
		for (int c = 0; c < nWidth; ++c)
		{
			SapGetMinMedMax(pData, nWidth, nHeight, nRowlen, nChannel, pCopy, r, c, temp, nSize, nMaxSize);
		}
	}
	SAFE_DELETE(temp);
	memcpy(pData, pCopy, nHeight * nRowlen * sizeof(Type));
	SAFE_DELETE(pCopy);
	return TRUE;
}


/** 
* @brief 获取最小值、中值、最大值.
* @details 自适应中值滤波原理：
* (1) 计算Zmin,Zmed,Zmax,若Zmed属于(Zmin, Zmax),转(2),否则增大滤波核重复(1),若尺寸超界,输出Z;
* (2) 若Z属于(Zmin, Zmax),不修改Z,否则令Z = Zmed
* @param[in] *pData 原始图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nChannel 图像每像素字节
* @param[in] *pCopy 图像拷贝
* @param[in] r 当前滤波像素（行）
* @param[in] c 当前滤波像素（列）
* @param[in] *temp 像素领域像素数组
* @param[in] nSize 当前滤波核尺寸
* @param[in] nMaxSize 滤波核最大尺寸
*/
template <typename Type> int SapGetMinMedMax(Type* pData, int nWidth, int nHeight, int nRowlen, int nChannel,
	Type *pCopy, int r, int c, rgb<Type> *temp, int nSize, int nMaxSize)
{
	const int K = nSize >> 1;// 滤波核半径
	int rgbCount = 0;  // 当前像素(r, c)的领域像素个数
	for (int r0 = r - K; r0 <= r + K; ++r0)
	{
		if (0 <= r0 && r0 < nHeight)
		{
			for (int c0 = c - K; c0 <= c + K; ++c0)
			{
				if (0 <= c0 && c0 < nWidth)
				{
					// (r0, c0)像素
					Type* Pixel = pData + r0*nRowlen + c0*nChannel;
					// 保存该像素
					temp[rgbCount].b = *Pixel++;
					temp[rgbCount].g = *Pixel++;
					temp[rgbCount].r = *Pixel++;
					temp[rgbCount].Init();
					++rgbCount;
				}
			}
		}
	}
	rgb<Type> *Min = NULL, *Med = NULL, *Max = NULL;
	if (rgbCount)
	{
		// 插入排序
		Sort(temp, rgbCount);
		Min = &temp[0];// min
		Med = &temp[rgbCount >> 1];// med
		Max = &temp[rgbCount - 1];// max
	}
	Type* Pixel = pCopy + r*nRowlen + c*nChannel;
	rgb<Type> cur(*Pixel, *(Pixel + 1), *(Pixel + 2));// 当前像素
	int nMin = Min->Init();
	int nMed = Med->Init();
	int nMax = Max->Init();
	int nCur = cur.Init();
	if (nMin < nMed && nMed < nMax)
	{
		if (nMin < nCur && nCur < nMax)
		{
			// 不修改pCopy
			return 1;
		}
		else
		{
			// 取中值
			*Pixel++ = Med->b;
			*Pixel++ = Med->g;
			*Pixel++ = Med->r;
			return 2;
		}
	}
	else
	{
		// 滤波核大小超界，退出; 或者增大滤波核进行处理
		return nSize >= nMaxSize ? 0 : 
			SapGetMinMedMax(pData, nWidth, nHeight, nRowlen, nChannel, pCopy, r, c, temp, nSize + 2, nMaxSize);
	}
}