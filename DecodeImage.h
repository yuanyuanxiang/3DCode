#ifdef WIN32

#pragma once

#include "CyImage.h"

/************************************************************************/
// 本文件包含解析QR码的函数
/************************************************************************/

// 获取待解码的字符串
BYTE* GetDecodeString(CyImage* pImage);

// 获取待解码字串
void GetDecodeString(BYTE* pDst, CyImage* pImage);

// 解码图像
BOOL DecodeImage(CyImage* pImage, CString &strDecodeString, int &ncLength, float2 pos[4], float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, BOOL use_hybrid, BOOL try_harder);

#endif // WIN32