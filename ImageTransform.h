#pragma once

/** 
* @file ImageTransform.h
* @brief 图像几何变换函数，可以跨平台
* @author 袁沅祥
*/

#include "DataTypes.h"
#include <cmath>

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
protected:
	const float *m_pSrc;/**< 图像数据 */
	int m_nWidth;		/**< 图像宽度 */
	int m_nHeight;		/**< 图像高度 */
	int m_nRowlen;		/**< 图像每行字节数 */
	int m_nChannel;		/**< 每像素元素数 */
	float *data;

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
	/**
	* @brief 图像变换类的构造函数
	* @param[in] *pSrc 图像指针
	* @param[in] nWidth 图像宽度
	* @param[in] nHeight 图像高度
	* @param[in] nRowlen 图像每行字节数
	* @param[in] nChannel 图像通道
	*/
	ImageTransform(const float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel) 
	{
		m_pSrc = pSrc;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_nRowlen = nRowlen;
		m_nChannel = nChannel;
		data = NULL;
	}

	~ImageTransform() { if (data) delete [] data; }

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
	float* ImageRotate(const PositionTransform &pt, int &NewWidth, int &NewHeight, CLogoRect &dstArea) const ;

	// 图像感兴趣区域
	float* ImageRoi(const CLogoRect &roi) const ;

	// 图像缩放
	float* ImageZoom(int NewWidth, int NewHeight) const ;
};
