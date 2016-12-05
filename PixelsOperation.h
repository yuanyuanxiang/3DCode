/************************************************************************
* @file 
* @details 
*		Set...对二维码的模块进行颜色设置
*		Get...获取二维码模块的颜色平均灰度
*		可以跨平台使用
* @author 
* @email 
* @version V1.0
* @date 2016/10/4
* Copyleft (c), All rights reserved.
************************************************************************/

#include "DataTypes.h"

#pragma once

// 获取row行col列模块平均色彩(RGB ref)
COLORREF GetPixelRef(BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel);

// 获取row行col列模块平均色彩(float3)
float3 GetPixel(BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel);

// 为row行col列模块设置像素值，像素大小由参数指明
void SetPixel(BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel, COLORREF ref = RGB(255, 255, 255));

// 为row行col列模块设置像素值，像素大小由参数指明
void SetPixel(BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel, int R, int G, int B);
