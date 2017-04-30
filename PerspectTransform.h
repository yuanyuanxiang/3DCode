/** 
* @file PerspectTransform.h
* @brief 透视变换类：该类参考自ZXing - PerspectiveTransform.
* @note Copyleft (c), All rights reserved.
*/

#pragma once

#include <vector>
#include "DataTypes.h"

/**
* @class PerspectTransform
* @brief 透视校正类
*/
class PerspectTransform
{
private:
	/// 透视变换矩阵
	float a11, a12, a13, a21, a22, a23, a31, a32, a33;
	/// 默认构造函数
	PerspectTransform();
	/// 构造函数
	PerspectTransform(float a11, float a21, float a31, float a12, float a22, float a32, float a13, float a23, float a33);

public:
	/// 四边形到四边形变换
	static PerspectTransform 
		quadrilateralToQuadrilateral(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3,
		float x0p, float y0p, float x1p, float y1p, float x2p, float y2p, float x3p, float y3p);
	/// 正方形到四边形变换
	static PerspectTransform 
		squareToQuadrilateral(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
	/// 四边形到正方形变换
	static PerspectTransform 
		quadrilateralToSquare(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
	/// 创建伴随矩阵
	PerspectTransform buildAdjoint();
	/// 矩阵乘法
	PerspectTransform times(PerspectTransform & other);
	/// 点变换
	void transformPoints(std::vector<float> &points);

	// 袁沅祥 增加
	inline void PerspectTransform::GetPptMatrix(float ppt[9]) const
	{
		ppt[0] = a11; ppt[1] = a12; ppt[2] = a13;
		ppt[3] = a21; ppt[4] = a22; ppt[5] = a23;
		ppt[6] = a31; ppt[7] = a32; ppt[8] = a33;
	}


	inline void PerspectTransform::GetPptMatrix(float &a11, float &a12, float &a13, 
		float &a21, float &a22, float &a23, 
		float &a31, float &a32, float &a33) const 
	{
		a11 = this->a11; a12 = this->a12; a13 = this->a13;
		a21 = this->a21; a22 = this->a22; a23 = this->a23;
		a31 = this->a31; a32 = this->a32; a33 = this->a33;
	}
};


/// 创建一个透视变换
PerspectTransform createTransform(const float2 pos[4], int dimension, float fmodulesize);

/// 进行透视变换
BYTE *perspectiveTransform(const BYTE* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
						   const PerspectTransform & transform, int nDstSize);