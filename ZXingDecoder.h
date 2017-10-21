
/** 
* @file ZXingDecoder.h
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

#include "ImageReaderSource.h"
#include "Transform.h"
#include "MedianFilter.h"
#include "PerspectTransform.h"
#include "templateFuncs.h"
#include "medianBlur.h"

using namespace std;
using namespace zxing;

#pragma warning(disable : 4101 4996)

/* 缩放图像到该尺寸进行解码 */
#define IMAGE_ZOOM_TO 320

/**
* @class ZXingDecoder
* @brief ZXing二维码解码器
*/
template <class Type>
class ZXingDecoder
{
protected:
	// param[in]
	BYTE *		m_pImage;				/**< 解码图像[4通道] */
	char *		m_pBuffer;				/**< 解码缓存区 */
	int			m_nWidth;				/**< 解码图像宽度 */
	int			m_nHeight;				/**< 解码图像高度 */
	BOOL		m_bUseHybrid;			/**< use hybrid */
	BOOL		m_bFilter;				/**< 滤波 HINT */
	BOOL		m_bScreenImage;			/**< 是否屏幕截图 */
	DecodeHints m_hint;					/**< HINT */
	// param[out]
	float2		m_pos[4];				/**< 定位元坐标 */
	// QR data
	int			m_nStrlen;				/**< QR码编码数据长度 */
	char		m_pData[MAX_CODEUTF8NUM];/**< QR码编码字符串 */
	int			m_nBackground;			/**< 原QR码背景色 */
	char		m_sLastError[256];		/**< 解码错误信息 */

	/// 二维码解析类的对象
	Ref<Type> m_READER;

public:
	/**
	* @brief 构造一个ZXing解码器
	* @param[in] h HINT
	* @param[in] bUseHybrid 解码参数1
	* @param[in] bTryHarder 解码参数2
	*/
	ZXingDecoder(DecodeHints h, BOOL bUseHybrid = TRUE, BOOL bTryHarder = TRUE)
	{
		memset(this, 0, sizeof(ZXingDecoder));
		m_hint = h;
		m_hint.setTryHarder(TRUE == bTryHarder);
		m_bUseHybrid = bUseHybrid;
		m_bFilter = TRUE;
		m_bScreenImage = FALSE;
	}

	~ZXingDecoder()
	{
		SAFE_DELETE(m_pImage);
		SAFE_DELETE(m_pBuffer);
	}

	// 初始化解码图像
	void SetImgSrc(const BYTE *pHead, int nWidth, int nHeight, int nChannel, const RoiRect & roi);

	// 强力解析QR码
	inline bool DecodeHard(COLORREF Background)
	{
		return DecodeSrcImage() ? true : 
			DecodeBackImage(Background, m_bFilter) ? true : DecodeBackImage(Background, !m_bFilter);
	}

	// 获取上一个错误
	inline const char* GetLastError() const { return m_sLastError; }

	/// 获取解码图像宽度
	inline int GetWidth() const { return m_nWidth; }

	/// 获取解码图像高度
	inline int GetHeight() const { return m_nHeight; }

	/// 是否Use Hybrid
	inline BOOL UseHybrid() const { return m_bUseHybrid; }

	/// 获得背景颜色
	inline int GetBackground() const { return m_nBackground; }

	/// 获得解码图像指针
	inline BYTE* GetImage() const { return m_pImage; }

	/// 获得解码字符串长度
	inline int GetStrlen() const { return m_nStrlen; }

	/// 获取定位坐标
	inline void GetPostion(float2 pos[4]) const { memcpy(pos, m_pos, 4 * sizeof(float2)); }

	/// 获得解码数据
	inline const char* GetData() const { return m_pData; }

	/// 从其他解码器拷贝已经初始化的成员
	template <class T>
	inline void CopyOf(const ZXingDecoder<T> & other)
	{
		m_nWidth = other.GetWidth();
		m_nHeight = other.GetHeight();
		m_bUseHybrid = other.UseHybrid();
		m_pImage = new BYTE[4 * m_nWidth * m_nHeight];
		memcpy(m_pImage, other.GetImage(), 4 * m_nWidth * m_nHeight * sizeof(BYTE));
	}

	/// 是否有可能是截屏图像
	bool IsScreenImage() const { return m_bScreenImage; }

private:
	// 调用ZXing解码QR码图像明文
	bool CallZXingDecode(int nWidth, int nHeight, bool bFlip);

	// 调用ZXing解码原始图像
	bool DecodeSrcImage();

