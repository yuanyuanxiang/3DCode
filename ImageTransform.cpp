#include "stdafx.h"
#include "DataTypes.h"
#include "ImageTransform.h"
#include "templateFuncs.h"
#include <cmath>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/** - 旋转图像 - 
* @param[in] *pSrc 图像指针
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nChannel 图像通道
* @param[in] angle 旋转角度
* @param[in] x0 旋转中心
* @param[in] y0 旋转中心
* @param[out] &Xmin 旋转后顶点
* @param[out] &Ymin 旋转后顶点
* @param[out] &Xmax 旋转后顶点
* @param[out] &Ymax 旋转后顶点
* @param[out] &NewWidth 新宽度
* @param[out] &NewHeight 新高度
* @param[out] &NewRowlen 新每行字节数
*/
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
				   float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, 
				   int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 为了减少计算，把三角函数放到循环外面
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// 四个顶点顺时针旋转,绕图像中心
	PositionTransform(x1, y1, cos_theta, sin_theta, x0, y0);
	PositionTransform(x2, y2, cos_theta, sin_theta, x0, y0);
	PositionTransform(x3, y3, cos_theta, sin_theta, x0, y0);
	PositionTransform(x4, y4, cos_theta, sin_theta, x0, y0);
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

	for (int i = 0; i < NewWidth; ++i)
	{
		int w = i * nChannel;
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, cos_theta, -sin_theta, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
			{
				pDst[nCurChannel + w + j * NewRowlen] = 
					biLinearInterp(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
			}
		}
	}

	return pDst;
}


/** - 按照rect裁剪图像 -
* @param[in] *pSrc				图像数据
* @param[out] &nWidth			图像宽度
* @param[out] &nHeight			图像高度
* @param[out] &nRowlen			每行字节数
* @param[in] nChannel			图像通道
* @param[in] rect				裁剪区域
* @note 返回裁剪所得图像数据，并且更新宽度、高度和每行字节数
*/
float* ImageCut(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int nChannel, CLogoRect rect)
{
	if (rect == CLogoRect())
		return pSrc;
	CLogoRect cutRect = rect;

	ASSERT (0 <= cutRect.left && cutRect.left <= nWidth && 0 <= cutRect.top && cutRect.top <= nHeight);

	if (cutRect.right > nWidth)
		cutRect.right = nWidth;
	if (cutRect.bottom > nHeight)
		cutRect.bottom = nHeight;
	int nNewWidth = cutRect.Width();
	int nNewHeight = cutRect.Height();
	if (nNewWidth <= 0 || nNewHeight <= 0)
		return NULL;
	int nNewRowlen = nNewWidth * nChannel;
	float* pDst = new float[nNewHeight * nNewRowlen];

	float *pSrcLine = pSrc + cutRect.left * nChannel + (nHeight - cutRect.bottom) * nRowlen;
	float *pDstLine = pDst;
	for (int i = 0; i < nNewHeight; ++i)
	{
		memcpy(pDstLine, pSrcLine, nNewRowlen * sizeof(float));
		pSrcLine += nRowlen;
		pDstLine += nNewRowlen;
	}
	// 更新图像信息
	nWidth = nNewWidth;
	nHeight = nNewHeight;
	nRowlen = nNewRowlen;
	return pDst;
}


