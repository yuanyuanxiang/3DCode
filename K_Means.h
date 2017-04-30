/** 
* @file K_Means.h
* @brief 模板函数,可以跨平台.
* @details 包括：K均值聚类的核心代码.
* @author 袁沅祥
* @version V1.0
* @date 2017/3/4
* @note 务必添加此包含"algorithm.h"文件，否则在Eclipse遇到
* error: 'sort' is not a member of 'std'
* 凡是以**二级指针作为参数，函数将修改此一级指针.\n
* Copyleft (c), All rights reserved.
*/

#include "DataTypes.h"

/* K_means */

#pragma once

// 计算组间差异(2016/9/19)
template <typename Type> Type Difference(const Type *Center, int K);

// 初始化聚类中心(2016/9/19)
template <typename T1, typename T2> void InitializeClusterCenters(T1 *Center, int K, const T2* pHead, int nRowBytes, RoiRect roi);

// 初始化聚类中心(random随机)
template <typename T1, typename T2> void RandomClusterCenters(T1 *Center, int K, const T2* pHead, int nRowBytes, RoiRect roi);

// 对图像进行用K_means聚类作分割
template <typename Type> void K_means(Type* pHead, int nRowBytes, RoiRect roi, int K, float fBackground, int nMaxepoches);

//////////////////////////////////////////////////////////////////////////


/** 
* @brief 计算组间差异(2016/9/19).
* @param[in] *Center 数组
* @param[in] K 元素数
*/
template <typename Type> Type Difference(const Type *Center, int K)
{
	Type sum(0);
	for (int i = 0; i < K; ++i)
	{
		for (int j = i; j < K; ++j)
		{
			sum += abs(Center[j] - Center[i]);
		}
	}
	return sum;
}


/** 
* @brief 初始化聚类中心(2016/9/19).
* @param[in] *Center 聚类中心
* @param[in] K 聚类个数
* @param[in] *pHead 图像数据
* @param[in] nRowBytes 图像每行元素个数
* @param[in] roi 图像感兴趣区域
* @note 函数计算图像直方图，取峰值处的灰度为中心.
*/
template <typename T1, typename T2> void InitializeClusterCenters(T1 *Center, int K, const T2* pHead, int nRowBytes, RoiRect roi)
{
#ifdef _DEBUG
	TRACE(" * 正在初始化聚类中心...\n");
#endif // _DEBUG
	// 获取直方图
	int nHist[256];
	ImageHistogram(pHead, nRowBytes, nHist, roi);
	int index[256];// 直方图的下标
	for (int i = 0; i < 256; )
	{
		index[i] = i; ++i;
		index[i] = i; ++i;
		index[i] = i; ++i;
		index[i] = i; ++i;
	}
	// 插入排序,使灰度值峰值排在前头
	for (int i = 1; i < 256; ++i)
	{
		for (int j = i; j > 0 && nHist[j] > nHist[j - 1]; --j)
		{
			SWAP(nHist[j], nHist[j-1]);
			SWAP(index[j], index[j-1]);
		}
	}
	// 寻找K个中心，这些中心的间距不得小于min_diff
	int count = 0, num;
	T1 min_diff = (T1)64;
	do
	{
		count++;
		num = 0;
		Center[0] = (T1)index[num]; //第1个中心
		for (int i = 0; i < K; ++i)
		{
			// 生成后续K个中心，确保中心差距较大
			while(num < 256)// 确保能生成一个
			{
				Center[i] = (T1)index[num++];
				int j = 0;
				for (; j < i; ++j)
				{
					// 到某个聚类中心的间距过小
					if (abs(Center[i] - Center[j]) < min_diff)
						break;
				}
				// Cj到全部Ci的距离较大，退出while循环，接着生成下一个
				if (j == i)
					break;
			}
			// 可能min_diff设置太大
			if (num == 256)
				break;
#ifdef _DEBUG
			TRACE("C[%d] = %.6f\t", i, (float)Center[i]);
#endif // _DEBUG
		}
#ifdef _DEBUG
		TRACE("\n");
#endif // _DEBUG
		min_diff -= (T1)1;
	} while (num == 256);
#ifdef _DEBUG
	TRACE(" * [%d]轮搜索聚类中心，类间距不小于%.2f.\n", count, (float)min_diff);
#endif
}


