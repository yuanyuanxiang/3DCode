/** 
* @file Transform.h
* @brief 图像变换模板函数,可以跨平台.
* @details 包括：图像缩放、旋转、翻转函数；图像感兴趣区域、保存、直方图函数等
* @author 袁沅祥
* @email 962914132@qq.com
* @version V1.0
* @date 2016/10/4
* Copyleft (c), All rights reserved.
* @note 务必添加此包含"algorithm.h"文件，否则在Eclipse遇到
* error: 'sort' is not a member of 'std'；
* 凡是以**二级指针作为参数，函数将修改此一级指针
*/

#include "DataTypes.h"
#include <fstream>

#pragma once

// 将图像写入文件的文件头大小（字节）
#define HEADER_SIZE 1024

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
template <typename Type> Type* ImageROI(const Type* pHead, int &nWidth, int &nHeight, int &nRowlen, RoiRect roi = RoiRect(0, 0, 0, 0));

// 将图像以二进制形式写入文件
template <typename Type> BOOL ImageWrite(const char* fileName, const Type* pSrc, int nWidth, int nHeight, int nRowlen);

// 彩色图像转为8位的单通道图像
template <typename Type> Type* Rgb2Gray(const Type* pHead, int nWidth, int nHeight, int nRowBytes);

// 统计灰度图像的直方图
template <typename Type> void ImageHistogram(const Type* pHead, int nRowlen, int nHist[256], RoiRect roi);

//////////////////////////////////////////////////////////////////////////


/** 
* @brief 上下翻转图像.
* @param[in] *pHead 数据头
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


/** 
* @brief 左右翻转图像
* @param[in] *pHead 数据头
* @param[in] nWidth 宽度
* @param[in] nHeight 高度
* @param[in] nRowlen 每行个数
*/
template <typename Type> void ImageFlipH(Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	Type* temp = new Type[nHeight * nRowlen];
	memcpy(temp, pHead, nHeight * nRowlen * sizeof(Type));

	int nChannel = nRowlen / nWidth;

#pragma omp parallel for
	for (int r = 0; r < nHeight; ++r)
	{
		int y = r * nRowlen;
		for (int c = 0; c < nWidth; ++c)
		{
			memcpy(pHead + c *nChannel + y, temp + (nWidth - 1 - c) * nChannel + y, nChannel * sizeof(Type));
		}
	}
	SAFE_DELETE(temp);
}


/** 
* @brief 左右翻转图像
* @param[in] **pHead 数据头
* @param[in] nWidth 宽度
* @param[in] nHeight 高度
* @param[in] nRowlen 每行个数
* @note 函数将修改图像指针
*/
template <typename Type> void ImageTranspose(Type** pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	// 图像每像素元素个数
	const int nChannel = nRowlen / nWidth;
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
		int x = i * nChannel, y1 = i * nNewRowlen;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, x1 = j * nChannel;
			for (int k = 0; k < nChannel; ++k)
			{
				temp[k + x1 + y1] = pSrc[k + x + y];
			}
		}
	}
	delete[] * pHead;
	*pHead = temp;
	temp = NULL;
}


/** 
* @brief 提取图像感兴趣区域
* @param[in] **pHead 数据头
* @param[in] &nWidth 宽度
* @param[in] &nHeight 高度
* @param[in] &nRowlen 每行个数
* @param[in] roi 图像感兴趣区域
* @return 函数返回图像感兴趣区域(指针，需要SAFE_DELETE)
* @note 函数传入图像宽度、高度等信息，输出ROI的宽度、高度等信息
*/
template <typename Type> Type* ImageROI(const Type* pHead, int &nWidth, int &nHeight, int &nRowlen, RoiRect roi)
{
	/// 如果roi非法
	if (roi.Width() <= 0 || roi.Height() <= 0 || roi.right > nWidth || roi.bottom > nHeight)
	{
		Type* pDst = new Type[nHeight * nRowlen];
		memcpy(pDst, pHead, nHeight * nRowlen * sizeof(Type));
		return pDst;
	}

	const int nChannel = nRowlen / nWidth;
	const int nNewWidth = roi.Width();
	const int nNewHeight = roi.Height();
	int nNewRowlen = nNewWidth * nChannel;
	// 是否需要进行对齐处理
	if (1 == sizeof(Type))
	{
		nNewRowlen = WIDTHBYTES(nNewRowlen * 8);
	}

	Type* pDst = new Type[nNewHeight * nNewRowlen];

#pragma omp parallel for
	int x0 = roi.left * nChannel;
	for (int i = 0; i < nNewHeight; ++i)
	{
		int nLine = nHeight - roi.top - i - 1;
		memcpy(pDst + i * nNewRowlen, pHead + x0 + nLine * nRowlen, nNewRowlen * sizeof(Type));
	}

	// 更新图像信息
	nWidth = nNewWidth;
	nHeight = nNewHeight;
	nRowlen = nNewRowlen;
	return pDst;
}


