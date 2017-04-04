#pragma once

/**
* @file templateFuncs.h
* @brief 函数模板
*/

#include "DataTypes.h"

// 获取坐标(x, y)处的值
template <typename Type> inline float GetPositionValue(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, int x, int y);

// 插值出浮点(x, y)处的值,双线性插值
template <typename Type> float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, float x, float y);

// 从图像获取数据流用于解码
template <typename Type> char* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 将Byte(uchar)数据拷贝到浮点数
template <typename Type> void MemcpyByte2Float(float* pDst, const Type* pSrc, int nWidth, int nHeight, int nChannel);

// 将浮点数据拷贝到BYTE(uchar)
template <typename Type> void MemcpyFloat2Byte(Type* pDst, const float* pSrc, int nWidth, int nHeight, int nChannel);

// 将宽为nWidth、高为nHeight的向量写入文件(非二进制形式)
template <typename Type> BOOL WriteVector(const char* filePath, const Type* pSrc, int nWidth, int nHeight);

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
template <typename Type> inline float GetPositionValue(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, int x, int y)
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
template <typename Type> float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, float x, float y)
{
	/** 
	// 双线性插值的示意图:
	// Ans2：		(x4, y4)				(x3, y3)
	//							(x, y)
	// Ans1：		(x1, y1)				(x2, y2)
	*/
	int x1, x2, x3, x4, y1, y2, y3, y4;
	float Ans1, Ans2;
	x1 = int(x);	y1 = int(y);
	x2 = x1 + 1;	y2 = y1;
	x3 = x2;		y3 = y1 + 1;
	x4 = x1;		y4 = y3;
	// 对越界的处理
	if (x1 < 0 || x3 >= nWidth || y1 < 0 || y3 >= nHeight)
		return 0.0F;

	Ans1 = *(pSrc + nCurChannel + x1 * nChannel + y1 * nRowlen) * (x2 - x) 
		+ *(pSrc + nCurChannel + x2 * nChannel + y2 * nRowlen) * (x - x1);
	Ans2 = *(pSrc + nCurChannel + x4 * nChannel + y4 * nRowlen) * (x3 - x) 
		+ *(pSrc + nCurChannel + x3 * nChannel + y3 * nRowlen) * (x - x4);
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
			for (int j = 0; j < nWidth; j++)
			{
				pDst[    4 * j + i * NewRowlen] = pHead[j + i * nRowlen];
				pDst[1 + 4 * j + i * NewRowlen] = pHead[j + i * nRowlen];
				pDst[2 + 4 * j + i * NewRowlen] = pHead[j + i * nRowlen];
				pDst[3 + 4 * j + i * NewRowlen] = 0;// Alpha
			}
		}
		break;
	case 24:// RGB
#pragma omp parallel for
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				pDst[    4 * j + i * NewRowlen] = pHead[	3 * j + i * nRowlen];
				pDst[1 + 4 * j + i * NewRowlen] = pHead[1 + 3 * j + i * nRowlen];
				pDst[2 + 4 * j + i * NewRowlen] = pHead[2 + 3 * j + i * nRowlen];
				pDst[3 + 4 * j + i * NewRowlen] = 0;// Alpha
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
		for (int j = 0; j < nHeight; ++j)
		{
			for (int k = 0; k < nChannel; ++k)
				pDst[k + i * nChannel + j * nRowlen] = (Type)pSrc[k + i * nChannel + j * nFloatRowlen];
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
		for (int j = 0; j < nHeight; ++j)
		{
			for (int k = 0; k < nChannel; ++k)
				pDst[k + i * nChannel + j * nFloatRowlen] = (float)pSrc[k + i * nChannel + j * nRowlen];
		}
	}
}


/** 
* @brief 将向量写入文件
@ details 将宽为nWidth、高为nHeight的向量以浮点形式写入文件
* @param[in] *filePath		文件名
* @param[in] *pSrc			图像数据
* @param[in] nWidth			图像宽度
* @param[in] nHeight		图像高度
* @return 成功或失败
* @note 写入文件后的数据可以直接被MATLAB读取，方法：
		Mat = load('test.txt');
		该函数的目的是用 MATLAB 检测数据是否正确
*/
template <typename Type> BOOL WriteVector(const char* filePath, const Type* pSrc, int nWidth, int nHeight)
{
	ofstream InputFile(filePath);
	if (InputFile)
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth - 1; j++)
			{
				InputFile << (float)pSrc[j + i * nWidth] << ",";
			}
			// 每行最后一个数据
			InputFile << (float)pSrc[nWidth - 1 + i * nWidth] << endl;
		}
		InputFile.close();
		return TRUE;
	}
	return FALSE;
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