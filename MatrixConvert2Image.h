/**
* @file MatrixConvert2Image.h
* @brief 矩阵转图像
*/


#include "DataTypes.h"

#pragma once
#include "ImageSrc.h"

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
	* @param[in] *mat 二维码矩阵
	* @param[in] nSymbleSize 二维码尺寸
	* @param[in] nPixelSize 像素大小
	* @param[in] nQRMargin QR边界
	*/
	CMatrix2Image(qrMat *mat, int nSymbleSize, int nPixelSize, int nQRMargin = QR_MARGIN) :
		m_qrMat (mat),
		m_nQrSize (nSymbleSize),
		m_nPixelSize (nPixelSize),
		m_nQRMargin (nQRMargin), 
		m_Src(m_nPixelSize * m_nQrSize + m_nQRMargin * 2, m_nPixelSize * m_nQrSize + m_nQRMargin * 2, 4) { }

	~CMatrix2Image(){ }

	// 将二维码矩阵转换为彩色图像
	BOOL CreateColorImage(COLORREF ForegroundColor, COLORREF BackgroundColor, 
		COLORREF QREncodeColor1, COLORREF QREncodeColor2);

	const ImageSrc* GetImageSrc() const { return &m_Src; }

private:
	// 寻找四个元素之间最小的，返回其标号
	int FindMinValueIndex(COLORREF x, COLORREF y, COLORREF z, COLORREF w);

	// 为(nRow, nCol)行列处设置颜色
	void SetColorPixel(int nRow, int nCol, int nMatVal, COLORREF QREncodeColor1, COLORREF QREncodeColor2);

	qrMat*		m_qrMat;		/**< QR矩阵 */
	int			m_nQrSize;		/**< QR尺寸 */
	int			m_nPixelSize;	/**< 像素大小 */
	int			m_nQRMargin;	/**< QR边界 */
	ImageSrc	m_Src;			/**< 图像数据 */
};