/** 
* @file Mask.h
* @brief 彩色二维码旧版掩码
*/

#pragma once

class COldMask
{
public:
	/**
	* @brief 构造一个旧的掩码
	* @param[in] nMin 随机数最小值
	* @param[in] nMax 随机数最大值
	* @param[in] nSeed 随机数种子
	* @note 旧的掩码是产生随机数，填补多余的模块
	*/
	COldMask(int nMin, int nMax, unsigned int nSeed) 
		: m_nMin(nMin), m_nMax(nMax), m_nSeed(nSeed), m_pMaskData(0), m_nMaskNum(0), m_nIndex(0) { }

	~COldMask() { SAFE_DELETE(m_pMaskData); }

	// 随机产生nNums个不同的整数，介于nMin到nMax
	void GenerateInterferenceData(int nNums, int nMaskingNo = 0, int nVersion = 0);

	/// 是否是干扰信息
	inline bool IsMasked(int &k)
	{
		if (k == m_pMaskData[m_nIndex] && m_nIndex < m_nMaskNum)
		{
			k++;
			m_nIndex++;
			return true;
		}
		return false;
	}

private:
	/** 随机数最小值 */
	int m_nMin;
	/** 随机数最大值 */
	int m_nMax;
	/** 随机数种子 */
	unsigned int m_nSeed;
	/** 随机数发生器的默认种子 */
	static unsigned int ___next;

	/// 掩码数据
	int *m_pMaskData;

	/// 掩码个数
	int m_nMaskNum;

	/// 下标
	int m_nIndex;

	/**
	* @brief rand在windows和android下产生的随机数列不一样的解决办法
	* @see http://blog.csdn.net/zhch152/article/details/47862853
	* @date 2015-08-22 20:08
	*/
	inline int my_rand()
	{
		___next = ___next * 0x343fd + 0x269EC3;
		return (___next >> 0x10) & 0x7FFF;
	}


	/** 
	* @brief 设置随机数发生器的种子
	*/
	inline void my_srand(unsigned int ___seed)
	{
		___next = ___seed;
	}
};