/** 
* @brief 将图像以二进制形式写入文件
* @param[in] *fileName 文件名称
* @param[in] *pHead 数据头
* @param[in] nWidth 宽度
* @param[in] nHeight 高度
* @param[in] nRowlen 每行个数
* @see FileIO中的WriteTxt/ReadTxt函数
*/
template <typename Type> BOOL ImageWrite(const char* fileName, const Type* pSrc, int nWidth, int nHeight, int nRowlen)
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
* @param[in] **pHead 数据头
* @param[in] &nWidth 宽度
* @param[in] &nHeight 高度
* @param[in] &nRowlen 每行个数
* @param[in] nChannel 图像通道
* @param[in] NewWidth 图像新宽度
* @param[in] NewHeight 图像新高度
* @note 函数会修改pfHead指针的内容，作为结果
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
		int x = i * nChannel;
		for (int j = 0; j < NewHeight; ++j)
		{
			int y = j * NewRowlen;
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + x + y] =
				(Type)biLinearInterp(*pfHead, nWidth, nHeight, nRowlen, nChannel, nCurChannel, i * wRatio, j * hRatio);
		}
	}

	// 修改pHead指向的内容
	delete[] * pfHead;
	*pfHead = pDst;
	pDst = NULL;

	// 更新图像信息
	nWidth = NewWidth;
	nHeight = NewHeight;
	nRowlen = NewRowlen;
}


/* - 限制图像最大边长 -
* @param[in] **pHead 数据头
* @param[in] &nWidth 宽度
* @param[in] &nHeight 高度
* @param[in] &nRowlen 每行个数
* @param[in] nChannel 图像通道
* @param[in] nMaxSize 最大边长320
* @note 当图像有一边长度超过限制时进行缩放并更新信息
* @warning 宽度、高度、每行字节数会被修改
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

/** 
* @brief 彩色转黑白.
* @param[in] *pHead 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowBytes 每行字节数
* @return 函数将返回灰度图像
* @note 仅限对char/BYTE进行处理
* @warning 需要对返回值进行delete处理.
*/
template <typename Type> Type* Rgb2Gray(const Type* pHead, int nWidth, int nHeight, int nRowBytes)
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
			int y1 = i * nRowBytes, y2 = i * nRowLenNew;
			for (int j = 0; j < nWidth; j++)
			{
				const Type *pSrcTemp = pHead + nChannel * j + y1;
				Type *pDstTemp = pDst + nChannelNew * j + y2;
				// 必须强制转换为无符号字符型数据(2016/9/12 注)
				BYTE R = *(pSrcTemp + 2);
				BYTE G = *(pSrcTemp + 1);
				BYTE B = *pSrcTemp;
				*pDstTemp = BYTE(RGB2GRAY(R, G, B));
			}
		}
		break;
	default:
		SAFE_DELETE(pDst);
		return NULL;
	}
	return pDst;
}


/** 
* @brief 统计灰度图像的直方图.
* @param[in] *pHead 图像指针
* @param[in] nRowlen 图像每行字节数
* @param[in] nHist 图像直方图
* @param[in] roi 图像感兴趣区域
*/
template <typename Type> void ImageHistogram(const Type* pHead, int nRowlen, int nHist[256], RoiRect roi)
{
	ASSERT(sizeof(Type) == 1);
	memset(nHist, 0, 256 * sizeof(int));
	for (int i = roi.top; i < roi.bottom; ++i)
	{
		int y = i * nRowlen;
		for (int j = roi.left; j < roi.right; ++j)
		{
			int index = (BYTE)pHead[j + y];
			++ nHist[index];
		}
	}
}