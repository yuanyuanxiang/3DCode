#pragma once

/** 
* @file ImageTransform.h
* @brief 图像几何变换函数，可以跨平台
* @author 袁沅祥
*/

#include "DataTypes.h"

/**
* @class PositionTransform
* @brief 点变换类
*/
class PositionTransform
{
public:
	float theta;		/**< 旋转角度 */
	float sin_theta;	/**< 角度正弦 */
	float cos_theta;	/**< 角度余弦 */
	float x0, y0;		/**< 旋转中心 */

public:
	// 旋转角度angle，旋转中心为原点
	PositionTransform(float angle) : 
		theta(angle), sin_theta(sin(angle)), cos_theta(cos(angle)), x0(0), y0(0) { }

	// 旋转角度angle，旋转中心为x, y
	PositionTransform(float angle, float x, float y) : 
		theta(angle), sin_theta(sin(angle)), cos_theta(cos(angle)), x0(x), y0(y) { }

	/// 坐标正向（逆时针）旋转
	inline void Transform(float &x, float &y) const 
	{
		float dx = x - x0, dy = y - y0;
		float x_temp = dx * cos_theta - dy * sin_theta;
		dy = dx * sin_theta + dy * cos_theta;
		dx = x_temp;
		x = x0 + dx;
		y = y0 + dy;
	}


	/// 坐标反向（顺时针）旋转
	inline void InvTransform(float &x, float &y) const 
	{
		float dx = x - x0, dy = y - y0;
		float x_temp = dx * cos_theta - dy * -sin_theta;
		dy = dx * -sin_theta + dy * cos_theta;
		dx = x_temp;
		x = x0 + dx;
		y = y0 + dy;
	}
};

/**
* @class ImageTransform
* @brief 图像变换类
*/
class ImageTransform
{
private:
	int m_nRefCount;
protected:
	const float *m_pSrc;/**< 图像数据 */
	int m_nWidth;		/**< 图像宽度 */
	int m_nHeight;		/**< 图像高度 */
	int m_nRowlen;		/**< 图像每行字节数 */
	int m_nChannel;		/**< 每像素元素数 */

	/** 
	* @brief 双线性插值获取浮点 (x, y) 处的函数值
	* @param[in] nCurChannel	当前通道
	* @param[in] x				插值坐标
	* @param[in] y				插值坐标
	*/
	inline float biLinearInterp(int nCurChannel, float x, float y) const 
	{
		/** 
		// 双线性插值的示意图:
		// Ans2：		(x4, y4)				(x3, y3)
		//							(x, y)
		// Ans1：		(x1, y1)				(x2, y2)
		*/
		int x1 = int(x), y1 = int(y), x3 = x1 + 1, y3 = y1 + 1;
		// 左下角的点
		const float* pLB = m_pSrc + nCurChannel + x1 * m_nChannel + y1 * m_nRowlen;
		// 对越界的处理
		return (x1 < 0 || x3 >= m_nWidth || y1 < 0 || y3 >= m_nHeight) ? 0 : 
			(*pLB * (x3 - x) + *(pLB + m_nChannel) * (x - x1)) * (y3 - y) 
			+ (*(pLB + m_nRowlen) * (x3 - x) + *(pLB + m_nChannel + m_nRowlen) * (x - x1)) * (y - y1);
	}

public:
	/// 置零
	ImageTransform()
	{
		memset(this, 0, sizeof(ImageTransform));
	}

	/**
	* @brief 图像变换类的构造函数
	* @param[in] *pSrc 图像指针
	* @param[in] nWidth 图像宽度
	* @param[in] nHeight 图像高度
	* @param[in] nChannel 图像通道
	* @param[in] nRefCounr 引用计数
	* @note 如果图像来源是外部已有图像，则引用计数默认为1即可；\n
	*		如果是新建立的图像，表示未被其他指针引用，则引用计数填0。
	*/
	ImageTransform(const float* pSrc, int nWidth, int nHeight, int nChannel, int nRefCount = 1) 
	{
		m_pSrc = pSrc;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_nRowlen = nChannel * nWidth;
		m_nChannel = nChannel;
		m_nRefCount = nRefCount + 1;
	}

	/**
	* @brief 拷贝构造函数
	* @warning other引用赋值给当前对象，然后other引用自增
	*/
	ImageTransform(ImageTransform &other)
	{
		m_pSrc = other.m_pSrc;
		m_nWidth = other.m_nWidth;
		m_nHeight = other.m_nHeight;
		m_nRowlen = other.m_nRowlen;
		m_nChannel = other.m_nChannel;
		m_nRefCount = other.GetRef();
		other.AddRef();
	}

	/**
	* @brief 析构函数
	* @note 引用自减1，若等于0则删除内存
	*/
	~ImageTransform()
	{
		if (0 == --m_nRefCount)
			SAFE_DELETE(m_pSrc);
	}

	inline void AddRef() { ++m_nRefCount; }

	inline int GetRef() const { return m_nRefCount; }

	inline operator const float*() const { return m_pSrc; }

	inline const float* GetImage() const { return m_pSrc; }

	inline int GetWidth() const { return m_nWidth; }

	inline int GetHeight()const { return m_nHeight; }

	inline int GetRowlen() const { return m_nRowlen; }

	inline int GetChannel() const { return m_nChannel; }

	/// 寻找四个数中最大者
	inline float FindMaxBetween4Numbers(float x, float y, float z, float w) const 
	{
		float Max1, Max2;
		Max1 = x > y ? x : y;
		Max2 = z > w ? z : w;
		return (Max1 > Max2 ? Max1 : Max2);
	}


	/// 寻找四个数中最小者
	inline float FindMinBetween4Numbers(float x, float y, float z, float w) const 
	{
		float Min1, Min2;
		Min1 = x < y ? x : y;
		Min2 = z < w ? z : w;
		return (Min1 < Min2 ? Min1 : Min2);
	}

	// 图像旋转
	ImageTransform ImageRotate(const PositionTransform &pt, CLogoRect &dstArea) const ;

	// 图像感兴趣区域
	ImageTransform ImageRoi(const CLogoRect &roi) const ;

	// 图像缩放
	ImageTransform ImageZoom(int NewWidth, int NewHeight) const ;
};
