
/** 
* @file QrDecoder.h
* @brief Qr码解码函数，可以跨平台
*/


#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include "zxing/common/Counted.h"
#include "zxing/Binarizer.h"
#include "zxing/Result.h"
#include "zxing/ReaderException.h"
#include "zxing/common/GlobalHistogramBinarizer.h"
#include "zxing/common/HybridBinarizer.h"
#include <exception>
#include "zxing/Exception.h"
#include "zxing/common/IllegalArgumentException.h"
#include "zxing/BinaryBitmap.h"
#include "zxing/DecodeHints.h"
#include "zxing/qrcode/QRCodeReader.h"
#include "datatypes.h"

using namespace std;
using namespace zxing;
using namespace zxing::qrcode;

vector<Ref<Result> > decode(Ref<BinaryBitmap> image, DecodeHints hints, float ppt[9], 
							float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo);


// 解码图像明文
char* DecodePublicData(char* decode_string, int nWidth, int nHeight, int nChannel, int &ncLength, float2 pos[4], float ppt[9], 
					   float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, BOOL use_hybrid, BOOL try_harder);
