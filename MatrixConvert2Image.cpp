#include "stdafx.h"
#include "MatrixConvert2Image.h"
#include "PixelsOperation.h"


int FindMinValueIndex(COLORREF x, COLORREF y, COLORREF z, COLORREF w)
{
	COLORREF min1 = x < y ? x : y;
	COLORREF min2 = z < w ? z : w;
	int idx1 = x < y ? 1 : 2;
	int idx2 = z < w ? 3 : 4;
	return min1 < min2 ? idx1 : idx2;
}


/* - 转化图像 -
BYTE* bDMatrix					01矩阵
int nSymbleSize					二维码尺寸
CyImage* pImage					二维码图像
int nPixelSize					像素大小
COLORREF ForegroundColor		前景色
COLORREF BackgroundColor		背景色
*/
BOOL Matrix2Image(BYTE* bDMatrix, int nSymbleSize, CyImage* pImage, int nPixelSize, COLORREF ForegroundColor, COLORREF BackgroundColor)
{
	int nWidth = nPixelSize * nSymbleSize + QR_MARGIN * 2;
	int nHeight = nPixelSize * nSymbleSize + QR_MARGIN * 2;

	if (FALSE == pImage->Create(nWidth, nHeight, 24, 0UL))
		return FALSE;

	BYTE* pHead = pImage->GetHeadAddress();
	int nRowlen = pImage->GetRowlen();
	memset(pHead, 255, nHeight * nRowlen * sizeof(BYTE));

	for (int i = 0; i < nSymbleSize; ++i)
	{
		for (int j = 0; j < nSymbleSize; ++j)
		{
			if (bDMatrix[j + (nSymbleSize - i - 1) * nSymbleSize])
			{
				// 设置背景色
				SetPixel(pHead, nPixelSize, i * nPixelSize, j * nPixelSize, nRowlen, 3, BackgroundColor);
			}
			else
			{
				// 设置前景色
				SetPixel(pHead, nPixelSize, i * nPixelSize, j * nPixelSize, nRowlen, 3, ForegroundColor);
			}
		}
	}
	// Three isolated points(三个孤立点)
	SetPixel(pHead, nPixelSize, 7 * nPixelSize, 7 * nPixelSize, nRowlen, 3, ForegroundColor);
	SetPixel(pHead, nPixelSize, 7 * nPixelSize, (nSymbleSize - 8) * nPixelSize, nRowlen, 3, ForegroundColor);
	SetPixel(pHead, nPixelSize, (nSymbleSize - 8) * nPixelSize, 7 * nPixelSize, nRowlen, 3, ForegroundColor);

	// 修改浮点数据
	pImage->MemcpyByteToFloat();
	return TRUE;
}


/** 将二维码矩阵转换为彩色图像. 
二维码数据：BYTE bMatrix[MAX_MODULESIZE][MAX_MODULESIZE]
二维码尺寸：int nSymbleSize
目标图像：CyImage* pImage
像素大小：int nPixelSize
前景色：COLORREF ForegroundColor
背景色：COLORREF BackgroundColor
彩色一：COLORREF QREncodeColor1
彩色二：COLORREF QREncodeColor2
*/
BOOL Matrix2ColorImage(BYTE bMatrix[MAX_MODULESIZE][MAX_MODULESIZE], int nSymbleSize, CyImage* pImage, int nPixelSize, 
					   COLORREF ForegroundColor, COLORREF BackgroundColor, COLORREF QREncodeColor1, COLORREF QREncodeColor2)
{
	// 背景模块的灰度值必须最小
	int minIndex = FindMinValueIndex(ForegroundColor, BackgroundColor, QREncodeColor1, QREncodeColor2);
	if (minIndex != 2)
	{
		TRACE(" * 警告：背景模块的灰度值不是最小，不利于解码!");
	}

	int nWidth = nPixelSize * nSymbleSize + QR_MARGIN * 2;
	int nHeight = nPixelSize * nSymbleSize + QR_MARGIN * 2;

	if (FALSE == pImage->Create(nWidth, nHeight, 24, 0UL))
		return FALSE;

	BYTE* pHead = pImage->GetHeadAddress();
	int nRowlen = pImage->GetRowlen();
	memset(pHead, 255, nHeight * nRowlen * sizeof(BYTE));

	for (int i = 0; i < nSymbleSize; ++i)
	{
		for (int j = 0; j < nSymbleSize; ++j)
		{
			switch (bMatrix[i][j])
			{
			case COLOR_FOREGROUND:
				SetPixel(pHead, nPixelSize, i * nPixelSize, j * nPixelSize, nRowlen, 3, ForegroundColor);break;
			case COLOR_BACKGROUND:
				SetPixel(pHead, nPixelSize, i * nPixelSize, j * nPixelSize, nRowlen, 3, BackgroundColor);break;
			case COLOR_QRCOLOR1:
				SetPixel(pHead, nPixelSize, i * nPixelSize, j * nPixelSize, nRowlen, 3, QREncodeColor1);break;
			case COLOR_QRCOLOR2:
				SetPixel(pHead, nPixelSize, i * nPixelSize, j * nPixelSize, nRowlen, 3, QREncodeColor2);break;
			default:
				break;
			}
		}
	}

	// 修改浮点数据
	pImage->MemcpyByteToFloat();
	return TRUE;
}