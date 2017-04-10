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
template <typename Type> char* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 将Byte(uchar)数据拷贝到浮点数
template <typename Type> void MemcpyByte2Float(float* pDst, const Type* pSrc, int nWidth, int nHeight, int nChannel);

// 将浮点数据拷贝到BYTE(uchar)
template <typename Type> void MemcpyFloat2Byte(Type* pDst, const float* pSrc, int nWidth, int nHeight, int nChannel);

// 计算数据块的最大最小值
template <typename Type> void MinMax(const Type* pSrc, int nWidth, int nHeight, Type &Min, Type &Max);


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
template <typename Type> float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
											  int nCurChannel, float x, float y)
{
	/** 
	// 双线性插值的示意图:
	// Ans2：		(x4, y4)				(x3, y3)
	//							(x, y)
	// Ans1：		(x1, y1)				(x2, y2)
	*/
	int x1, x3, y1, y3;
	x1 = int(x);	y1 = int(y);
	x3 = x1 + 1;	y3 = y1 + 1;
	// 对越界的处理
	if (x1 < 0 || x3 >= nWidth || y1 < 0 || y3 >= nHeight)
		return 0.0F;

	float Ans1, Ans2;
	// 左下角的点
	const Type* pLB = pSrc + nCurChannel + x1 * nChannel + y1 * nRowlen;
	Ans1 = * pLB * (x3 - x) + *(pLB + nChannel) * (x - x1);
	Ans2 = *(pLB + nRowlen) * (x3 - x) + *(pLB + nChannel + nRowlen) * (x - x1);
	return (Ans1 * (y3 - y) + Ans2 * (y - y1));
}


/**
* @brief 获得解码数据流
* @param[in] *pHead			图像数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] nRowlen		图像每行字节数
@return 4通道的图像数据(char)
@note Type可以为char或BYTE
*/
template <typename Type> char* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	ASSERT(sizeof(Type) == 1);
	// 输入图像的像素位数
	const int nBPP = nRowlen / nWidth * 8;
	// 4通道图像的每行字节数
	const int NewRowlen = nWidth * 4;
	// 为返回值分配内存
	char *pDst = new char[nHeight * NewRowlen];
	switch (nBPP)
	{
	case 8:// GRAY
#pragma omp parallel for
		for (int i = 0; i < nHeight; i++)
		{
			const Type* y1 = pHead + i * nRowlen;
			char* y2 = pDst + i * NewRowlen;
			for (int j = 0; j < nWidth; j++)
			{
				const Type* pSrc = y1 + j;
				char* pCur = y2 + 4 * j;
				* pCur = *(pCur+1) = *(pCur+2) = *pSrc;
				*(pCur+3) = 0;// Alpha
			}
		}
		break;
	case 24:// RGB
#pragma omp parallel for
		for (int i = 0; i < nHeight; i++)
		{
			const Type* y1 = pHead + i * nRowlen;
			char* y2 = pDst + i * NewRowlen;
			for (int j = 0; j < nWidth; j++)
			{
				const Type* pSrc = y1 + 3 * j;
				char* pCur = y2 + 4 * j;
				* pCur = *pSrc;
				*(pCur+1) = *(pSrc+1);
				*(pCur+2) = *(pSrc+2);
				*(pCur+3) = 0;// Alpha
			}
		}
		break;
	case 32:// RGBA
		memcpy(pDst, pHead, nHeight * NewRowlen * sizeof(char));
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

#pragma omp parallel for
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

#pragma omp parallel for
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


/** 
* @brief 计算数据块的最大最小值
* @param[in] *pSrc			图像数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @param[in] &Min			最小值
* @param[in] &Max			最大值
* @return 通过Min、Max输出最小最大值
*/
template <typename Type> void MinMax(const Type* pSrc, int nWidth, int nHeight, Type &Min, Type &Max)
{
	// 寻找最大最小值
	pair<Type*, Type*> minmax_pair = minmax_element(pSrc, pSrc + nWidth * nHeight);
	Min = *minmax_pair.first;
	Max = *minmax_pair.second;
}