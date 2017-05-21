#pragma once
#include "DataTypes.h"

/** 
* @class ImageSrc
* @brief 图像数据源
* @details 该数据结构存放了图像的重要信息, 图像的注释性信息.

* @author yuanyuanxiang
* @version 1.0
* @date 2017/5/14
*/
class ImageSrc
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
	ImageSrc(const BYTE *pHead, int nWidth, int nHeight, int nChannel, 
		const RoiRect &roi = 0) : 
		m_pSrc(pHead), 
		m_nWidth(nWidth), 
		m_nHeight(nHeight), 
		m_nRowlen(WIDTHBYTES(8 * nChannel * nWidth)),
		m_nChannel(nChannel),
		m_roi(roi),
		m_Background(0) { }

	~ImageSrc(void) { }

	/// 文件头标识
	static const char *m_sTag;
	/// 图像数据源
	const BYTE *m_pSrc;
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

	/// 初始化
	static void Init(const char *tag) { m_sTag = tag; }
};


/**
* @class DecodeSrc
* @brief 二维码解码的数据源结构

* @author yuanyuanxiang
* @version 1.0
* @date 2017/5/21
*/
class DecodeSrc
{
public:
	/// 图像数据源
	ImageSrc m_imgSrc;
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
	DecodeSrc(const ImageSrc & src, BOOL bUseHybrid, BOOL bTryHarder) :
		m_imgSrc(src), m_bUseHybrid(bUseHybrid), m_bTryHarder(bTryHarder)
	{
	}
};