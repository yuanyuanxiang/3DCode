/** 
* @file ColorsDecode.h
* @details 解码彩色数据，可在ANDROID下使用。
* @author 
* @email 
* @version V1.0
* @date 2016/10/16
* Copyleft (c), All rights reserved.
*/

#pragma once
#include "ColorsEncode.h"

/** 
* @class ColorsDecode 
* @brief 彩色解码模块
* @details 本类新增8个成员变量（将通过构造函数与Init函数初始化）
*/
class ColorsDecode : public ColorsEncode
{
private:
	// 图像信息
	BYTE*			m_pHead;		/**< 图像头指针 */
	int				m_nWidth;		/**< 图像宽度 */
	int				m_nHeight;		/**< 图像高度 */
	int				m_nRowlen;		/**< 图像每行字节数 */
	int				m_nChannel;		/**< 图像每像素字节 */
	int				m_nModuleSize;	/**< 二维码模块大小 */
	// 编码颜色
	float3			m_Foreground;	/**< 彩色前景 */
	float3			m_Background;	/**< 彩色背景 */
	
public:
	/** 
	* @brief 创建一个彩色编码器
	* @param[in] bMatrix 									二维码数据
	* @param[in] nSymbolSize								二维码尺寸
	* @param[in] nModuleSize								模块大小
	* @note 函数将初始化本类的7个变量
	*/
	ColorsDecode(BYTE bMatrix[MAX_MODULESIZE][MAX_MODULESIZE], int nSymbolSize, int nModuleSize)
		: ColorsEncode(bMatrix, nSymbolSize, 0)
	{
		m_nModuleSize = nModuleSize;
		m_Foreground = 0;
		m_Background = 0;
	}

	~ColorsDecode() { }

	/** 
	* @brief 初始化彩色解码器
	* @param[in] * pHead	二维码图像指针
	* @param[in] nWidth		图像宽度
	* @param[in] nHeight	图像高度
	* @param[in] nChannel	图像通道数
	* @note 函数将初始化本类的5个变量
	*/
	void Init(BYTE* pHead, int nWidth, int nHeight, int nChannel)
	{
		m_pHead = pHead;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_nChannel = nChannel;
		m_nRowlen = WIDTHBYTES(nChannel * 8 * nWidth);
	}

	// RS4方法解码彩色
	char* DecodeColors(COLORREF QREncodeColor1, COLORREF QREncodeColor2);

	/** 
	* @brief 获取彩色数据头信息
	* @param[out] &strLen			数据长度
	* @param[out] &nInnerecLevel	纠错等级
	* @param[out] &nInnerMask		掩码版本
	* @note 解码之后调用此函数获取解码结果
	*/
	void GetHeaderInfo(int &strLen, int &nInnerecLevel, int &nInnerMask) const 
	{
		strLen = m_strLen;
		nInnerecLevel = m_ecLevel;
		nInnerMask = m_nMaskingNo;
	}

private:
	// 解码数据头
	BOOL DecodeHeader();
};