#pragma once

/**
* @file templateFuncs.h
* @brief 函数模板
* @details 主要包括：1、图像双线性插值函数；2、从图像获取解码流；
*		3、字节流和浮点数据相互拷贝；4、求解数据块的最大最小值。
*/

#include "DataTypes.h"

// 获取坐标(x, y)处的值
template <typename Type> inline float GetPositionValue(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
													   int nCurChannel, int x, int y);

// 插值出浮点(x, y)处的值,双线性插值
template <typename Type> float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
											  int nCurChannel, float x, float y);

// 从图像获取数据流用于解码
template <typename Type> Type* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 将Byte(uchar)数据拷贝到浮点数
template <typename Type> void MemcpyByte2Float(float* pDst, const Type* pSrc, int nWidth, int nHeight, int nChannel);

// 将浮点数据拷贝到BYTE(uchar)
template <typename Type> void MemcpyFloat2Byte(Type* pDst, const float* pSrc, int nWidth, int nHeight, int nChannel);


/** 
* @brief 获取整点(x, y)处的函数值
* @param[in] *pSrc			图像数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] nRowlen		图像每行字节数
* @param[in] nChannel		图像通道
* @param[in] nCurChannel	当前通道
* @param[in] x				插值坐标
* @param[in] y				插值坐标
*/
template <typename Type> inline float GetPositionValue(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
													   int nCurChannel, int x, int y)
{
	if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
		return 0.0F;

	return float(*(pSrc + nCurChannel + x * nChannel + y * nRowlen));
}


/** 
* @brief 双线性插值获取浮点(x, y) 处的函数值
* @param[in] *pSrc			图像数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] nRowlen		图像每行字节数
* @param[in] nChannel		图像通道
* @param[in] nCurChannel	当前通道
* @param[in] x				插值坐标
* @param[in] y				插值坐标
*/
template <typename Type> inline float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
											  int nCurChannel, float x, float y)
{
	/** 
	// 双线性插值的示意图:
	// Ans2：		(x4, y4)				(x3, y3)
	//							(x, y)
	// Ans1：		(x1, y1)				(x2, y2)
	*/
	int x1 = int(x), y1 = int(y), x3 = x1 + 1, y3 = y1 + 1;
	// 左下角的点
	const Type* pLB = pSrc + nCurChannel + x1 * nChannel + y1 * nRowlen;
	// 对越界的处理
	return (x1 < 0 || x3 >= nWidth || y1 < 0 || y3 >= nHeight) ? 0 : 
		(*pLB * (x3 - x) + *(pLB + nChannel) * (x - x1)) * (y3 - y) 
		+ (*(pLB + nRowlen) * (x3 - x) + *(pLB + nChannel + nRowlen) * (x - x1)) * (y - y1);
}


/**
* @brief 获得解码数据流
* @param[in] *pHead			图像数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] nRowlen		图像每行字节数
@return 4通道的图像数据(请将其转为char再用于解码)
@note Type可以为char或BYTE
*/
template <typename Type> Type* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	ASSERT(sizeof(Type) == 1);
	// 输入图像的像素位数
	const int nBPP = nRowlen / nWidth * 8;
	// 4通道图像的每行字节数
	const int NewRowlen = nWidth * 4;
	// 为返回值分配内存
	Type *pDst = new Type[nHeight * NewRowlen];
	switch (nBPP)
	{
	case 8:// GRAY
		{
			const Type* pSrc0 = pHead;
			Type* pCur0 = pDst;
			for (int i = 0; i < nHeight; i++, pSrc0 += nRowlen, pCur0 += NewRowlen)
			{
				const Type* pSrc = pSrc0;
				Type* pCur = pCur0;
				for (int j = 0; j < nWidth; j++, pSrc ++ , pCur += 4)
				{
					memset(pCur, *pSrc, 3 * sizeof(Type));
					*(pCur+3) = 0;// Alpha
				}
			}
		}
		break;
	case 24:// RGB
		{
			const Type* pSrc0 = pHead;
			Type* pCur0 = pDst;
			for (int i = 0; i < nHeight; i++, pSrc0 += nRowlen, pCur0 += NewRowlen)
			{
				const Type* pSrc = pSrc0;
				Type* pCur = pCur0;
				for (int j = 0; j < nWidth; j++, pSrc += 3, pCur += 4)
				{
					memcpy(pCur, pSrc, 3 * sizeof(Type));
					*(pCur+3) = 0;// Alpha
				}
			}
		}
		break;
	case 32:// RGBA
		memcpy(pDst, pHead, nHeight * NewRowlen * sizeof(Type));
		break;
	default:
		SAFE_DELETE(pDst);
		return NULL;
	}
	return pDst;
}


/** 
* @brief 将浮点数据拷贝到BYTE. 
* @param[in] *pDst			字节数据
* @param[in] *pSrc			浮点数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] nChannel		图像通道
*/
template <typename Type> void MemcpyFloat2Byte(Type* pDst, const float* pSrc, int nWidth, int nHeight, int nChannel)
{
	// BYTE每行字节数
	const int nRowlen = WIDTHBYTES(nWidth * nChannel * 8);
	// 浮点数据每行个数
	const int nFloatRowlen = nWidth * nChannel;

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y1 = j * nFloatRowlen, y2 = j * nRowlen;
			for (int k = 0; k < nChannel; ++k)
				pDst[k + x + y2] = (Type)pSrc[k + x + y1];
		}
	}
}


/** 
* @brief 将BYTE拷贝到浮点数据
* @param[in] *pDst			浮点数据
* @param[in] *pSrc			字节数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] nChannel		图像通道
*/
template <typename Type> void MemcpyByte2Float(float* pDst, const Type* pSrc, int nWidth, int nHeight, int nChannel)
{
	// BYTE数据每行字节数
	const int nRowlen = WIDTHBYTES(nWidth * nChannel * 8);
	// 浮点数据每行个数
	const int nFloatRowlen = nWidth * nChannel;

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y1 = j * nRowlen, y2 = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
				pDst[k + x + y2] = (float)pSrc[k + x + y1];
		}
	}
}