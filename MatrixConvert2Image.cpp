#include "stdafx.h"
#include "MatrixConvert2Image.h"
#include "PixelsOperation.h"


/// 寻找最小值的下标
int FindMinValueIndex(COLORREF x, COLORREF y, COLORREF z, COLORREF w)
{
	COLORREF min1 = x < y ? x : y;
	COLORREF min2 = z < w ? z : w;
	int idx1 = x < y ? 1 : 2;
	int idx2 = z < w ? 3 : 4;
	return min1 < min2 ? idx1 : idx2;
}


/** - 转化图像 -
* @param[in] * bDMatrix				01矩阵
* @param[in] nSymbleSize			二维码尺寸
* @param[in] * pImage				二维码图像
* @param[in] nPixelSize				像素大小
* @param[in] ForegroundColor		前景色
* @param[in] BackgroundColor		背景色
*/
BOOL Matrix2Image(BYTE* bDMatrix, int nSymbleSize, CImage* pImage, int nPixelSize, 
				  COLORREF ForegroundColor, COLORREF BackgroundColor)
{
	int nWidth = nPixelSize * nSymbleSize + QR_MARGIN * 2;
	int nHeight = nPixelSize * nSymbleSize + QR_MARGIN * 2;

	if (!pImage->IsNull())
		pImage->Destroy();
	if (FALSE == pImage->Create(nWidth, nHeight, 24, 0UL))
		return FALSE;

	BYTE* pHead = (BYTE*) pImage->GetBits() + pImage->GetPitch() * (pImage->GetHeight() - 1);
	int nRowlen = abs(pImage->GetPitch());
	memset(pHead, 255, nHeight * nRowlen * sizeof(BYTE));

	for (int i = 0; i < nSymbleSize; ++i)
	{
		int nRow = i * nPixelSize;
		for (int j = 0; j < nSymbleSize; ++j)
		{
			int nCol = j * nPixelSize;
			if (bDMatrix[j + (nSymbleSize - i - 1) * nSymbleSize])
			{
				// 设置背景色
				SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, 3, BackgroundColor);
			}
			else
			{
				// 设置前景色
				SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, 3, ForegroundColor);
			}
		}
	}
	// Three isolated points(三个孤立点)
	SetPixel(pHead, nPixelSize, 7 * nPixelSize, 7 * nPixelSize, nRowlen, 3, ForegroundColor);
	SetPixel(pHead, nPixelSize, 7 * nPixelSize, (nSymbleSize - 8) * nPixelSize, nRowlen, 3, ForegroundColor);
	SetPixel(pHead, nPixelSize, (nSymbleSize - 8) * nPixelSize, 7 * nPixelSize, nRowlen, 3, ForegroundColor);

	return TRUE;
}


/** 将二维码矩阵转换为彩色图像. 
* @param[in] bMatrix 二维码数据
* @param[in] nSymbleSize 二维码尺寸
* @param[in] * pImage 目标图像
* @param[in] nPixelSize 像素大小
* @param[in] ForegroundColor 前景色
* @param[in] BackgroundColor 背景色
* @param[in] QREncodeColor1 彩色一
* @param[in] QREncodeColor2 彩色二
*/
BOOL Matrix2ColorImage(BYTE bMatrix[MAX_MODULESIZE][MAX_MODULESIZE], int nSymbleSize, CImage* pImage, int nPixelSize, 
					   COLORREF ForegroundColor, COLORREF BackgroundColor, COLORREF QREncodeColor1, COLORREF QREncodeColor2)
{
	// 背景模块的灰度值必须最小
	int minIndex = FindMinValueIndex(ForegroundColor, BackgroundColor, QREncodeColor1, QREncodeColor2);
	if (minIndex != 2)
	{
		TRACE(" * 警告：背景模块的灰度值不是最小，不利于解码!\n");
	}

	int nWidth = nPixelSize * nSymbleSize + QR_MARGIN * 2;
	int nHeight = nPixelSize * nSymbleSize + QR_MARGIN * 2;

	if (!pImage->IsNull())
		pImage->Destroy();
	if (FALSE == pImage->Create(nWidth, nHeight, 24, 0UL))
		return FALSE;

	BYTE* pHead = (BYTE*) pImage->GetBits() + pImage->GetPitch() * (pImage->GetHeight() - 1);
	int nRowlen = abs(pImage->GetPitch());
	memset(pHead, 255, nHeight * nRowlen * sizeof(BYTE));

	for (int i = 0; i < nSymbleSize; ++i)
	{
		int nRow = i * nPixelSize;
		for (int j = 0; j < nSymbleSize; ++j)
		{
			int nCol = j * nPixelSize;
			switch (bMatrix[i][j])
			{
			case COLOR_FOREGROUND:
				SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, 3, ForegroundColor);break;
			case COLOR_BACKGROUND:
				SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, 3, BackgroundColor);break;
			case COLOR_QRCOLOR1:
				SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, 3, QREncodeColor1);break;
			case COLOR_QRCOLOR2:
				SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, 3, QREncodeColor2);break;
			default:
				break;
			}
		}
	}

	return TRUE;
}