/**
* @file MatrixConvert2Image.h
* @brief 矩阵转图像
*/

#ifdef WIN32

#include "DataTypes.h"

#pragma once

#define COLOR_FOREGROUND	0		//前景色
#define COLOR_BACKGROUND	1		//背景色
#define COLOR_QRCOLOR1		2		//彩色一(前景)
#define COLOR_QRCOLOR2		17		//彩色二(背景)
#define MAX_MODULESIZE		177		//QR码最大尺寸

class CMatrix2Image
{
public:
	/**
	* @brief 
	* @param[in] bMatrix 二维码数据
	* @param[in] nSymbleSize 二维码尺寸
	* @param[in] *pImage 目标图像
	* @param[in] nPixelSize 像素大小
	*/
	CMatrix2Image(qrMat *mat, int nSymbleSize, CImage* pImage, int nPixelSize) 
	{
		m_qrMat = mat;
		m_nQrSize = nSymbleSize;
		m_pImage = pImage;
		m_nPixelSize = nPixelSize;
		m_nWidth = m_nPixelSize * m_nQrSize + QR_MARGIN * 2;
		m_nHeight = m_nPixelSize * m_nQrSize + QR_MARGIN * 2;
		m_nChannel = 3;

		if (!m_pImage->IsNull())
			m_pImage->Destroy();
		m_pImage->Create(m_nWidth, m_nHeight, 8 * m_nChannel, 0UL);

		m_pHead = (BYTE*) m_pImage->GetBits() + m_pImage->GetPitch() * (m_pImage->GetHeight() - 1);
		m_nRowlen = abs(m_pImage->GetPitch());
		memset(m_pHead, 255, m_nHeight * m_nRowlen * sizeof(BYTE));
	}
	~CMatrix2Image(){ }

	// 将二维码矩阵转换为彩色图像
	BOOL Matrix2ColorImage(COLORREF ForegroundColor, COLORREF BackgroundColor, 
		COLORREF QREncodeColor1, COLORREF QREncodeColor2);

private:
	// 寻找四个元素之间最小的，返回其标号
	int FindMinValueIndex(COLORREF x, COLORREF y, COLORREF z, COLORREF w);

	// 为(nRow, nCol)行列处设置颜色
	void SetColorPixel(int nRow, int nCol, int nMatVal, COLORREF QREncodeColor1, COLORREF QREncodeColor2);

	qrMat *m_qrMat;
	int m_nQrSize;
	CImage* m_pImage;
	BYTE* m_pHead;
	int m_nWidth;
	int m_nHeight;
	int m_nRowlen;
	int m_nChannel;
	int m_nPixelSize;
};

#endif // WIN32