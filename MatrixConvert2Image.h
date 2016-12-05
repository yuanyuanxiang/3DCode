#ifdef WIN32

#include "CyImage.h"
#pragma once

#define COLOR_FOREGROUND	0		//前景色
#define COLOR_BACKGROUND	1		//背景色
#define COLOR_QRCOLOR1		2		//彩色一(前景)
#define COLOR_QRCOLOR2		3		//彩色二(背景)
#define MAX_MODULESIZE		177		//QR码最大尺寸

// 寻找四个元素之间最小的，返回其标号
int FindMinValueIndex(COLORREF x, COLORREF y, COLORREF z, COLORREF w);

// 将二维码矩阵转换为图像
BOOL Matrix2Image(BYTE* bDMatrix, int nSymbleSize, CyImage* pImage, int nPixelSize, COLORREF ForegroundColor, COLORREF BackgroundColor);

// 将二维码矩阵转换为图像
BOOL Matrix2ColorImage(BYTE bMatrix[MAX_MODULESIZE][MAX_MODULESIZE], int nSymbleSize, CyImage* pImage, int nPixelSize, 
					   COLORREF ForegroundColor, COLORREF BackgroundColor, COLORREF QREncodeColor1, COLORREF QREncodeColor2);

#endif // WIN32