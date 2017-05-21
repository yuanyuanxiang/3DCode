#pragma once
#include "DataTypes.h"

/** 
* @class ImageInfo
* @brief 图像数据源信息
* @details 该数据结构存放了图像的重要信息, 图像的注释性信息.

* @author yuanyuanxiang
* @version 1.0
* @date 2017/5/14
*/
class ImageInfo
{
public:
	/**
	* @brief 构造一个图像结构
	* @param[in] *pHead				图像数据
	* @param[in] nWidth				图像宽度
	* @param[in] nHeight			图像高度
	* @param[in] nChannel			图像通道
	* @param[in] roi				感兴趣区域
	*/
	ImageInfo(const BYTE *pHead, int nWidth, int nHeight, int nChannel, 
		const RoiRect &roi = 0) : 
		m_pHead(pHead), 
		m_nWidth(nWidth), 
		m_nHeight(nHeight), 
		m_nRowlen(WIDTHBYTES(8 * nChannel * nWidth)),
		m_nChannel(nChannel),
		m_roi(roi),
		m_Background(0) { }

	ImageInfo() { memset(this, 0, sizeof(ImageInfo)); }

	~ImageInfo(void) { }

	/// 文件头标识
	static const char *m_sTag;
	/// 初始化
	static void Init(const char *tag) { m_sTag = tag; }

public:
	/// 图像数据源
	const BYTE *m_pHead;
	/// 图像宽度
	int m_nWidth;
	/// 图像高度
	int m_nHeight;
	/// 图像每行元素个数
	int m_nRowlen;
	/// 图像每像素元素个数
	int m_nChannel;
	/// 感兴趣区域
	RoiRect m_roi;
	/// 背景颜色
	COLORREF m_Background;
};


/**
* @class DecodeSrcInfo
* @brief 二维码解码的数据源结构

* @author yuanyuanxiang
* @version 1.0
* @date 2017/5/21
*/
class DecodeSrcInfo
{
public:
	/// 图像数据源
	ImageInfo m_imgSrc;
	/// QR码解码参数1(交叉二值化)
	BOOL m_bUseHybrid;
	/// QR码解码参数2(强力解码)
	BOOL m_bTryHarder;

	/** 
	* @brief 构造函数
	* @param[in] src		图像数据源
	* @param[in] bUseHybrid	QR码解码参数1
	* @param[in] bTryHarder	QR码解码参数2
	*/
	DecodeSrcInfo(const ImageInfo & src, BOOL bUseHybrid, BOOL bTryHarder) :
		m_imgSrc(src), m_bUseHybrid(bUseHybrid), m_bTryHarder(bTryHarder)
	{
	}

	DecodeSrcInfo() { memset(this, 0, sizeof(DecodeSrcInfo)); }
};


/**
* @class ImageSrc
* @brief 图像类
*/
class ImageSrc : public ImageInfo
{
private:
	BYTE *m_data;
public:
	ImageSrc(int nWidth, int nHeight, int nChannel):
		ImageInfo(NULL, nWidth, nHeight, nChannel)
	{
		m_data = new BYTE[m_nHeight * m_nRowlen * sizeof(BYTE)];
		m_pHead = m_data;
	}

	ImageSrc() { memset(this, 0, sizeof(ImageSrc)); }

	~ImageSrc() { SAFE_DELETE(m_data); }

	inline int GetWidth() const { return m_nWidth; }

	inline int GetHeight() const { return m_nHeight; }

	inline int GetRowlen() const { return m_nRowlen; }

	inline int GetChannel() const { return m_nChannel; }

	inline int GetBPP() const { return 8 * m_nChannel; }

	inline const BYTE* GetHeadAddress() const { return m_pHead; }

	inline BYTE* GetImage() const { return m_data; }
};