/** 
* @brief 随机生成K个中心.
* @param[in] *Center 聚类中心
* @param[in] K 聚类个数
* @param[in] *pHead 图像数据
* @param[in] nRowBytes 图像每行元素个数
* @param[in] roi 图像感兴趣区域
*/
template <typename T1, typename T2> void RandomClusterCenters(T1 *Center, int K, const T2* pHead, int nRowBytes, RoiRect roi)
{
#ifdef _DEBUG
	TRACE(" * 正在初始化聚类中心...\n");
#endif // _DEBUG
	// 随机生成K个中心
	unsigned int seed = (unsigned int)time(NULL);
	for (int i = 0; i < K; i++)
	{
		srand(seed);
		int w = roi.left + rand() % roi.Width();
		int h = roi.top + rand() % roi.Height();
		// BYTE 强制转换是必须的，下同(2016/9/12)
		Center[i] = (BYTE)pHead[w + h * nRowBytes];
#ifdef _DEBUG
		TRACE("C[%d] = %.6f\t", i, (float)Center[i]);
#endif // _DEBUG
		seed = rand();
	}
#ifdef _DEBUG
	TRACE("\n");
#endif // _DEBUG
}


/** 
* @brief K_means 聚类分割.
* @param[in] *pHead 图像数据
* @param[in] nRowBytes 每行字节数
* @param[in] roi 图像感兴趣区域
* @param[in] K 聚类个数
* @param[in] nMaxepoches 最大迭代次数
* @note (1)必须是灰度图像;(2)此函数将修改图像数据pHead.
*/
template <typename Type> void K_means(Type* pHead, int nRowBytes, RoiRect roi, int K, float fBackground, int nMaxepoches)
{
	// 聚类区域的尺寸
	const int nWidth = roi.Width();
	const int nHeight = roi.Height();
	const int nRowlen = nWidth;// 聚类标记每行元素个数
	Type* Cluster = new Type[nHeight * nRowlen];// 聚类标记TAG
												// 聚类中心
	float *Center = new float[K];
	float *oldCenter = new float[K];
	int *sumCenter = new int[K];// 聚类中心求和
	int *sum = new int[K];// 每个聚类中心元素个数
						  // 生成K个中心
	InitializeClusterCenters(Center, K, pHead, nRowBytes, roi);
	/* 多次迭代直至收敛，本次试验迭代nMaxepoches次 */
	for (int it = 0; it < nMaxepoches; ++it)
	{
		ZeroMemory(sumCenter, K * sizeof(int));
		ZeroMemory(sum, K * sizeof(int));
		/* 求出每个样本点距应该属于哪一个聚类 */
		for (int j = roi.top; j < roi.bottom; ++j)
		{
			Type *pRowj = pHead + j * nRowBytes;
			int y = (j - roi.top) * nRowlen;
			for (int i = roi.left; i < roi.right; ++i)
			{
				/* 当前像素的浮点值 */
				int curPixel = (BYTE)pRowj[i];
				/* 都初始化属于第0个聚类 */
				int tag = 0;
				float temp = curPixel - Center[tag];
				float min_distance = (temp > 0.0F ? temp : -temp);
				for (int s = 1; s < K; s++)
				{
					temp = curPixel - Center[s];
					float new_distance = (temp > 0.0F ? temp : -temp);
					if (new_distance < min_distance)
					{
						min_distance = new_distance;
						tag = s;
					}
				}
				// 将(i, j)标记为tag
				Cluster[(i - roi.left) + y] = (Type)tag;
				sumCenter[tag] += curPixel;
				++sum[tag];
			}
		}
		/* 保存上一个结果，并更新聚类中心 */
		memcpy(oldCenter, Center, K * sizeof(float));
		float diff = 0.F;
		for (int i = 0; i < K; ++i)
		{
			if (sum[i])
			{
				Center[i] = (float)sumCenter[i] / sum[i];
			}
			diff += (oldCenter[i] - Center[i]) * (oldCenter[i] - Center[i]);
		}
		// 注销掉下述语句以监视迭代过程
#ifdef _DEBUG
		TRACE(" * 迭代次数 = %d：\n", it + 1);
		for (int i = 0; i < K; ++i)
		{
			TRACE("C[%d] = %.6f\t%d个\t", i, Center[i], sum[i]);
		}
		TRACE("\n");
#endif // _DEBUG
		// 当前后相邻两次中心变化较小时（0.01个像素）退出
		if (diff < 0.0003f)
			break;
	}

	// 寻找背景值对应的类号
	int backValIdx = 0;
	float min_dis = fabs(Center[0] - fBackground);
	for (int i = 1; i < K; ++i)
	{
		float temp_dis = fabs(Center[i] - fBackground);
		if (temp_dis < min_dis)
		{
			backValIdx = i;
			min_dis = temp_dis;
		}
	}

	// 对原始图像进行分割
	for (int j = 0; j < nHeight; ++j)
	{
		int y0 = j * nRowlen;
		int y = (j + roi.top) * nRowBytes;
		for (int i = 0; i < nWidth; ++i)
		{
			pHead[i + roi.left + y] = (Cluster[i + y0] == backValIdx) ? 0 : 255;
		}
	}
	delete[] Center;
	delete[] oldCenter;
	delete[] sumCenter;
	delete[] sum;
	delete[] Cluster;
}