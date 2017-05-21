#ifdef WIN32

#pragma once

#include "atlimage.h"
#include "DataTypes.h"
#include "ImageSrc.h"

/** 
* @file CyImage.h
* @brief 图像读取/写入/处理类
* @author 袁沅祥
*/

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }/**< 安全删除指针p */
#endif

#ifndef RAD
#define RAD(i) (PI * (i) / 180.0f)	/**< 角度转弧度 */
#endif

/**
* @class CyImage
* @brief 继承自CImage 
* @author 袁沅祥
* @date 2015年5月
* @warning 仅限在ATL环境
* @remark yuan_yuanxiang@163.com
*/
class CyImage : public CImage
{
public:
	// 构造函数
	CyImage();
	// 析构函数
	~CyImage();
	// 拷贝构造函数
	CyImage(const CyImage &pImage);
	// 赋值运算符重载
	CyImage operator = (const CyImage &pImage);

	float* GetFloatDataHead() const;										//获取浮点数据头指针
	int GetFloatDataRowlen() const;											//获取每行浮点数据个数
	BOOL SetFloatData(float* pSrc, int nRowlen, int nHeight);				//设置浮点数据
	float GetMaximum() const;												//获取最大值
	float GetMinimum() const;												//获取最小值

	void FlipH();															//水平翻转图像
	void FlipV();															//垂直翻转图像
	void Transpose();														//转置图像
	void Rotate(float degree = 90.f);										//图像按角度旋转
	void Zoom(float rate = 2.f);											//图像按倍数缩放
	void Zoom(int nNewWidth, int nNewHeight);								//图像按像素缩放
	void Cut(CLogoRect rect = CLogoRect(0, 0, 0, 0));						//图像裁剪
	void ToGray();															//转换为灰度图
	BOOL ChangeBPP(UINT bpp);												//修改图像位深度

	void MemcpyByteToFloat();												//字符数据拷贝到浮点数据
	void MemcpyFloatToByte();												//浮点数据拷贝到字符数据
	void MemcpyFloatToByteBounded(float lower = 0, float upper = 255);		//根据上下限拷贝数据

	/* 获取图像的属性*/

	BYTE* GetHeadAddress() const;											//获取图像首地址
	BYTE* GetLineAddress(int nLineID) const;								//获取某行的地址
	int GetChannel() const;													//获取通道个数
	int GetRowlen() const;													//获取每行字节数
	int GetLength() const;													//获取总的字节数
	int GetDiagLength() const;												//获取图像对角线长
	void GetInfomation(int &nWidth, int &nHeight) const;					//获取图像的信息
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen) const;
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP) const;
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel) const;
	BOOL GetHistogram(int hist[4][256]) const;						/* 获取图像R、G、B通道直方图*/
	BOOL GetGrayHist(int hist[256]) const;									/* 获取灰度直方图 */
	void GetCluster(int cluster[256]) const;								/* 像素按个数排序 */
	// 

	/* 制作备份图像，需要delete方法 */
	CyImage* MakeCopy();

	/* 获取背景图像，需要delete方法 */
	CyImage* GetBackground(float threahold, float3 background);

	/* 提取感兴趣区域，需要delete方法 */
	CyImage* ROI(CLogoRect rect = CLogoRect(0, 0, 0, 0));

	/* 对图像旋转，返回新图像的数据，宽度、高度和每行浮点数，需要delete方法 */
	float* Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, 
		int &NewWidth, int &NewHeight, int &NewRowlen);

	/* 根据参数放大图像，返回浮点数据，需要delete方法 */
	float* Zoom(int NewWidth, int NewHeight, int bNeededReturn);
	//

	// 创建图像
	BOOL Create(const BYTE* pSrc, int nWidth, int nHeight, int nBPP) throw();
	BOOL Create(const float* pSrc, int nWidth, int nHeight, int nRowlen) throw();
	BOOL Create(const ImageSrc *pSrc) throw();
	// 重载函数
	BOOL Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0) throw();
	HRESULT Load(LPCTSTR pszFileName) throw();
	HRESULT Save(LPCTSTR pszFileName, REFGUID guidFileType = GUID_NULL) const throw();
	void Destroy() throw();

#ifdef _AFX // MFC
	HRESULT Load(CWnd* pParentWnd = NULL) throw();
	BOOL Save(REFGUID guidFileType) const throw();
	BOOL Save(CWnd* pParentWnd = NULL, REFGUID guidFileType = GUID_NULL) const throw();
#endif

private:
	float*	m_pFloatData;													/**< 对应图像的浮点数 */
	float	m_fMaximum;														/**< 浮点数据的最大值 */
	float	m_fMinimum;														/**< 浮点数据的最小值 */
	
	BOOL	InitFloatData();												//初始化浮点数据
	void	ComputeMinMax();												//浮点数据最大最小值
	void	Bpp8To24();														// 8位转24位
	void	Bpp8To32();														// 8位转32位
	void	Bpp24To8();														// 24位转8位
	void	Bpp24To32();													// 24位转32位
	void	Bpp32To8();														// 32位转8位
	void	Bpp32To24();													// 32位转24位
	void	Bpp1To8();														// 掩码转8位
	void	Bpp1To24();														// 掩码转24位
	void	Bpp1To32();														// 掩码转32位
};

// swap 交换int型元素的次序
void swap(int &nElem1, int &nElem2);

// 为8位色图像设置颜色表
void SetColorTabFor8BitImage(CImage *pImage);

// 从file path获取file name
CString GetFileName(CString strFilePath, BOOL bHasPostfix = TRUE);

// 从file path获取file ext
CString GetFileExt(CString strFilePath);

// 数字转为CString
CString Num2String(int nNum);

// 在目标图像挖出一块矩形用源图像替换
void ReplacedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect);

// 在目标图像挖出一块矩形与源图像混合
void MixedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect, float dst_rate);

#endif // WIN32