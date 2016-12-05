#ifdef WIN32

#pragma once
#include "CyImage.h"
#include "QR_Encode.h"

#include <vector>
using namespace std;


// QR常规编码方法
BOOL EncodeData(CString strEncodeData, CQR_Encode* pQR_Encode, int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo);

// 将编码所得二维码转换为图像显示，返回前景色索引
vector<CPixelPoint> Convert2Image(CyImage* pImage, int nPixelSize, COLORREF ForegroundColor, COLORREF BackgroundColor,
								  CLogoRect LogoRect, CQR_Encode* pQR_Encode, BOOL bForegroundColorReturned = TRUE);

#endif