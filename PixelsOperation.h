/** 
* @file PixelsOperation.h
* @brief 像素操作函数
* @details 
*		Set...对二维码的模块进行颜色设置；
*		Get...获取二维码模块的颜色平均灰度；
*		可以跨平台使用。
* Copyleft (c), All rights reserved.
*/

#include "DataTypes.h"

#pragma once

// 获取row行col列模块平均色彩(float3)
float3 GetPixel(const BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel);

// 为row行col列模块设置像素值，像素大小由参数指明
void SetPixel(BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel, int R, int G, int B);

/** 
* @brief - 获取像素值 -
* @param[in] *pHead			像素首地址
* @param[in] nPixelSize		每模块像素个数
* @param[in] nRow			行指标
* @param[in] nCol			列指标
* @param[in] nRowlen		每行字节数
* @param[in] nChannel		图像通道数
* @note 获取nRow行nCol列像素像素值
*/
inline COLORREF GetPixelRef(const BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel)
{
	float3 avg = GetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, nChannel);
	return RGB(avg.x, avg.y, avg.z);
}

/** 
* @brief - 设置像素值 -
* @param[in] *pHead			像素首地址
* @param[in] nPixelSize		每模块像素个数
* @param[in] nRow			行指标
* @param[in] nCol			列指标
* @param[in] nRowlen		每行字节数
* @param[in] nChannel		图像通道数
* @param[in]  ref			颜色值
* @note 为nRow行nCol列像素设置像素值
*/
inline void SetPixel(BYTE *pHead, int nPixelSize, int nRow, int nCol, int nRowlen, int nChannel, COLORREF ref)
{
	SetPixel(pHead, nPixelSize, nRow, nCol, nRowlen, nChannel, GetRValue(ref), GetGValue(ref), GetBValue(ref));
}