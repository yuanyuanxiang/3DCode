/************************************************************************
* @file 
* @details 模板函数,可以跨平台.
* @author 
* @email 
* @version V1.0
* @date 2016/10/4
* Copyleft (c), All rights reserved.
* @note 务必添加此包含"algorithm.h"文件，否则在Eclipse遇到
	* error: 'sort' is not a member of 'std'
	* 凡是以**二级指针作为参数，函数将修改此一级指针
************************************************************************/

#include "DataTypes.h"
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace  std;

#pragma once

// 将图像写入文件的文件头大小（字节）
#define HEADER_SIZE 1024

// 获取坐标(x, y)处的值
template <typename Type> inline float GetPositionValue(Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, int x, int y);

// 插值出浮点(x, y)处的值,双线性插值
template <typename Type> float biLinearInterp(Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, float x, float y);

// 放大图像：会修改pfHead及更新图像信息
template <typename Type> void ZoomImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int NewWidth, int NewHeight);

// 限制图像最大边长，当超出时修改大小
template <typename Type> void LimitImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int nMaxSize = 320);

// 上下翻转图像
template <typename Type> void ImageFlipV(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 左右翻转图像
template <typename Type> void ImageFlipH(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 转置图像
template <typename Type> void ImageTranspose(Type** pHead, const int nWidth, const int nHeight, const int nRowlen);

// 提取图像感兴趣区域
template <typename Type> Type* ImageROI(Type* pHead, int &nWidth, int &nHeight, int &nRowlen, RoiRect roi = RoiRect(0, 0, 0, 0));

// 将图像以二进制形式写入文件
template <typename Type> BOOL ImageWrite(const char* fileName, Type* pSrc, int nWidth, int nHeight, int nRowlen);

// 从图像获取数据流用于解码
template <typename Type> char* GetDecodeString(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 将Byte(uchar)数据拷贝到浮点数
template <typename Type> void MemcpyByte2Float(float* pDst, Type* pSrc, int nWidth, int nHeight, int nChannel);

// 将浮点数据拷贝到BYTE(uchar)
template <typename Type> void MemcpyFloat2Byte(Type* pDst, float* pSrc, int nWidth, int nHeight, int nChannel);

// 将宽为nWidth、高为nHeight的向量写入文件(非二进制形式)
template <typename Type> BOOL WriteVector(const char* filePath, Type* pSrc, int nWidth, int nHeight);

// 计算数据块的最大最小值
template <typename Type> void MinMax(Type* pSrc, int nWidth, int nHeight, Type &Min, Type &Max);

/* K_means */

/************************************************************************/
/*
     作者：袁沅祥
	 时间：2015年12月3日
	 邮箱：yuan_yuanxiang@163.com
*/
/************************************************************************/

// 彩色图像转为8位的单通道图像
template <typename Type> Type* Rgb2Gray(Type* pHead, int nWidth, int nHeight, int nRowBytes);

// 对图像进行用K_means聚类作分割
template <typename Type> void K_means(Type* pHead, int nRowBytes, RoiRect roi, int K, float fBackground, int nMaxepoches);

// 计算组间差异(2016/9/19)
template <typename Type> float Difference(Type *Center, int K);

// 初始化聚类中心(2016/9/19)
template <typename Type> void InitializeClusterCenters(float *Center, int K, Type* pHead, int nRowBytes, RoiRect roi);

// 初始化聚类中心(random随机)
template <typename Type> void RandomClusterCenters(float *Center, int K, Type* pHead, int nRowBytes, RoiRect roi);

// 统计灰度图像的直方图
template <typename Type> void ImageHistogram(Type* pHead, int nRowlen, int nHist[256], RoiRect roi);

/************************************************************************/
/*   中值滤波                                                           */
/*   参考：http://blog.csdn.net/mfcing/article/details/43817715         */
/************************************************************************/

// 插入排序
template <typename Type> void InsertionSort(Type *pArray, int Num);

// 中值滤波
template <typename Type> BOOL MedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nSize = 3);

// 快速中值滤波
template <typename Type> BOOL FastMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen);

// 自适应中值滤波
template <typename Type> BOOL SapMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nMaxSize = 7);

template <typename Type> void SapGetMinMedMax(Type* pData, int nWidth, int nHeight, int nRowlen, int nChannel, 
											 Type *pCopy, int r, int c, rgb<Type> *temp, int nSize, int nMaxSize);

