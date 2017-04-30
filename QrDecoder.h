
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
#include "QR_Encode.h"
#include "K_Means.h"

using namespace std;
using namespace zxing;
using namespace zxing::qrcode;

/* 缩放图像到该尺寸进行解码 */
#define IMAGE_ZOOM_TO 320

class QrDecoder
{
private:
	// param[in]
	char *		m_pImage;				/**< 解码图像[4通道] */
	char *		m_pBuffer;				/**< 解码缓存区 */
	int			m_nWidth;				/**< 解码图像宽度 */
	int			m_nHeight;				/**< 解码图像高度 */
	// param[out]
	float2		m_pos[4];				/**< 定位元坐标 */
	float		m_fModuleSize;			/**< QR码模块尺寸 */
	int			m_nLevel;				/**< QR码纠错等级 */
	int			m_nVersion;				/**< QR码版本号 */
	int			m_nMaskingNo;			/**< QR码掩码号 */
	// QR data
	int			m_nStrlen;				/**< QR码编码数据长度 */
	static char	m_pData[MAX_CODEUTF8NUM];/**< QR码编码字符串 */
	CQR_Encode* m_qr;					/**< 原QR码 */
	int			m_nBackground;			/**< 原QR码背景色 */
public:
	QrDecoder()
	{
		memset(this, 0, sizeof(QrDecoder));
	}

	~QrDecoder()
	{
		SAFE_DELETE(m_pImage);
		SAFE_DELETE(m_pBuffer);
		if (m_qr) delete m_qr;
	}

	// 初始化解码图像
	void Init(const BYTE *pHead, int nWidth, int nHeight, int nChannel, const RoiRect & roi);

	// 解码QR码
	bool Decode(COLORREF Background, BOOL bUseHybrid, BOOL bTryHarder);

	// 透视旋转变换
	int perspectTransform(BYTE **pDst, int nMoudleSize) const;

	/// 获得QR码比特矩阵
	inline qrMat* InverseQr()
	{
		if (!m_qr)
		{
			m_qr = new CQR_Encode;
			m_qr->EncodeDataUtf8(m_nLevel, m_nVersion, TRUE, m_nMaskingNo, m_pData, m_nStrlen);
		}
		return m_qr->m_byModuleData;
	}

	/// 获取解码图像宽度
	inline int GetWidth() const { return m_nWidth; }

	/// 获取解码图像高度
	inline int GetHeight() const { return m_nHeight; }

	/// 获得解码图像指针
	inline char* GetImage() const { return m_pImage; }

	/// 获得解码字符串长度
	inline int GetStrlen() const { return m_nStrlen; }

	/// 获得QR码大小
	inline int GetQrSize() const { return 17 + 4 * m_nVersion; }

	/// 获取解码信息
	inline void GetQrInfo(float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo) const 
	{
		fModuleSize = m_fModuleSize;
		nLevel = m_nLevel;
		nVersion = m_nVersion;
		nMaskingNo = m_nMaskingNo;
	}

	/// 获得解码数据
	inline const char* GetQrData() const { return m_pData; }

	/// 获取定位坐标
	inline void GetPostion(float2 pos[4]) const { memcpy(pos, m_pos, 4 * sizeof(float2)); }

private:
	// 调用ZXing解码QR码
	vector<Ref<Result> > decode(Ref<BinaryBitmap> image, DecodeHints hints, 
		float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo);

	// 解码图像明文
	bool DecodePublicData(bool bFlip, BOOL use_hybrid, BOOL try_harder);

	/// 设置当前解码的缓存
	inline void SetImage(const char* pSrc)
	{
		SAFE_DELETE(m_pBuffer);
		m_pBuffer = new char[4 * m_nWidth * m_nHeight];
		memcpy(m_pBuffer, pSrc, 4 * m_nWidth * m_nHeight * sizeof(char));
	}

	// K_means 聚类分割
	void K_means(BYTE* pHead, int nRowBytes, RoiRect roi, int nMaxepoches);

	/// x到哪个中心更近，返回最近点的标号
	inline int CLUSTER(int x, int c1, int c2, int c3) const
	{
		int d1 = FastAbs(x - c1), d2 = FastAbs(x - c2), d3 = FastAbs(x - c3);
		return d1 < d2 ? (d1 < d3 ? 0 : 2) : (d2 < d3 ? 1 : 2);
	}
};