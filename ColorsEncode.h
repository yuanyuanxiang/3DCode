/** 
* @file ColorsEncode.h
* @details 编码彩色数据，可在ANDROID下使用。
* @author 
* @email 
* @version V1.1
* @date 2016/10/15
* Copyleft (c), All rights reserved.							
*/

#pragma once

#include "ColorsBase.h"

#define ENCODE_MOUDLE		1			/**< 编解码方案开关 1-白色 0-黑色 */

#define COLOR_FOREGROUND	0			/**< 前景色 */

#define COLOR_BACKGROUND	1			/**< 背景色 */

#define COLOR_QRCOLOR1		2			/**< 彩色一(前景) */

#define COLOR_QRCOLOR2		17			/**< 彩色二(背景) */

#define SUCCESS				0			/**< 成功 */

#define ERROR_ZCOMPRESS		1			/**< 压缩出错 */

#define ERROR_UNSURPORT		2			/**< 不支持 */

#define ERROR_LONGSTRING	3			/**< 字符太长 */

/**

	00		01		10		11
	00		00		00		00

	00		01		10		11
	10		10		10		10

	00		01		10		11
	01		01		01		01

	00		01		10		11
	11		11		11		11
*/

/** 
* @class ColorsEncode 
* @brief 彩色编码模块
*/
class ColorsEncode : public ColorsBase
{
public:
	/** 
	* @brief 创建一个彩色编码解码基类对象
	* @param[in] bMatrix									二维码数据
	* @param[in] nSymbolSize								二维码尺寸
	* @param[in] LogoRect									二维码LOGO
	*/
	ColorsEncode(qrMat bMatrix[MAX_MODULESIZE], int nSymbolSize, CLogoRect LogoRect = 0)
		: ColorsBase(bMatrix, nSymbolSize)
	{
		// 设置LOGO
		SetLogoRect(LogoRect);

#if (!ENCODE_MOUDLE)
		// 设置在黑色模块编码色彩
		SetEncodeModule(BlackModule);
#endif // QRCOLORS_ANOTHER

		// 获取数据头索引
		GetDataHeaderIndex();

		m_strLen = 0;
		m_ecLevel = 0;
		m_nMaskingNo = 0;
		m_nVersion = 0;
	}

	/// 默认的析构函数
	~ColorsEncode() { }

	// 为原二维码着色
	int EncodeColors(char *pUtf8, int nStrLen, int nInnerEcLevel, int nInnerMaskNo, int nInnerVersion);

protected:
	// 彩色编码及数据格式
	int				m_strLen;		/**< 数据长度 */
	int				m_ecLevel;		/**< 纠错等级 */
	int				m_nMaskingNo;	/**< 掩码版本 */
	int				m_nVersion;		/**< 编码版本 - 
									0版本采用旧的掩码，非0采用新的掩码
									30版本SUPER_QR用来开关是否强势扩容
									31版本采用Zlib对数据进行压缩编码 */

#if (ENCODE_MOUDLE)
	// 获取位置识别区的16个模块的索引
	void GetPositionParternIndex(CMyRect rect);

	// 获取分隔图形的14个模块的索引
	void GetSeparateParternIndex(int nDirection);
#endif

	// 获取彩色数据头的索引
	virtual void GetDataHeaderIndex();

private:
	// 编码数据头
	int EncodeHeader();
};