	// 提取背景图像进行解码
	bool DecodeBackImage(COLORREF Background, bool bBlur = true);

	/// 设置当前解码的缓存
	inline void SetDecBuffer(const BYTE* pSrc)
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
		register int d1 = abs(x - c1), d2 = abs(x - c2), d3 = abs(x - c3);
		return d1 < d2 ? (d1 < d3 ? 0 : 2) : (d2 < d3 ? 1 : 2);
	}
};


/**
* @brief 初始化解码图像
* @details 主要进行提取感兴趣区域、缩放等预处理
* @param[in] *pHead 原始图像
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nChannel 图像通道
* @param[in] roi 感兴趣区域
*/
template <class Type>
void ZXingDecoder<Type>::SetImgSrc(const BYTE *pHead, int nWidth, int nHeight, int nChannel, const RoiRect & roi)
{
	//////////////////////////////////////////////////////////////////////////
	/// Step1 图像预处理
	ASSERT(nChannel >= 3);
	m_bFilter = TRUE;
	int nRowlen = WIDTHBYTES(nWidth * 8 * nChannel);// 计算图像每行字节数
	BYTE *pRoi = ImageROI(pHead, nWidth, nHeight, nRowlen, roi);// 提取感兴趣区域
#ifdef _DEBUG
	ImageWrite(".\\ImageROI.txt", pRoi, nWidth, nHeight, nRowlen);
#endif
	double fRate = max((double)nWidth / nHeight, (double)nHeight / nWidth);// 宽高比
	if(fRate >= 1.5)// 提取截屏图像
	{
		m_bScreenImage = TRUE;
		m_bFilter = FALSE;// 假定截图不滤波
		RoiRect cutAero;
		GetQrRect(pRoi, nWidth, nHeight, nChannel, nRowlen, cutAero);
		BYTE *pBackup = pRoi;
		pRoi = ImageROI(pRoi, nWidth, nHeight, nRowlen, cutAero);
		SAFE_DELETE(pBackup);
#ifdef _DEBUG
		ImageWrite(".\\CutImage.txt", pRoi, nWidth, nHeight, nRowlen);
#endif
	}
	LimitImage(&pRoi, nWidth, nHeight, nRowlen, nChannel, IMAGE_ZOOM_TO);// 缩放
	m_nWidth = nWidth;  // 图像宽度
	m_nHeight = nHeight; // 图像高度

	/// Step2 获取解码字符串
	SAFE_DELETE(m_pImage);
	m_pImage = GetDecodeString(pRoi, m_nWidth, m_nHeight, nRowlen);
	// 清理缓存
	SAFE_DELETE(pRoi);

	//////////////////////////////////////////////////////////////////////////
}


