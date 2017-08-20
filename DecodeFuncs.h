/**
* @file DecodeFuncs.h
* @brief 彩色二维码解码函数，ANDROID可用
*/

#include "ImageSrc.h"


/** 
* @brief 对整个二维码图像数据源进行解码
* @remark 此函数是解码彩色二维码的唯一接口
* @param[in]	&pSrc	二维码解码数据源
* @param[out]	*qr		QR二维码信息
* @param[out]	*inner	彩色二维码信息
* @return 成功或失败. 解码QR码成功返回1，解码彩色成功返回2.
*/
BOOL DecodeWholeImage(const DecodeSrcInfo &pSrc, BarCodeInfo *qr, BarCodeInfo *inner);