/************************************************************************/
/* ********************** 模板函数的实现 ********************************/
/************************************************************************/

/** @brief 上下翻转图像. 
* @param[in] pHead 数据头
* @param[in] nWidth 宽度
* @param[in] nHeight 高度
* @param[in] nRowlen 每行个数
@return void
*/
template <typename Type> void ImageFlipV(Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	// 缓存数据
	Type* temp = new Type[nHeight * nRowlen * sizeof(Type)];
	memcpy(temp, pHead, nHeight * nRowlen * sizeof(Type));

#pragma omp parallel for
	for (int r = 0; r < nHeight; ++r)
	{
		memcpy(pHead + r * nRowlen, temp + (nHeight - 1 - r) * nRowlen, nRowlen * sizeof(Type));
	}
	SAFE_DELETE(temp);
}


/* - 左右翻转图像 - 
Type* pHead：数据头
const int nWidth：宽度
const int nHeight：高度
const int nRowlen：每行个数
*/
template <typename Type> void ImageFlipH(Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	Type* temp = new Type[nHeight * nRowlen];
	memcpy(temp, pHead, nHeight * nRowlen * sizeof(Type));

	int nChannel = nRowlen / nWidth;

#pragma omp parallel for
	for (int r = 0; r < nHeight; ++r)
	{
		for (int c = 0; c < nWidth; ++c)
		{
			memcpy(pHead + c *nChannel + r * nRowlen, temp + (nWidth - 1 - c) * nChannel + r * nRowlen, nChannel * sizeof(Type));
		}
	}
	SAFE_DELETE(temp);
}


/* - 左右翻转图像 - 
Type** pHead：数据头
const int nWidth：宽度
const int nHeight：高度
const int nRowlen：每行个数
* @note 函数将修改图像指针
*/
template <typename Type> void ImageTranspose(Type** pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	// 图像每像素元素个数
	int nChannel = nRowlen / nWidth;
	// 每行元素个数
	int nNewRowlen = nHeight * nChannel;
	// 是否需要进行对齐处理
	if (1 == sizeof(Type))
	{
		nNewRowlen = WIDTHBYTES(nNewRowlen * 8);
	}

	Type *pSrc = *pHead;
	Type *temp = new Type[nWidth * nNewRowlen];
#pragma omp parallel for
	for (int i = 0; i < nWidth; ++i)
	{
		for (int j = 0; j < nHeight; ++j)
		{
			for (int k = 0; k < nChannel; ++k)
			{
				temp[k + j * nChannel + i * nNewRowlen] = pSrc[k + i * nChannel + j * nRowlen];
			}
		}
	}
	delete [] *pHead;
	*pHead = temp;
	temp = NULL;
}


/** 提取图像感兴趣区域
Type* pHead：图像头指针
int &nWidth：图像宽度
int &nHeight：图像高度
int &nRowlen：图像每行元素个数
RoiRect roi：图像感兴趣区域
* @return 函数返回图像感兴趣区域(指针，需要SAFE_DELETE)
*/
template <typename Type> Type* ImageROI(Type* pHead, int &nWidth, int &nHeight, int &nRowlen, RoiRect roi)
{
	/// 如果roi非法
	if (roi.Width() <= 0 || roi.Height() <= 0 || roi.right > nWidth || roi.bottom > nHeight)
	{
		Type* pDst = new Type[nHeight * nRowlen];
		memcpy(pDst, pHead, nHeight * nRowlen * sizeof(Type));
		return pDst;
	}

	int nChannel = nRowlen / nWidth;
	int nNewWidth = roi.Width();
	int nNewHeight = roi.Height();
	int nNewRowlen = nNewWidth * nChannel;
	// 是否需要进行对齐处理
	if (1 == sizeof(Type))
	{
		nNewRowlen = WIDTHBYTES(nNewRowlen * 8);
	}

	Type* pDst = new Type[nNewHeight * nNewRowlen];

#pragma omp parallel for
	for (int i = 0; i < nNewHeight; ++i)
	{
		int nLine = nHeight - roi.top - i - 1;
		memcpy(pDst + i * nNewRowlen, pHead + roi.left * nChannel + nLine * nRowlen, nNewRowlen * sizeof(Type));
	}

	// 更新图像信息
	nWidth = nNewWidth;
	nHeight = nNewHeight;
	nRowlen = nNewRowlen;
	return pDst;
}