/**
* @brief K_means 聚类分割.
* @param[in] *pHead 图像数据
* @param[in] nRowBytes 每行字节数
* @param[in] roi 图像感兴趣区域
* @param[in] nMaxepoches 最大迭代次数
* @note (1)必须是灰度图像;(2)此函数将修改图像数据pHead.
*/
template <class Type>
void ZXingDecoder<Type>::K_means(BYTE* pHead, int nRowBytes, RoiRect roi, int nMaxepoches)
{
	// 聚类区域的尺寸
	const int nWidth = roi.Width();
	const int nHeight = roi.Height();
	const int nRowlen = nWidth;// 聚类标记每行元素个数
	BYTE* Cluster = new BYTE[nHeight * nRowlen];// 聚类标记TAG
	// 聚类中心
	int Center[3], oldCenter[3];
	int sumCenter[3];// 聚类中心求和
	int sum[3];// 每个聚类中心元素个数
	// 生成3个中心
	InitializeClusterCenters<int, BYTE>(Center, 3, pHead, nRowBytes, roi);
	/* 多次迭代直至收敛，本次试验迭代nMaxepoches次 */
	for (int it = 0; it < nMaxepoches; ++it)
	{
		sumCenter[0] = sumCenter[1] = sumCenter[2] = 0;
		sum[0] = sum[1] = sum[2] = 0;
		/* 求出每个样本点距应该属于哪一个聚类 */
		BYTE *pLeftTop = pHead + roi.top * nRowBytes + roi.left;
		int y = - roi.left;
		for (int j = roi.top; j < roi.bottom; ++j)
		{
			BYTE *pCur = pLeftTop;
			for (int i = roi.left; i < roi.right; ++i)
			{
				/* 当前像素的浮点值 */
				register int curPixel = *pCur++;
				register int tag = CLUSTER(curPixel, Center[0], Center[1], Center[2]);
				// 将(i, j)标记为tag
				Cluster[i + y] = tag;
				sumCenter[tag] += curPixel;
				++sum[tag];
			}
			pLeftTop += nRowBytes;
			y += nRowlen;
		}
		/* 保存上一个结果，并更新聚类中心 */
		oldCenter[0] = Center[0];
		oldCenter[1] = Center[1];
		oldCenter[2] = Center[2];
		if (sum[0]) Center[0] = sumCenter[0] / sum[0];
		if (sum[1]) Center[1] = sumCenter[1] / sum[1];
		if (sum[2]) Center[2] = sumCenter[2] / sum[2];
		int diff = abs(oldCenter[0] - Center[0]) + abs(oldCenter[1] - Center[1])
			+ abs(oldCenter[2] - Center[2]);
#ifdef _DEBUG
		TRACE(" * 迭代次数 = %d：\n", it + 1);
		for (int i = 0; i < 3; ++i)
		{
			TRACE("C[%d] = %.6f\t%d个\t", i, (float)Center[i], sum[i]);
		}
		TRACE("\n");
#endif // _DEBUG
		// 当前后相邻两次中心变化较小时（0.01个像素）退出
		if (diff < 3)
			break;
	}

	// 寻找背景值对应的类号
	int backIdx = CLUSTER(m_nBackground, Center[0], Center[1], Center[2]);
	int backVal = Center[backIdx] < 128 ? 0 : 255, foreVal = 255 - backVal;// 背景、前景

	// 对原始图像进行分割
	BYTE *pSrc0 = Cluster;
	BYTE *pDst0 = pHead + roi.left + roi.top * nRowBytes;
	for (int j = 0; j < nHeight; ++j)
	{
		BYTE *pSrc = pSrc0;
		BYTE *pDst = pDst0;
		for (int i = 0; i < nWidth; ++i)
		{
			*pDst ++ = (*pSrc++ == backIdx) ? backVal : foreVal;
		}
		pSrc0 += nRowlen;
		pDst0 += nRowBytes;
	}
	delete [] Cluster;
}


/** 
* @brief 解码未经滤波和K分割的原始图像
*/
template <class Type>
bool ZXingDecoder<Type>::DecodeSrcImage()
{
	int W = m_nWidth, H = m_nHeight;

	// 2017-7-27 提高解码效率, 以下语句不可少
	SetDecBuffer(m_pImage);
	if (!CallZXingDecode(W, H, false))
		if (CallZXingDecode(W, H, true))
			ImageFlipV(m_pImage, W, H, 4 * W);

	return m_nStrlen;
}


/**
* @brief 提取图像的背景图像(QR码)进行解码
* @param[in] Background QR码的背景色
* @param[in] bBlur 是否进行滤波
*/
template <class Type>
bool ZXingDecoder<Type>::DecodeBackImage(COLORREF Background, bool bBlur)
{
	int W = m_nWidth, H = m_nHeight;

	//////////////////////////////////////////////////////////////////////////
	// 某些图像需要滤波(滤波核边长为3，不宜过大)
	// 针对4通道图像采用快速中值滤波，否则调用MedianFilter
	if (bBlur)
	{
		FastMedianFilter(m_pImage, W, H, 4 * W);
		//medianBlur_SortNet<MinMax8u, MinMaxVec8u>(m_pImage, W, H, 4 * W, 4);
#ifdef _DEBUG
		ImageWrite(".\\MedianFilter.txt", m_pImage, W, H, 4 * W);
#endif
	}

	BYTE* gray = Rgb2Gray(m_pImage, W, H, 4 * W);// 灰度图像
	m_nBackground = RgbColorRef2Gray(Background);// 背景颜色
	int nRowlenNew = WIDTHBYTES(W * 8);// 灰度图像每行字节数
	BYTE *p = (BYTE*)gray;
#ifdef _DEBUG
	ImageWrite(".\\Rgb2Gray.txt", gray, W, H, nRowlenNew);
#endif
	// -------------------
	// | a11 | a12 | a13 |
	// | a21 | a22 | a23 |
	// | a31 | a32 | a33 |
	// -------------------
	// 对角线
	K_means(p, nRowlenNew, RoiRect(0, 0, W/3, H/3), 10);// a11
	K_means(p, nRowlenNew, RoiRect(W / 3, H / 3, W * 2 / 3, H * 2 / 3), 10);// a22
	K_means(p, nRowlenNew, RoiRect(W * 2 / 3, H * 2 / 3, W, H), 10);// a33
	// 左下角
	K_means(p, nRowlenNew, RoiRect(0, H / 3, W / 3, H * 2 / 3), 10);// a21
	K_means(p, nRowlenNew, RoiRect(0, H * 2 / 3, W / 3, H), 10);// a31
	K_means(p, nRowlenNew, RoiRect(W / 3, H * 2 / 3, W * 2 / 3, H), 10);// a32
	// 右上角
	K_means(p, nRowlenNew, RoiRect(W / 3, 0, W * 2 / 3, H / 3), 10);// a12
	K_means(p, nRowlenNew, RoiRect(W * 2 / 3, 0, W, H / 3), 10);// a13
	K_means(p, nRowlenNew, RoiRect(W * 2 / 3, H / 3, W, H * 2 / 3), 10);// a23
#ifdef _DEBUG
	ImageWrite(".\\K_means.txt", gray, W, H, nRowlenNew);
#endif
	BYTE* gray_decstring = GetDecodeString(gray, W, H, nRowlenNew);
	SetDecBuffer(gray_decstring);
	if (!CallZXingDecode(W, H, false))
		if (CallZXingDecode(W, H, true))
			ImageFlipV(m_pImage, W, H, 4 * W);
	SAFE_DELETE(gray_decstring);
	SAFE_DELETE(gray);
	return m_nStrlen;
}


