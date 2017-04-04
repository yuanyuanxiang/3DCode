#include "QR_Encode.h"
#include <vector>
using namespace std;

/**
* @file DecodeFuncs.h
* @brief 彩色二维码解码函数，ANDROID可用
*/

/* 缩放图像到该尺寸进行解码 */
#define IMAGE_ZOOM_TO 320

#ifndef NEW_VERSION /// 开启这个宏，表示采用新版的编码解码函数

// 判断索引(i行, j列)不在矩形内
BOOL IndexNotInRect(int i, int j, const CLogoRect &rect);

// 经过常规编码后，获取前景色索引
vector<CPixelPoint> GetForegroundColorIndex(const CQR_Encode* pQR_Encode, BOOL bForegroundColorReturned = TRUE, 
											CLogoRect logoRect = CLogoRect(0, 0, 0, 0));

// 解码彩色密文(内部调用函数)
BYTE* DecodePrivateData(const BYTE *pHead, int nRowlen, int nChannel, int nPixelSize, int nSymbolSize,
						COLORREF QREncodeColor1, COLORREF QREncodeColor2,
						int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, 
						char* strPublicString, int ncLength, int &strLen, int &nInnerecLevel, int &nInnerMask);
#endif

/// 解码彩色图像
BOOL DecodeWholeImage(char* pDst[2], const BYTE *pHead, int nWidth, int nHeight, int nChannel, 
					  float2 m_fPos[4], float &m_fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, 
					  BOOL bUseHybrid, BOOL bTryHarder, 
					  COLORREF Foreground, COLORREF Background, 
					  COLORREF QREncodeColor1, COLORREF QREncodeColor2, 
					  int &strLen, int &nInnerecLevel, int &nInnerMask, 
					  const RoiRect roi = RoiRect(0, 0, 0, 0));

/// 解码彩色图像
void DecodeWholeImage(char* pDst[2], const int *pIntHead, int nWidth, int nHeight, 
					  float2 m_fPos[4], float &m_fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, 
					  BOOL bUseHybrid, BOOL bTryHarder, 
					  COLORREF Foreground, COLORREF Background, 
					  COLORREF QREncodeColor1, COLORREF QREncodeColor2, 
					  int &strLen, int &nInnerecLevel, int &nInnerMask, 
					  const RoiRect roi = RoiRect(0, 0, 0, 0));

/// 解码彩色图像
void EasyDecode(char* pDst[2], const BYTE *pHead, int nWidth, int nHeight, int nChannel, 
				COLORREF Foreground, COLORREF Background, 
				COLORREF QREncodeColor1, COLORREF QREncodeColor2, 
				const RoiRect roi = RoiRect(0, 0, 0, 0));