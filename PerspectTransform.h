/************************************************************************
* @file 
* @details 透视变换类：该类参考自ZXing - PerspectiveTransform
* @author 
* @email 
* @version V1.0
* @date 2016/10/4
* Copyleft (c), All rights reserved.
/************************************************************************/

#pragma once
#include <vector>
#include "DataTypes.h"

/**
* @class PerspectTransform - 透视校正类
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

	/// 获取透视矩阵
	void GetPptMatrix(float ppt[9]);

	/// 获取透视矩阵的元素
	void GetPptMatrix(	float &a11, float &a12, float &a13, 
						float &a21, float &a22, float &a23, 
						float &a31, float &a32, float &a33);
};


/// 创建一个透视变换
PerspectTransform createTransform(float2 pos[4], int dimension, float fmodulesize);

/// 进行透视变换
BYTE *PerspectiveTransform(BYTE* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
						   PerspectTransform & transform, 
						   float2 m_fPos[4], int nSymbolSize, int nModulesize, int &nDstSize);