/** 
* @brief 解码图像明文
* @param[in] nWidth 解码缓存图像宽度
* @param[in] nHeight 解码缓存图像高度
* @param[in] bFlip 是否翻转图像进行解码
*/
template <class Type>
bool ZXingDecoder<Type>::CallZXingDecode(int nWidth, int nHeight, bool bFlip)
{
	if (bFlip)
	{
		ImageFlipV(m_pBuffer, nWidth, nHeight, 4 * nWidth);
	}

	// 尝试创建LuminanceSource
	Ref<LuminanceSource> source;
	try
	{
		source = ImageReaderSource::create(m_pBuffer, nWidth, nHeight, 3);
	}
	catch (const zxing::IllegalArgumentException &e)
	{
		TRACE(" * zxing::IllegalArgumentException: %s\n", e.what());
		strcpy(m_sLastError, e.what());
		return false;
	}
	catch (...)
	{
		return false;
	}

	// 尝试进行解码
	vector<Ref<Result> > results;
	Ref<Binarizer> binarizer;
	try
	{
		// 获得二值化图像
		binarizer = m_bUseHybrid ? new HybridBinarizer(source) : 
			new GlobalHistogramBinarizer(source);
		Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
		// 初始化解码器
		m_READER = new Type;
		// 进行解码
		Ref<Result> result = m_READER->decode(binary, m_hint);

		results = vector<Ref<Result> >(1, result);
	}
	catch (const zxing::ReaderException& e)
	{
		TRACE(" * zxing::ReaderException: %s\n", e.what());
		strcpy(m_sLastError, e.what());
		return false;
	}
	catch (const zxing::IllegalArgumentException& e)
	{
		TRACE(" * zxing::IllegalArgumentException: %s\n", e.what());
		strcpy(m_sLastError, e.what());
		return false;
	}
	catch (const zxing::Exception& e)
	{
		TRACE(" * zxing::Exception: %s\n", e.what());
		strcpy(m_sLastError, e.what());
		return false;
	}
	catch (const std::exception& e)
	{
		TRACE(" * std::exception: %s\n", e.what());
		strcpy(m_sLastError, e.what());
		return false;
	}
	catch (...)
	{
		return false;
	}

	if (results.size() > 0)
	{
		string temp = results[0]->getText()->getText();
		const char *chr = temp.c_str();
		m_nStrlen = temp.length();
		if (m_nStrlen >= MAX_CODEUTF8NUM)
			return false;
		strcpy(m_pData, chr);
#ifdef _DEBUG
		TRACE(" * Data[%d]: %s \n", m_nStrlen, chr);
		TRACE(" * Format: %s \n", BarcodeFormat::barcodeFormatNames[results[0]->getBarcodeFormat()]);
#endif // _DEBUG
		for (int j = 0; j < results[0]->getResultPoints()->size(); j++)
		{
			// p0-左下; p1-左上(原点); p2-右上; p3-alignmentPattern
			m_pos[j].x = results[0]->getResultPoints()[j]->getX();
			m_pos[j].y = results[0]->getResultPoints()[j]->getY();
#ifdef _DEBUG
			TRACE(" * Point[%d]:(%f, %f) \n", j, m_pos[j].x, m_pos[j].y);
#endif // _DEBUG
		}
		return true;
	}

	return false;
}
