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

/// 解码所用像素值类型
typedef float3 pixel;

/** 
* @class ColorsDecode 
* @brief 彩色解码模块
* @details 本类新增8个成员变量（将通过构造函数与Init函数初始化）
*/
class ColorsDecode : public ColorsEncode
{
private:
	// 图像信息
	const BYTE*		m_pHead;		/**< 图像头指针 */
	int				m_nWidth;		/**< 图像宽度 */
	int				m_nHeight;		/**< 图像高度 */
	int				m_nRowlen;		/**< 图像每行字节数 */
	int				m_nChannel;		/**< 图像每像素字节 */
	int				m_nModuleSize;	/**< 二维码模块大小 */
	static char		m_pData[MAX_CODEUTF8NUM];/**< 解码结果 */
	// 编码颜色
	pixel			m_Foreground;	/**< 彩色前景 */
	pixel			m_Background;	/**< 彩色背景 */
	// Color Infomation + Logo & Version
	int				m_nHeaderBits[90];/**< 彩色编码的头 */
	// 是否需要翻转比特流
	bool			m_bInversed;	/**< 彩色前景背景搞反了 */
	
public:
	/** 
	* @brief 创建一个彩色编码器
	* @param[in] bMatrix 									二维码数据
	* @param[in] nSymbolSize								二维码尺寸
	* @param[in] nModuleSize								模块大小
	* @note 函数将初始化本类的7个变量
	*/
	ColorsDecode(qrMat bMatrix[MAX_MODULESIZE], int nSymbolSize, int nModuleSize)
		: ColorsEncode(bMatrix, nSymbolSize, 0)
	{
		m_nModuleSize = nModuleSize;
		m_Foreground = 0;
		m_Background = 0;
		m_bInversed = false;
	}

	~ColorsDecode() { }

	// RS4方法解码彩色
	const char* DecodeColors(const BYTE* pHead, int nWidth, int nHeight, int nChannel);

	/** 
	* @brief 获取彩色数据头信息
	* @param[out] &strLen			数据长度
	* @param[out] &nInnerecLevel	纠错等级
	* @param[out] &nInnerMask		掩码版本
	* @note 解码之后调用此函数获取解码结果
	*/
	inline void GetHeaderInfo(int &strLen, int &nInnerecLevel, int &nInnerMask) const 
	{
		strLen = m_strLen;
		nInnerecLevel = m_ecLevel;
		nInnerMask = m_nMaskingNo;
	}

private:
	// 获取数据头
	void GetHeaderBits(int nMaxepoches = 10);
	// 解码数据头
	BOOL DecodeHeader(bool bInverse = false);
	// K均值二值化
	void K_means(const pixel *pArray, int *Cluster, int nNum, int nMaxepoches);
};