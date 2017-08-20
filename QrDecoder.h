
/** 
* @file QrDecoder.h
* @brief Qr码解码函数，可以跨平台
*/


#pragma once

#include "ZXingDecoder.h"
using namespace zxing::qrcode;


/**
* @class QrDecoder
* @brief QR码解码器
*/
class QrDecoder : public ZXingDecoder<QRCodeReader>
{
private:
	// QR data
	CQR_Encode* m_qr;

public:
	/**
	* @brief 构造一个QR码解码器
	* @param[in] bUseHybrid 解码参数1
	* @param[in] bTryHarder 解码参数2
	*/
	QrDecoder(BOOL bUseHybrid, BOOL bTryHarder) : 
		ZXingDecoder(DecodeHints::QR_CODE_HINT, bUseHybrid, bTryHarder)
	{
		memset(this, 0, sizeof(QrDecoder));
	}

	~QrDecoder()
	{
		if (m_qr) delete m_qr;
	}

	/// 透视旋转变换
	int perspectTransform(BYTE **pDst, int nMoudleSize) const;

	/// 获得QR码比特矩阵
	inline qrMat* InverseQr()
	{
		if (!m_qr)
		{
			m_qr = new CQR_Encode;
			Decoder QRDecoder = m_READER->getDecoder();
			m_qr->EncodeDataUtf8(QRDecoder.GetErrorCorrectLevel(), QRDecoder.GetQRCodeVersion(), 
				TRUE, QRDecoder.GetMaskingNo(), m_pData, m_nStrlen);
		}
		return m_qr->m_byModuleData;
	}

	/// 获得QR码指针
	inline CQR_Encode* GetQrPointer() const { return m_qr; }

	/// 获得QR码版本
	inline int GetQrVersion() const { return m_READER->getDecoder().GetQRCodeVersion(); }

	/// 获得QR码大小
	inline int GetQrSize() const { return 17 + 4 * GetQrVersion(); }

	/// 获得QR码每模块大小
	inline int GetQrModuleSize() const { return m_READER->m_fModuleSize; }

	/// 获取解码信息
	inline void GetQrInfo(float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo) const 
	{
		Decoder QRDecoder = m_READER->getDecoder();
		fModuleSize = m_READER->m_fModuleSize;
		nLevel = QRDecoder.GetErrorCorrectLevel();
		nVersion = QRDecoder.GetQRCodeVersion();
		nMaskingNo = QRDecoder.GetMaskingNo();
	}

	/// 获取解码信息
	inline void GetBarCodeInfo(BarCodeInfo & qr) const 
	{
		Decoder QRDecoder = m_READER->getDecoder();
		qr.m_fModuleSize = m_READER->m_fModuleSize;
		qr.m_nEcLevel = QRDecoder.GetErrorCorrectLevel();
		qr.m_nVersion = QRDecoder.GetQRCodeVersion();
		qr.m_nMaskingNo = QRDecoder.GetMaskingNo();
		qr.m_nStrLen = m_nStrlen;
		strcpy(qr.m_pData, m_pData);
	}
};