/** 将图像以二进制形式写入文件 
const char* fileName:文件名称
Type* pSrc:图像数据源
int nWidth:图像宽度
int nHeight:图像高度
int nRowlen:每行元素个数
参看：FileIO中的WriteTxt/ReadTxt函数
*/
template <typename Type> BOOL ImageWrite(const char* fileName, Type* pSrc, int nWidth, int nHeight, int nRowlen)
{
	ofstream fout;
	fout.open(fileName, ofstream::binary);

	if (fout.is_open())
	{
		try
		{
			// 写文件头（大小HEADER_SIZE）
			char* pHead = "yuanyuanxiang";
			int nLen = strlen(pHead);
			fout.write(pHead, nLen);
			int nChannel = nRowlen / nWidth;
			int nInt = sizeof(int);
			fout.write((char*)&nRowlen, nInt);
			fout.write((char*)&nHeight, nInt);
			fout.write((char*)&nChannel, nInt);
			fout.seekp(HEADER_SIZE);
			// 写图像数据
			fout.write((char*)pSrc, nHeight * nRowlen);
			fout.close();
			return TRUE;
		}
		catch (...)
		{
			fout.close();
			return FALSE;
		}
	}
	return FALSE;
}


/* - 放大图像 -
Type* pfHead：图像头指针
int &nWidth：图像宽度
int &nHeight：图像高度
int &nRowlen：图像每行字节数
int nChannel：图像通道
int NewWidth：图像新宽度
int NewHeight：图像新高度
备注：函数会修改pfHead指针的内容，作为结果
*/
template <typename Type> void ZoomImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int NewWidth, int NewHeight)
{
	if (nWidth == NewWidth && nHeight == NewHeight)
		return;

	int NewRowlen = NewWidth * nChannel;
	// 是否需要进行对齐处理
	if (1 == sizeof(Type))
	{
		NewRowlen = WIDTHBYTES(NewRowlen * 8);
	}

	Type* pDst = new Type[NewRowlen * NewHeight];
	float wRatio = 1.f * nWidth / NewWidth;
	float hRatio = 1.f * nHeight / NewHeight;

#pragma omp parallel for
	for (int i = 0; i < NewWidth; ++i)
	{
		for (int j = 0; j < NewHeight; ++j)
		{
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + i * nChannel + j * NewRowlen] = 
					(Type)biLinearInterp(*pfHead, nWidth, nHeight, nRowlen, nChannel, nCurChannel, i * wRatio, j * hRatio);
		}
	}

	// 修改pHead指向的内容
	delete [] *pfHead;
	*pfHead = pDst;
	pDst = NULL;

	// 更新图像信息
	nWidth = NewWidth;
	nHeight = NewHeight;
	nRowlen = NewRowlen;
}


/* - 限制图像最大边长 -
图像指针：Type** pfHead
图像宽度：int &nWidth
图像高度：int &nHeight
图像每行字节数：int &nRowlen
图像通道：int nChannel
最大边长：int nMaxSize = 320
备注：当图像有一边长度超过限制时进行缩放并更新信息
重要：宽度、高度、每行字节数会被修改
*/
template <typename Type> void LimitImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int nMaxSize)
{
	float rate = 0.0f;
	if (nWidth > nHeight)
	{
		if (nWidth > nMaxSize)
		{
			rate = 1.f * nMaxSize / nWidth;
			ZoomImage(pfHead, nWidth, nHeight, nRowlen, nChannel, nMaxSize, int(rate * nHeight));
		}
	}
	else
	{
		if (nHeight > nMaxSize)
		{
			rate = 1.f * nMaxSize / nHeight;
			ZoomImage(pfHead, nWidth, nHeight, nRowlen, nChannel, int(rate * nWidth), nMaxSize);
		}
	}
}


// 获取整点(x, y)处的函数值
template <typename Type> inline float GetPositionValue(Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, int x, int y)
{
	if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
		return 0.0F;

	return float(*(pSrc + nCurChannel + x * nChannel + y * nRowlen));
}


