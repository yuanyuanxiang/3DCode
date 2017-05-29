/** 
* @file ColorsBase.h
* @brief 彩色二维码彩色编码解码基类
* @date 2016/10/15
*/

#pragma once

#include "DataTypes.h"
#include <vector>
using namespace std;

#ifndef MAX_MODULESIZE
#define MAX_MODULESIZE 177 /**< QR码最大尺寸 */
#endif

/** 
* @enum EncodeModule
* @brief 彩色编码模块
* @details 1 - 在白色模块编码；0 - 在黑色模块编码
*/
enum EncodeModule
{
	BlackModule = 0,		/**< 在前景色编码（黑色） */
	WhiteModule = 1,		/**< 在背景色编码（白色） */
};

/// 掩码操作
const static int HEADER_MASK[90] = {
	// 16 x 3 前48位为 Color Infomation
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
	// 14 x 3 后42位为 Logo & Version
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

/// 采用新版
#define NEW_VERSION 1

/** 
* @class ColorsBase 
* @brief 彩色二维码编码解码基类
*/
class ColorsBase
{
protected:
	BYTE*				m_pBitMatrix[MAX_MODULESIZE];	/**< 原二维码 */
	int					m_nSymbolSize;					/**< 二维码尺寸 */
	vector<CPixelPoint> m_HeaderIndex;					/**< 数据头索引 */
	CLogoRect			m_LogoRect;						/**< 二维码LOGO */

private:
	EncodeModule		m_EncodeModule;/**< 彩色编码模块（1，默认为白色） */
	vector<CPixelPoint> m_vColorsModuleIndex;			/**< 彩色模块索引 */

public:
	/** 
	* @brief 创建一个彩色编码解码基类对象
	* @param[in] bMatrix 									二维码数据
	* @param[in] nSymbolSize								二维码尺寸
	*/
	ColorsBase(qrMat bMatrix[MAX_MODULESIZE], int nSymbolSize)
	{
		/// 采用for循环，将bMatrix的每行拷贝给m_pBitMatrix
		/// @note 只能采用循环，不能使用memcpy
		for (int i = 0; i < nSymbolSize; ++i)
		{
			m_pBitMatrix[i] = bMatrix[i];
		}
		/// 构造的对象默认无LOGO
		m_LogoRect = 0;
		/// 默认在白色模块编码色彩
		m_EncodeModule = WhiteModule;
		m_nSymbolSize = nSymbolSize;
	}

	/// 默认的析构函数
	~ColorsBase() { }

	/// 设置编码模块
	inline void SetEncodeModule(EncodeModule Module)
	{
		m_EncodeModule = Module;
	}

	/// 获取编码模块
	inline EncodeModule GetEncodeModule() const 
	{
		return m_EncodeModule;
	}

	/// 设置二维码LOGO
	inline void SetLogoRect(CMyRect logo)
	{
		m_LogoRect = logo;
	}

	/// 获取二维码LOGO
	inline CMyRect GetLogoRect() const 
	{
		return m_LogoRect;
	}

	/// 获取模块总数
	inline int GetModulesCount() const 
	{
		return m_nSymbolSize * m_nSymbolSize - m_LogoRect.Width() * m_LogoRect.Height();
	}

	// 获取编码模块总数
	int GetColorsMoudlesCount() const 
	{
		return m_vColorsModuleIndex.size();
	}

	// 获取编码模块的索引
	const vector<CPixelPoint>& GetColorsModuleIndex() const 
	{
		return m_vColorsModuleIndex;
	}

	//  初始化编码模块的索引
	void InitColorsModuleIndex(BOOL bFore = TRUE);

	// (row, col)非数据头索引
	BOOL NotHeaderIndex(int row, int col) const;

	// 获取彩色数据头的索引
	virtual void GetDataHeaderIndex() = 0;
};