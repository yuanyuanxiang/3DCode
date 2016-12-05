#include "DataTypes.h"

/************************************************************************/
/// 彩色二维码解码函数，ANDROID可用
/************************************************************************/

/* 缩放图像到该尺寸进行解码 */
#define IMAGE_ZOOM_TO 320


/// 解码彩色图像
BOOL DecodeWholeImage(char* pDst[2], BYTE *pHead, int nWidth, int nHeight, int nChannel, 
					  float2 m_fPos[4], float &m_fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, 
					  BOOL bUseHybrid, BOOL bTryHarder, 
					  COLORREF Foreground, COLORREF Background, 
					  COLORREF QREncodeColor1, COLORREF QREncodeColor2, 
					  int &strLen, int &nInnerecLevel, int &nInnerMask, 
					  RoiRect roi = RoiRect(0, 0, 0, 0));

/// 解码彩色图像
void DecodeWholeImage(char* pDst[2], int *pIntHead, int nWidth, int nHeight, 
					  float2 m_fPos[4], float &m_fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, 
					  BOOL bUseHybrid, BOOL bTryHarder, 
					  COLORREF Foreground, COLORREF Background, 
					  COLORREF QREncodeColor1, COLORREF QREncodeColor2, 
					  int &ncLength, int &strLen, int &nInnerecLevel, int &nInnerMask, 
					  RoiRect roi = RoiRect(0, 0, 0, 0));

/// 解码彩色图像
void EasyDecode(char* pDst[2], BYTE *pHead, int nWidth, int nHeight, int nChannel, 
				COLORREF Foreground, COLORREF Background, 
				COLORREF QREncodeColor1, COLORREF QREncodeColor2, 
				RoiRect roi = RoiRect(0, 0, 0, 0));