/* - 双线性插值 -
 * 获取浮点(x, y) 处的函数值
图像信息：Type *pSrc, int nWidth, int nHegiht, int nRowlen, int nChannel
当前通道：int nCurChannel
插值位置：float x, float y
*/
template <typename Type> float biLinearInterp(Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int nCurChannel, float x, float y)
{
	/************************************************************************
	// 双线性插值的示意图:
	// Ans2：		(x4, y4)				(x3, y3)
	//							(x, y)
	// Ans1：		(x1, y1)				(x2, y2)
	************************************************************************/
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


/** - 获得解码数据流 -
Type* pHead				图像指针
const int nWidth		图像宽度
const int nHeight		图像高度
const int nRowlen		每行字节数
@return 4通道的图像数据(char)
@note Type可以为char或BYTE
*/
template <typename Type> char* GetDecodeString(Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	// 输入图像的像素位数
	int nBPP = nRowlen / nWidth * 8;
	// 4通道图像的每行字节数
	int NewRowlen = nWidth * 4;
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


/** 将浮点数据拷贝到BYTE. 
Type* pDst：目的数据
float* pSrc：原始数据
int nWidth：宽度
int nHeight：高度
int nChannel：通道
*/
template <typename Type> void MemcpyFloat2Byte(Type* pDst, float* pSrc, int nWidth, int nHeight, int nChannel)
{
	// BYTE每行字节数
	int nRowlen = WIDTHBYTES(nWidth * nChannel * 8);
	// 浮点数据每行个数
	int nFloatRowlen = nWidth * nChannel;

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


/* - 将BYTE拷贝到浮点数据 -
float* pDst：目的数据
Type* pSrc：原始数据
int nWidth：宽度
int nHeight：高度
int nChannel：通道
*/
template <typename Type> void MemcpyByte2Float(float* pDst, Type* pSrc, int nWidth, int nHeight, int nChannel)
{
	// BYTE数据每行字节数
	int nRowlen = WIDTHBYTES(nWidth * nChannel * 8);
	// 浮点数据每行个数
	int nFloatRowlen = nWidth * nChannel;

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


/* - 将向量写入文件 - 
将宽为nWidth、高为nHeight的向量以浮点形式写入文件
参数：4个
	const char* filePath：被写入的文件名
	Type* pSrc：数据源的头指针
	int nWidth：数据源的宽度(每行元素个数)
	int nHeight：数据源的高度
返回值：成功或失败
备注：写入文件后的数据可以直接被MATLAB读取，方法：
	Mat = load('test.txt');
 * 注：该函数的目的是用 MATLAB 检测数据是否正确
*/
template <typename Type> BOOL WriteVector(const char* filePath, Type* pSrc, int nWidth, int nHeight)
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


/* - 计算数据块的最大最小值 - 
参数：4个
	Type* pSrc：数据源
	int nWidth：数据源的宽度(每行元素个数)
	int nHeight：数据源的高度
	Type &Min：最小值
	Type &Max：最大值
返回值：通过Min、Max输出最小最大值
*/
template <typename Type> void MinMax(Type* pSrc, int nWidth, int nHeight, Type &Min, Type &Max)
{
	// 寻找最大最小值
	pair<Type*, Type*> minmax_pair = minmax_element(pSrc, pSrc + nWidth * nHeight);
	Min = *minmax_pair.first;
	Max = *minmax_pair.second;
}


/** 彩色转黑白.
* @param[in] pHead 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowBytes 每行字节数
* @return 函数将返回灰度图像
* @note 仅限对char/BYTE进行处理
* @warning 需要对返回值进行delete处理.
*/
template <typename Type> Type* Rgb2Gray(Type* pHead, int nWidth, int nHeight, int nRowBytes)
{
	ASSERT(1 == sizeof(Type));
	const int nChannel = nRowBytes / nWidth;
	const int nChannelNew = 1;
	const int nRowLenNew = WIDTHBYTES(nWidth * 8);
	Type *pDst = new Type[nHeight * nRowLenNew];

	switch (nChannel)
	{
	case 1:
		memcpy(pDst, pHead, nHeight * nRowBytes);
		break;
	case 3:
	case 4:// 3、4通道处理是一样的
#pragma omp parallel for
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				Type *pSrcTemp = pHead + nChannel * j + i * nRowBytes;
				Type *pDstTemp = pDst + nChannelNew * j + i * nRowLenNew;
				// 必须强制转换为无符号字符型数据(2016/9/12 注)
				BYTE R = *(pSrcTemp + 2);
				BYTE G = *(pSrcTemp + 1);
				BYTE B = *pSrcTemp;
				*pDstTemp = BYTE ((9798 * R + 19235 * G + 3735 * B) / 32768);
			}
		}
		break;
	default:
		SAFE_DELETE(pDst);
		return NULL;
	}
	return pDst;
}


/** 统计灰度图像的直方图. 
*/
template <typename Type> void ImageHistogram(Type* pHead, int nRowlen, int nHist[256], RoiRect roi)
{
	memset(nHist, 0, 256 * sizeof(int));
	for (int i = roi.top; i < roi.bottom; ++i)
	{
		for (int j = roi.left; j < roi.right; ++j)
		{
			int index = (BYTE)pHead[j + i * nRowlen];
			++nHist[index];
		}
	}
}


/** @brief 计算组间差异(2016/9/19). 
*/
template <typename Type> float Difference(Type *Center, int K)
{
	float sum = 0.0f;
	for (int i = 0; i < K; ++i)
	{
		for (int j = i; j < K; ++j)
		{
			sum += fabs(1.0f * (Center[j] - Center[i]));
		}
	}
	return sum;
}


/** @brief 初始化聚类中心(2016/9/19). 
* @param[in] *Center 聚类中心
* @param[in] K 聚类个数
* @param[in] *pHead 图像数据
* @param[in] nRowBytes 图像每行元素个数
* @param[in] roi 图像感兴趣区域
* 当@ref 函数计算图像直方图，取峰值处的灰度为中心.
*/
template <typename Type> void InitializeClusterCenters(float *Center, int K, Type* pHead, int nRowBytes, RoiRect roi)
{
#ifdef _DEBUG
	TRACE(" * 正在初始化聚类中心...\n");
#endif // _DEBUG
	// 获取直方图
	int nHist[256];
	ImageHistogram(pHead, nRowBytes, nHist, roi);
	int index[256];
	for (int i = 0; i < 256; ++i)
	{
		index[i] = i;
	}
	// 插入排序,使灰度值峰值排在前头
	for (int i = 1; i < 256; ++i)
	{
		for (int j = i; j > 0 && nHist[j] > nHist[j-1]; --j)
		{
			int t;
			t = nHist[j]; nHist[j] = nHist[j-1]; nHist[j-1] = t;
			t = index[j]; index[j] = index[j-1]; index[j-1] = t;
		}
	}
	// 寻找K个中心，这些中心的间距不得小于min_diff
	int num;
	float min_diff = 25.0f;
	do 
	{
		num = 0;
		for (int i = 0; i < K; ++i)
		{
			int flag = 0;
			do
			{
				Center[i] = (float)index[num];
				int j = 0;
				for ( ; j < i; ++j)
				{
					// 到某个聚类中心的间距过小
					if (fabs(Center[i] - Center[j]) < min_diff)
						break;
				}
				// Cj到全部Ci的距离较大
				if (j == i)
					flag = 1;
				++num;
			}while(num < 256 && flag == 0);
#ifdef _DEBUG
			TRACE("C[%d] = %.6f\t", i, Center[i]);
#endif // _DEBUG
		}
#ifdef _DEBUG
		TRACE("\n");
#endif // _DEBUG
		min_diff -= 4.0f;
	}while(num == 256);
}


/** 随机生成K个中心. 
* @param[in] *Center 聚类中心
* @param[in] K 聚类个数
* @param[in] *pHead 图像数据
* @param[in] nRowBytes 图像每行元素个数
* @param[in] roi 图像感兴趣区域
*/
template <typename Type> void RandomClusterCenters(float *Center, int K, Type* pHead, int nRowBytes, RoiRect roi)
{
#ifdef _DEBUG
	TRACE(" * 正在初始化聚类中心...\n");
#endif // _DEBUG
	// 随机生成K个中心
	unsigned int seed = (unsigned int)time(NULL);
	for (int i = 0; i < K; i++)
	{
		srand(seed);
		int w = roi.left + rand() % roi.Width();
		int h = roi.top + rand() % roi.Height();
		// BYTE 强制转换是必须的，下同(2016/9/12)
		Center[i] = (BYTE)pHead[w + h * nRowBytes];
#ifdef _DEBUG
		TRACE("C[%d] = %.6f\t", i, Center[i]);
#endif // _DEBUG
		seed = rand();
	}
#ifdef _DEBUG
	TRACE("\n");
#endif // _DEBUG
}


/** @brief K_means 聚类分割. 
* @param[in] pHead 图像数据
* @param[in] nRowBytes 每行字节数
* @param[in] roi 图像感兴趣区域
* @param[in] K 聚类个数
* @param[in] nMaxepoches 最大迭代次数
* @note (1)必须是灰度图像;(2)此函数将修改图像数据@ref pHead.
*/
template <typename Type> void K_means(Type* pHead, int nRowBytes, RoiRect roi, int K, float fBackground, int nMaxepoches)
{
	// 聚类区域的尺寸
	int nWidth = roi.Width();
	int nHeight = roi.Height();
	int nRowlen = nWidth;// 聚类标记每行元素个数
	Type* Cluster = new Type[nHeight * nRowlen];// 聚类标记TAG
	// 聚类中心
	float *Center = new float[K];
	float *oldCenter = new float[K];
	int *sumCenter = new int[K];// 聚类中心求和
	int *sum = new int[K];// 每个聚类中心元素个数
	// 生成K个中心
	InitializeClusterCenters(Center, K, pHead, nRowBytes, roi);
	/* 多次迭代直至收敛，本次试验迭代nMaxepoches次 */
	for (int it = 0; it < nMaxepoches; ++it)
	{
		ZeroMemory(sumCenter, K * sizeof(int));
		ZeroMemory(sum, K * sizeof(int));
		/* 求出每个样本点距应该属于哪一个聚类 */
		for (int j = roi.top; j < roi.bottom; ++j)
		{
			Type *pRowj = pHead + j * nRowBytes;
			for (int i = roi.left; i < roi.right; ++i)
			{
				/* 当前像素的浮点值 */
				int curPixel = (BYTE)pRowj[i];
				/* 都初始化属于第0个聚类 */    
				int tag = 0;
				float temp = curPixel - Center[tag];
				float min_distance = (temp > 0.0F ? temp : -temp);
				for (int s = 1; s < K; s++)
				{
					temp = curPixel - Center[s];
					float new_distance = (temp > 0.0F ? temp : -temp);
					if (new_distance < min_distance)
					{
						min_distance = new_distance;
						tag = s;
					}
				}
				// 将(i, j)标记为tag
				Cluster[(i-roi.left) + (j-roi.top) * nRowlen] = (Type)tag;
				sumCenter[tag] += curPixel;
				++sum[tag];
			}
		}
		/* 保存上一个结果，并更新聚类中心 */
		memcpy(oldCenter, Center, K * sizeof(float));
		float diff = 0.F;
		for (int i = 0; i < K; ++i)
		{
			if (sum[i])
			{
				Center[i] = (float)sumCenter[i] / sum[i];
			}
			diff += (oldCenter[i] - Center[i]) * (oldCenter[i] - Center[i]);
		}
		// 注销掉下述语句以监视迭代过程
#ifdef _DEBUG
		TRACE(" * 迭代次数 = %d：\n", it + 1);
		for (int i = 0; i < K; ++i)
		{
			TRACE("C[%d] = %.6f\t%d个\t", i, Center[i], sum[i]);
		}
		TRACE("\n");
#endif // _DEBUG
		// 当前后相邻两次中心变化较小时（0.01个像素）退出
		if (diff < 0.0003f)
			break;
	}
	
	// 寻找背景值对应的类号
	int backValIdx = 0;
	float min_dis = fabs(Center[0] - fBackground);
	for (int i = 1; i < K; ++i)
	{
		float temp_dis = fabs(Center[i] - fBackground);
		if (temp_dis < min_dis)
		{
			backValIdx = i;
			min_dis = temp_dis;
		}
	}

	// 对原始图像进行分割
	for (int j = 0; j < nHeight; ++j)
	{
		for (int i = 0; i < nWidth; ++i)
		{
			pHead[i + roi.left + (j + roi.top) * nRowBytes] = (Cluster[i + j * nRowlen] == backValIdx) ? 0 : 255;
		}
	}
	delete [] Center;
	delete [] oldCenter;
	delete [] sumCenter;
	delete [] sum;
	delete [] Cluster;
}

// 设置为1，来启用快速排序
#define QUICK_SORT 1

/** 中值滤波. 
* @details 中值滤波比较耗时.
* @param[in] * pData 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nSize 滤波核尺寸
* @warning 仅限彩色图像
* 在解码中最为耗时.
*/
template <typename Type> BOOL MedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen, int nSize)
{
	// 滤波核尺寸必须为奇数
	ASSERT(nSize & 0x00000001);
	Type *pCopy = new Type[nHeight * nRowlen];
	memcpy(pCopy, pData, nHeight * nRowlen * sizeof(Type));
	int nChannel = nRowlen / nWidth;
	ASSERT(nChannel >= 3);
	int K = nSize >> 1;// 滤波核半径
	rgb<Type> *temp = new rgb<Type>[nSize * nSize];// 存放领域像素
	// 逐个像素进行滤波(边界不处理，这样便不需要作越界判断)
	for (int r = K; r < nHeight - K; ++r)
	{
		for (int c = K; c < nWidth - K; ++c)
		{
			int rgbCount = 0;// 当前像素(r, c)的领域像素个数
			for (int r0 = r - K; r0 <= r + K; ++r0)
			{
				for (int c0 = c - K; c0 <= c + K; ++c0)
				{
					// (r0, c0)像素
					Type* Pixel = pData + r0*nRowlen + c0*nChannel;
					temp[rgbCount].b = *Pixel++;
					temp[rgbCount].g = *Pixel++;
					temp[rgbCount].r = *Pixel++;
#if QUICK_SORT
					if (rgbCount > 0)
					{
						// 从小到大插入排序
						int Aj = RGB2INT(temp[rgbCount]);// A[j]
						int Aj_1 = RGB2INT(temp[rgbCount - 1]);// A[j-1]
						for (int j = rgbCount; j > 0 && Aj < Aj_1; )
						{
							Type t;
							t = temp[j].r; temp[j].r = temp[j - 1].r; temp[j - 1].r = t;
							t = temp[j].g; temp[j].g = temp[j - 1].g; temp[j - 1].g = t;
							t = temp[j].b; temp[j].b = temp[j - 1].b; temp[j - 1].b = t;
							--j;
							Aj = Aj_1;
							Aj_1 = RGB2INT(temp[j - 1]);
						}
					}
#endif
					++rgbCount;
				}
			}
			ASSERT(rgbCount == nSize * nSize);
#if !(QUICK_SORT)
			for (int i = 0; i < (rgbCount>>1); ++i)
			{
				int Ai = RGB2INT(temp[i]);
				for (int j = i + 1; j < rgbCount; ++j)
				{
					int Aj = RGB2INT(temp[j]);
					if (Aj < Ai)
					{
						Type t;
						t = temp[j].r; temp[j].r = temp[i].r; temp[i].r = t;
						t = temp[j].g; temp[j].g = temp[i].g; temp[i].g = t;
						t = temp[j].b; temp[j].b = temp[i].b; temp[i].b = t;
						Ai = Aj;
					}
				}
			}
#endif
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

// 像素指针(3bytes)的度量值
#define PIXEL2INT(p) ( 299 * p[0] + 587 * p[1] + 114 * p[2] )

// 将像素指针a,b排序为 a < b (ia, ib为像素度量值)
#define Minmax(ia, ib, a, b) if( ia > ib ) { BYTE *t = a; a = b; b = t; int it = ia; ia = ib; ib = it; }

// 将像素指针a,b排序为 a > b (ia, ib为像素度量值)
#define Maxmin(ia, ib, a, b) if( ia < ib ) { BYTE *t = a; a = b; b = t; int it = ia; ia = ib; ib = it; }

/** 快速中值滤波. 
* @details 中值滤波比较耗时.
* @param[in] * pData 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @warning 仅限彩色图像
* 在解码中最为耗时.
* @see http://wenda.chinabaike.com/z/shenghuo/20131226/725816.html
*/
template <typename Type> BOOL FastMedianFilter(Type* pData, int nWidth, int nHeight, int nRowlen)
{
	Type *pCopy = new Type[nHeight * nRowlen];
	memcpy(pCopy, pData, nHeight * nRowlen * sizeof(Type));
	int nChannel = nRowlen / nWidth;
	ASSERT(nChannel >= 3);
	int K = 1;// 滤波核半径
	BYTE // 滤波核
		*p0 = new BYTE[3], *p1 = new BYTE[3], *p2 = new BYTE[3], 
		*p3 = new BYTE[3], *p4 = new BYTE[3], *p5 = new BYTE[3], 
		*p6 = new BYTE[3], *p7 = new BYTE[3], *p8 = new BYTE[3];
	// 逐个像素进行滤波(边界不处理，这样便不需要作越界判断)
	for (int r = K; r < nHeight - K; ++r)
	{
		for (int c = K; c < nWidth - K; ++c)
		{
			int r0 = r - K;
			int c0 = c - K;
			
			memcpy(p0, pData + r0*nRowlen + c0*nChannel, 3);
			memcpy(p1, pData + r0*nRowlen + (c0+1)*nChannel, 3);
			memcpy(p2, pData + r0*nRowlen + (c0+2)*nChannel, 3);
			memcpy(p3, pData + (r0+1)*nRowlen + c0*nChannel, 3);
			memcpy(p4, pData + (r0+1)*nRowlen + (c0+1)*nChannel, 3);
			memcpy(p5, pData + (r0+1)*nRowlen + (c0+2)*nChannel, 3);
			memcpy(p6, pData + (r0+2)*nRowlen + c0*nChannel, 3);
			memcpy(p7, pData + (r0+2)*nRowlen + (c0+1)*nChannel, 3);
			memcpy(p8, pData + (r0+2)*nRowlen + (c0+2)*nChannel, 3);

			int ip0 = PIXEL2INT(p0), ip1 = PIXEL2INT(p1), ip2 = PIXEL2INT(p2);
			int ip3 = PIXEL2INT(p3), ip4 = PIXEL2INT(p4), ip5 = PIXEL2INT(p5);
			int ip6 = PIXEL2INT(p6), ip7 = PIXEL2INT(p7), ip8 = PIXEL2INT(p8);

			Minmax(ip0, ip1, p0, p1); Minmax(ip0, ip2, p0, p2); Minmax(ip1, ip2, p1, p2);
			Minmax(ip3, ip4, p3, p4); Minmax(ip3, ip5, p3, p5); Minmax(ip4, ip5, p4, p5);
			Minmax(ip6, ip7, p6, p7); Minmax(ip6, ip8, p6, p8); Minmax(ip7, ip8, p7, p8);
			Maxmin(ip6, ip0, p6, p0); Maxmin(ip6, ip3, p6, p3);
			Minmax(ip2, ip5, p2, p5); Minmax(ip2, ip8, p2, p8);
			Minmax(ip1, ip4, p1, p4); Minmax(ip1, ip7, p1, p7); Minmax(ip4, ip7, p4, p7);
			Minmax(ip2, ip4, p2, p4); Minmax(ip2, ip6, p2, p6); Minmax(ip4, ip6, p4, p6);

			memcpy(pCopy + r*nRowlen + c*nChannel, p4, 3);
		}
	}
	delete [] p0; delete [] p1; delete [] p2;
	delete [] p3; delete [] p4; delete [] p5;
	delete [] p6; delete [] p7; delete [] p8;
	memcpy(pData, pCopy, nHeight * nRowlen * sizeof(Type));
	SAFE_DELETE(pCopy);
	return TRUE;
}


/** 自适应中值滤波. 
* @details 中值滤波比较耗时.
* @param[in] * pData 图像数据
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
	int nChannel = nRowlen / nWidth;
	ASSERT(nChannel >= 3);
	// 逐个像素进行滤波
	int nSize = 3;// 起始滤波核尺寸
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


/** 获取最小值、中值、最大值. 
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
template <typename Type> void SapGetMinMedMax(Type* pData, int nWidth, int nHeight, int nRowlen, int nChannel, 
											  Type *pCopy, int r, int c, rgb<Type> *temp, int nSize, int nMaxSize)
{
	int K = nSize >> 1;// 滤波核半径
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
	rgb<Type> cur(*Pixel, *(Pixel+1), *(Pixel+2));// 当前像素
	int nMin = RGB2INT(*Min);
	int nMed = RGB2INT(*Med);
	int nMax = RGB2INT(*Max);
	int nCur = RGB2INT(cur);
	if (nMin < nMed && nMed < nMax)
	{
		if(nMin < nCur && nCur < nMax)
		{
			// 不修改pCopy
			return;
		}
		else
		{
			// 取中值
			*Pixel++ = Med->b;
			*Pixel++ = Med->g;
			*Pixel++ = Med->r;
			return;
		}
	}
	else
	{
		if (nSize >= nMaxSize)
		{
			// 滤波核大小超界，退出
			return;
		}
		else
		{
			// 增大滤波核进行处理
			return SapGetMinMedMax(pData, nWidth, nHeight, nRowlen, nChannel, pCopy, r, c, temp, nSize + 2, nMaxSize);
		}
	}
}


/** 插入排序. 
* @param[in] pArray 待排序数组
* @param[in] Num 数据元素个数
*/
template <typename Type> void InsertionSort(Type *pArray, int Num)
{
	for (int i = 0; i < Num; ++i)
	{
		for (int j = i; (j > 0) && pArray[j] < pArray[j - 1]; --j)
		{
			Type temp = pArray[j];
			pArray[j] = pArray[j - 1];
			pArray[j - 1] = temp;
		}
	}
}