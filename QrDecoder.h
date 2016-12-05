
/************************************************************************/
// Qr码解码函数，可以跨平台
/************************************************************************/


#pragma once

#include "DataTypes.h"

// 解码图像明文
char* DecodePublicData(char* decode_string, int nWidth, int nHeight, int nChannel, int &ncLength,
					   float2 pos[4], float ppt[9], float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, BOOL use_hybrid, BOOL try_harder);