/// 坐标旋转变换，输入旋转角度
void PositionTransform(float &x, float &y, float theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


/// 坐标旋转变换，输入选族角度
void PositionTransform(float &x, float &y, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


/// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, cos_sin);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


/// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_sin[2])
{
	float x_temp = x * cos_sin[0] - y * cos_sin[1];
	y = x * cos_sin[1] + y * cos_sin[0];
	x = x_temp;
}


/// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0)
{
	float delta_x = x - x0;
	float delta_y = y - y0;
	PositionTransform(delta_x, delta_y, cos_theta, sin_theta);
	x = x0 + delta_x;
	y = y0 + delta_y;
}


/// 坐标旋转，输入参数为旋转角度的余弦、正弦值
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta)
{
	float x_temp = x * cos_theta - y * sin_theta;
	y = x * sin_theta + y * cos_theta;
	x = x_temp;
}


/// 寻找四个数中最大者
float FindMaxBetween4Numbers(float x, float y, float z, float w)
{
	float Max1, Max2;
	Max1 = x > y ? x : y;
	Max2 = z > w ? z : w;
	return (Max1 > Max2 ? Max1 : Max2);
}


/// 寻找四个数中最小者
float FindMinBetween4Numbers(float x, float y, float z, float w)
{
	float Min1, Min2;
	Min1 = x < y ? x : y;
	Min2 = z < w ? z : w;
	return (Min1 < Min2 ? Min1 : Min2);
}


/** - 旋转图像 -
* @param[in] *pSrc 图像指针
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nChannel 图像通道
* @param[in] angle 旋转角度
* @param[out] &NewWidth 新的宽度
* @param[out] &NewHeight 新的高度
* @param[out] &NewRowlen 新的每行字节数
*/
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
				   float angle, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 为了减少计算，把三角函数放到循环外面
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// 四个顶点顺时针旋转,绕图像左下角
	PositionTransform(x1, y1, cos_theta, sin_theta);
	PositionTransform(x2, y2, cos_theta, sin_theta);
	PositionTransform(x3, y3, cos_theta, sin_theta);
	PositionTransform(x4, y4, cos_theta, sin_theta);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

	for (int i = 0; i < NewWidth; ++i)
	{
		int w = i * nChannel;
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, cos_theta, -sin_theta);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + w + j * NewRowlen] = 
				biLinearInterp(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


/** - 旋转图像 -
* @param[in] *pSrc 图像指针
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nChannel 图像通道
* @param[in] x0 旋转中心
* @param[in] y0 旋转中心
* @param[in] angle 旋转角度
* @param[out] &NewWidth 新的宽度
* @param[out] &NewHeight 新的高度
* @param[out] &NewRowlen 新的每行字节数
*/
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
				   float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen)
{
	// 原始图像四个顶点的坐标
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = 0;							y1 = 0;
	x2 = float(nWidth - 1);			y2 = 0;
	x3 = x2;						y3 = float(nHeight - 1);
	x4 = x1;						y4 = y3;
	// 2015.5.19 为了减少计算，把三角函数放到循环外面
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	// 四个顶点顺时针旋转,绕图像中心
	PositionTransform(x1, y1, cos_theta, sin_theta, x0, y0);
	PositionTransform(x2, y2, cos_theta, sin_theta, x0, y0);
	PositionTransform(x3, y3, cos_theta, sin_theta, x0, y0);
	PositionTransform(x4, y4, cos_theta, sin_theta, x0, y0);
	int Xmin, Xmax, Ymin, Ymax;
	Xmax = int(FindMaxBetween4Numbers(x1, x2, x3, x4));
	Ymax = int(FindMaxBetween4Numbers(y1, y2, y3, y4));
	Xmin = int(FindMinBetween4Numbers(x1, x2, x3, x4));
	Ymin = int(FindMinBetween4Numbers(y1, y2, y3, y4));
	// 新图像宽度、高度、每行字节数的变化
	NewWidth = Xmax - Xmin + 1;
	NewHeight = Ymax - Ymin + 1;
	NewRowlen = nChannel * NewWidth;
	float *pDst = new float[NewRowlen * NewHeight];
	memset(pDst, 0, NewRowlen * NewHeight * sizeof(float));

	for (int i = 0; i < NewWidth; ++i)
	{
		int w = i * nChannel;
		for (int j = 0; j < NewHeight; ++j)
		{
			float x = float(i + Xmin);
			float y = float(j + Ymin);
			PositionTransform(x, y, cos_theta, -sin_theta, x0, y0);
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + w + j * NewRowlen] = 
				biLinearInterp(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, x, y);
		}
	}

	return pDst;
}


/** - 放大图像 -
* @param[in] *pSrc 图像指针
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 图像每行字节数
* @param[in] nChannel 图像通道
* @param[in] NewWidth 新的宽度
* @param[in] NewHeight 新的高度
*/
float* ImageZoom(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int NewWidth, int NewHeight)
{
	int NewRowlen = nChannel * NewWidth;
	float* pDst = new float[NewRowlen * NewHeight];
	float wRatio = 1.f * nWidth / NewWidth;
	float hRatio = 1.f * nHeight / NewHeight;

	for (int i = 0; i < NewWidth; ++i)
	{
		int w = i * nChannel;
		for (int j = 0; j < NewHeight; ++j)
		{
			for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
				pDst[nCurChannel + w + j * NewRowlen] = 
				biLinearInterp(pSrc, nWidth, nHeight, nRowlen, nChannel, nCurChannel, i * wRatio, j * hRatio);
		}
	}

	return pDst;
}