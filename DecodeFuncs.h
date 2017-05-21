/**
* @file DecodeFuncs.h
* @brief 彩色二维码解码函数，ANDROID可用
*/

#include "ImageSrc.h"

// 对整个二维码图像数据源进行解码
BOOL DecodeWholeImage(const DecodeSrc &pSrc, BarCodeInfo *qr, BarCodeInfo *inner);