#pragma once

/************************************************************************/
// 图像几何变换函数，可以跨平台
/************************************************************************/

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float theta, float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点(0, 0)旋转
void PositionTransform(float &x, float &y, float theta);

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float cos_sin[2], float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点(0, 0)旋转
void PositionTransform(float &x, float &y, float cos_sin[2]);

// 对坐标(x, y)旋转angle角度,坐标原点由后两个参数给定
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta, float x0, float y0);

// 对坐标(x, y)旋转angle角度, 绕坐标原点(0, 0)旋转
void PositionTransform(float &x, float &y, float cos_theta, float sin_theta);

// 找到四个数中最大的
float FindMaxBetween4Numbers(float x, float y, float z, float w);

// 找到四个数中最小的
float FindMinBetween4Numbers(float x, float y, float z, float w);

// 图像旋转
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像裁剪
float* ImageCut(float* pSrc, int &nWidth, int &nHeight, int &nRowlen, int nChannel, CLogoRect rect);

// 图像旋转(以左下角为原点)
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, float angle, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像旋转，以某点为中心
float* ImageRotate(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);

// 图像缩放
float* ImageZoom(float* pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, int NewWidth, int NewHeight);