#include "stdafx.h"
#include "MatrixConvert2Image.h"
#include "PixelsOperation.h"


/// 寻找最小值的下标
int CMatrix2Image::FindMinValueIndex(COLORREF x, COLORREF y, COLORREF z, COLORREF w)
{
	COLORREF min1 = x < y ? x : y;
	COLORREF min2 = z < w ? z : w;
	int idx1 = x < y ? 1 : 2;
	int idx2 = z < w ? 3 : 4;
	return min1 < min2 ? idx1 : idx2;
}


/** 
* @brief 将二维码矩阵转换为彩色图像. 
* @param[in] bMatrix 二维码数据
* @param[in] nSymbleSize 二维码尺寸
* @param[in] *pImage 目标图像
* @param[in] nPixelSize 像素大小
* @param[in] ForegroundColor 前景色
* @param[in] BackgroundColor 背景色
* @param[in] QREncodeColor1 彩色一
* @param[in] QREncodeColor2 彩色二
*/
BOOL CMatrix2Image::CreateColorImage(COLORREF ForegroundColor, COLORREF BackgroundColor, 
									  COLORREF QREncodeColor1, COLORREF QREncodeColor2)
{
	// 背景模块的灰度值必须最小
	int minIndex = FindMinValueIndex(ForegroundColor, BackgroundColor, QREncodeColor1, QREncodeColor2);
	if (minIndex != 2)
	{
		TRACE(" * 警告：背景模块的灰度值不是最小，不利于解码!\n");
	}

	BYTE *m_pHead = m_Src.GetImage();
	int m_nRowlen = m_Src.GetRowlen();
	int m_nChannel = m_Src.GetChannel();
	for (int i = 0; i < m_nQrSize; ++i)
	{
		int nRow = i * m_nPixelSize;
		for (int j = 0; j < m_nQrSize; ++j)
		{
			int nCol = j * m_nPixelSize;
			switch (m_qrMat[i][j])
			{
			case COLOR_FOREGROUND:
				::SetPixel(m_pHead, m_nPixelSize, nRow, nCol, m_nRowlen, m_nChannel, ForegroundColor);
				break;
			case COLOR_BACKGROUND:
				::SetPixel(m_pHead, m_nPixelSize, nRow, nCol, m_nRowlen, m_nChannel, BackgroundColor);
				break;
			default:
				SetColorPixel(nRow, nCol, m_qrMat[i][j], QREncodeColor1, QREncodeColor2);
				break;
			}
		}
	}

	return TRUE;
}


/*** 
* @brief 为(nRow, nCol)行列处设置颜色
* @param[in] nRow 行
* @param[in] nCol 列
* @param[in] nMatVal 颜色值索引
* @details 一个模块被分为4部分，按照a1,a2,a3,a4顺序着色. \n
*			a1		a2	\n
*			a3		a4	\n
*/
void CMatrix2Image::SetColorPixel(int nRow, int nCol, int nMatVal, COLORREF QREncodeColor1, COLORREF QREncodeColor2)
{
	int b[4], v = nMatVal - COLOR_QRCOLOR1;
	b[0] =  v     & 1;
	b[1] = (v>>1) & 1;
	b[2] = (v>>2) & 1;
	b[3] = (v>>3) & 1;
	int R[2] = {GetRValue(QREncodeColor1), GetRValue(QREncodeColor2)};
	int G[2] = {GetGValue(QREncodeColor1), GetGValue(QREncodeColor2)};
	int B[2] = {GetBValue(QREncodeColor1), GetBValue(QREncodeColor2)};
	int nHalfPixel = m_nPixelSize >> 1;
	for (int i = 0; i < nHalfPixel; ++i)
	{
		// 行指针
		BYTE *pRow = m_Src.GetImage() + (nRow + i + m_nQRMargin) * m_Src.GetRowlen();
		for (int j = 0; j < nHalfPixel; ++j)//a1
		{
			// 像素指针
			BYTE *Pixel = pRow + m_Src.GetChannel() * (j + nCol + m_nQRMargin);
			Pixel[0] = B[b[0]];
			Pixel[1] = G[b[0]];
			Pixel[2] = R[b[0]];
		}
		for (int j = nHalfPixel; j < m_nPixelSize; ++j)//a2
		{
			// 像素指针
			BYTE *Pixel = pRow + m_Src.GetChannel() * (j + nCol + m_nQRMargin);
			Pixel[0] = B[b[1]];
			Pixel[1] = G[b[1]];
			Pixel[2] = R[b[1]];
		}
	}
	for (int i = nHalfPixel; i < m_nPixelSize; ++i)
	{
		// 行指针
		BYTE *pRow = m_Src.GetImage() + (nRow + i + m_nQRMargin) * m_Src.GetRowlen();
		for (int j = 0; j < nHalfPixel; ++j)//a3
		{
			// 像素指针
			BYTE *Pixel = pRow + m_Src.GetChannel() * (j + nCol + m_nQRMargin);
			Pixel[0] = B[b[2]];
			Pixel[1] = G[b[2]];
			Pixel[2] = R[b[2]];
		}
		for (int j = nHalfPixel; j < m_nPixelSize; ++j)//a4
		{
			// 像素指针
			BYTE *Pixel = pRow + m_Src.GetChannel() * (j + nCol + m_nQRMargin);
			Pixel[0] = B[b[3]];
			Pixel[1] = G[b[3]];
			Pixel[2] = R[b[3]];
		}
	}
}