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
using namespace std;

#pragma once

#pragma warning(disable : 4244)

// 将图像写入文件的文件头大小（字节）
#define HEADER_SIZE 1024

// 放大图像：会修改pfHead及更新图像信息
template <typename Type> void ZoomImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int NewWidth, int NewHeight);

// 限制图像最大边长，当超出时修改大小
template <typename Type> void LimitImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int nMaxSize = 320);

// 获取截屏图像中的QR码矩形
template <typename Type> void GetQrRect(const Type* pHead, int nWidth, int nHeight, int nChannel, int nRowlen, RoiRect &cutAero);

// 上下翻转图像
template <typename Type> void ImageFlipV(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 左右翻转图像
template <typename Type> void ImageFlipH(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// 转置图像
template <typename Type> void ImageTranspose(Type** pHead, const int nWidth, const int nHeight, const int nRowlen);

// 提取图像感兴趣区域
template <typename Type> Type* ImageROI(const Type* pHead, int &nWidth, int &nHeight, int &nRowlen, const RoiRect &roi = 0);

// 将图像以二进制形式写入文件
template <typename Type> BOOL ImageWrite(const char* fileName, const Type* pSrc, int nWidth, int nHeight, int nRowlen);

// 彩色图像转为8位的单通道图像
template <typename Type> Type* Rgb2Gray(const Type* pHead, int nWidth, int nHeight, int nRowBytes);

// 统计灰度图像的直方图
template <typename Type> void GrayHistogram(const Type* pHead, int nRowlen, int nHist[256], const RoiRect &roi);

// 统计图像的直方图
template <typename Type> void ImageHistogram(const Type* pHead, int nWidth, int nHeight, int nRowlen, int nHist[256]);

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

	const Type *pSrc = temp + (nHeight - 1) * nRowlen;
	Type *pDst = pHead;
	for (int r = 0; r < nHeight; ++r)
	{
		memcpy(pDst, pSrc, nRowlen * sizeof(Type));
		pSrc -= nRowlen;
		pDst += nRowlen;
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

	const int nChannel = nRowlen / nWidth;
	const Type *pSrc = temp + (nWidth - 1) * nChannel;
	Type *pDst = pHead;
	for (int r = 0; r < nHeight; ++r)
	{
		const Type *pSrcLine = pSrc;// 行指针
		Type *pDstLine = pDst;
		for (int c = 0; c < nWidth; ++c)
		{
			memcpy(pDstLine, pSrcLine, nChannel * sizeof(Type));
			pSrcLine -= nChannel;
			pDstLine += nChannel;
		}
		pSrc += nRowlen;
		pDst += nRowlen;
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
	memset(temp, 0, nWidth * nNewRowlen * sizeof(Type));
	for (int k = 0; k < nChannel; ++k)
	{
		int x = 0, y1 = 0;
		for (int i = 0; i < nWidth; ++i)
		{
			int y = 0, x1 = 0;
			for (int j = 0; j < nHeight; ++j)
			{
				temp[k + x1 + y1] = pSrc[k + x + y];
				y += nRowlen;
				x1 += nChannel;
			}
			x += nChannel;
			y1 += nNewRowlen;
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
* @param[in] &roi 图像感兴趣区域
* @return 函数返回图像感兴趣区域(指针，需要SAFE_DELETE)
* @note 函数传入图像宽度、高度等信息，输出ROI的宽度、高度等信息
*/
template <typename Type> Type* ImageROI(const Type* pHead, int &nWidth, int &nHeight, int &nRowlen, const RoiRect &roi)
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

	Type *pDstLine = pDst;
	const Type *pSrcLine = pHead + roi.left * nChannel + (nHeight - 1 - roi.top) * nRowlen;
	for (int i = 0; i < nNewHeight; ++i)
	{
		memcpy(pDstLine, pSrcLine, nNewRowlen * sizeof(Type));
		pSrcLine -= nRowlen;
		pDstLine += nNewRowlen;
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

	for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
	{
		int x = 0;
		float s = 0;
		for (int i = 0; i < NewWidth; ++i)
		{
			int y = 0;
			float t = 0;
			for (int j = 0; j < NewHeight; ++j)
			{
				int x1 = int(s), y1 = int(t), x3 = x1 + 1, y3 = y1 + 1;
				// 左下角的点
				const BYTE* pLB = *pfHead + nCurChannel + x1 * nChannel + y1 * nRowlen;//必须转换为BYTE*
				// 对越界的处理
				pDst[nCurChannel + x + y] = (x1 < 0 || x3 >= nWidth || y1 < 0 || y3 >= nHeight) ? 0 : 
					(*pLB * (x3 - s) + *(pLB + nChannel) * (s - x1)) * (y3 - t) 
					+ (*(pLB + nRowlen) * (x3 - s) + *(pLB + nChannel + nRowlen) * (s - x1)) * (t - y1);
				y += NewRowlen;
				t += hRatio;
			}
			x += nChannel;
			s += wRatio;
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
* @warning 输入图像需为字节类型，宽度、高度、每行字节数会被修改
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


// 获取图像行的梯度和[不计算边缘，计算中间1/3]
template <typename Type>
inline int RowProject(const Type* pHead, int nWidth, int nHeight, int nChannel, int nRowlen, int *row)
{
	memset(row, 0, nHeight * sizeof(int));
	const int nHalfW = nWidth / 2;
	const int R = nWidth / 6;
	const BYTE *p = (const BYTE*) pHead;
	int avg = 0;
	for (int r = 0; r < nHeight; ++r)
	{
		for (int i = nHalfW - R, y = r * nRowlen; i < nHalfW + R; ++i) 
			row[r] += abs(*(p + i * nChannel + y) - *(p + (i-1) * nChannel + y));
		if (nHeight/3 <= r && r < nHeight * 2/3)
			avg += row[r];
	}
	int f_avg = avg / nHeight;
	return f_avg;
}


// 获取图像列的梯度和[不计算边缘，计算中间1/3]
template <typename Type>
inline int ColProject(const Type* pHead, int nWidth, int nHeight, int nChannel, int nRowlen, int *col)
{
	memset(col, 0, nWidth * sizeof(int));
	const int nHalfH = nHeight / 2;
	const int R = nHeight / 6;
	const BYTE *p = (const BYTE*) pHead;
	int avg = 0;
	for (int c = 0; c < nWidth; ++c)
	{
		for (int j = nHalfH - R, x = c * nChannel; j < nHalfH + R; ++j)
			col[c] += abs(*(p + x + j * nRowlen) - *(p + x + (j+1) * nRowlen));
		if (nWidth/3 <= c && c < nWidth * 2/3)
			avg += col[c];
	}
	int f_avg = avg / nWidth;
	return f_avg;
}

/* - 提取截屏图像中间的Qr码 -
* @param[in] *pHead 数据头
* @param[in] &nWidth 宽度
* @param[in] &nHeight 高度
* @param[in] &nRowlen 每行个数
* @param[in] &cutAero 裁剪比例
* @note 当图像宽高比大于等于1.5时，提取QR码
* @warning 输入图像需为字节类型，宽度、高度、每行字节数会被修改
*/
template <typename Type> void GetQrRect(const Type* pHead, int nWidth, int nHeight, int nChannel, int nRowlen, RoiRect &cutAero)
{
	if (nWidth > 2048 || nHeight > 2048)
	{
		cutAero = RoiRect(0, 0, nWidth, nHeight);
		return;
	}
	int nHalfW = nWidth/2, nHalfH = nHeight/2;
	int difRow[2048] = {0};
	int difCol[2048] = {0};
	double avgRow = RowProject(pHead, nWidth, nHeight, nChannel, nRowlen, difRow);
	double avgCol = ColProject(pHead, nWidth, nHeight, nChannel, nRowlen, difCol);
	int Start = 0, End = 0;
	for (int i = 0; i < nHalfH; ++i)// 计算top
	{
		if (difRow[nHalfH - i] < avgRow)
		{
			Start = nHalfH - 1 - i;
			break;
		}
	}
	for (int i = 0; i < nHalfH; ++i)// 计算bottom
	{
		if (difRow[nHalfH + i] < avgRow)
		{
			End = nHalfH + 1 + i;
			break;
		}
	}
	for (int i = 0; i < nHalfW; ++i)// 计算left
	{
		if (difCol[nHalfW - i] < avgCol)
		{
			cutAero.left = nHalfW - 1 - i;
			break;
		}
	}
	for (int i = 0; i < nHalfW; ++i)// 计算right
	{
		if (difCol[nHalfW + i] < avgCol)
		{
			cutAero.right = nHalfW + 1 + i;
			break;
		}
	}
	cutAero.top = nHeight - End;
	cutAero.bottom = nHeight - Start;
	cutAero.left -= QR_MARGIN;
	cutAero.right += QR_MARGIN;
	cutAero.top -= QR_MARGIN;
	cutAero.bottom += QR_MARGIN;
	cutAero.left = max(0, cutAero.left);
	cutAero.right = min(cutAero.right, nWidth - 1);
	cutAero.top = max(0, cutAero.top);
	cutAero.bottom = min(cutAero.bottom, nHeight - 1);
	int rW = 4, rH = 6;
	if (nWidth > nHeight)
		swap(rW, rH);
	if (cutAero.Width() < nWidth/rW || cutAero.Height() < nHeight/rH)
	{
		const int N = 5;
		cutAero = nHeight > nWidth ? RoiRect(QR_MARGIN, nHeight/N, nWidth-1 - QR_MARGIN, nHeight * (N-1)/N):
			RoiRect(nWidth/N, QR_MARGIN, nWidth * (N-1)/N, nHeight-1 - QR_MARGIN);
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
		{
			const Type *pSrcTemp0 = pHead;
			Type *pDstTemp0 = pDst;
			for (int i = 0; i < nHeight; i++)
			{
				const Type *pSrcTemp = pSrcTemp0;
				Type *pDstTemp = pDstTemp0;
				for (int j = 0; j < nWidth; j++)
				{
					// 必须强制转换为无符号字符型数据(2016/9/12 注)
					BYTE R = *(pSrcTemp + 2);
					BYTE G = *(pSrcTemp + 1);
					BYTE B = *pSrcTemp;
					*pDstTemp = BYTE(RGB2GRAY(R, G, B));
					pSrcTemp += nChannel;
					pDstTemp += nChannelNew;
				}
				pSrcTemp0 += nRowBytes;
				pDstTemp0 += nRowLenNew;
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
* @param[in] &roi 图像感兴趣区域
*/
template <typename Type> void GrayHistogram(const Type* pHead, int nRowlen, int nHist[256], const RoiRect &roi)
{
	ASSERT(sizeof(Type) == 1);
	memset(nHist, 0, 256 * sizeof(int));
	const BYTE *pSrcLine = (const BYTE*)pHead + roi.left + roi.top * nRowlen;
	for (int i = roi.top; i < roi.bottom; ++i)
	{
		const BYTE *pCur = pSrcLine;
		for (int j = roi.left; j < roi.right; ++j)
		{
			BYTE cur = *pCur ++;
			++ nHist[cur];
		}
		pSrcLine += nRowlen;
	}
}

/** 
* @brief 统计图像的直方图.
* @param[in] *pHead 图像指针
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nHist 图像直方图
*/
template <typename Type> 
void ImageHistogram(const Type* pHead, int nWidth, int nHeight, int nRowlen, int nHist[256])
{
	const int nChannel = nRowlen / nWidth;
	if (1 == nChannel)
		return GrayHistogram(pHead, nRowlen, nHist, RoiRect(0, 0, nWidth, nHeight));
	ASSERT(sizeof(Type) == 1);
	memset(nHist, 0, 256 * sizeof(int));
	const BYTE *pSrcLine = (const BYTE*) pHead;
	for (int i = 0; i < nHeight; ++i)
	{
		const BYTE *pCur = pSrcLine;
		for (int j = 0; j < nWidth; ++j)
		{
			int index = RGB2GRAY(*(pCur+2), *(pCur+1), *pCur);
			++ nHist[index];
			pCur += nChannel;
		}
		pSrcLine += nRowlen;
	}
}