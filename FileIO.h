#pragma once

/** 
* @file FileIO.h
* @brief 文件读写函数，可以跨平台
*/

#include "DataTypes.h"
#include "Transform.h"

/**
* @brief 将数组写入文件
* @param[in] *fileName 文件名
* @param[in] *pSrc 数组指针
* @param[in] nLength 数组长度
* @return 成功返回TRUE
*/
template <typename Type> BOOL WriteArray(const char* fileName, const Type* pSrc, int nLength)
{
	ofstream fout;
	fout.open(fileName);

	if (fout.is_open())
	{
		try
		{
			for (int i = 0; i < nLength - 1; ++i)
			{
				fout << *(pSrc + i) << ",";
			}
			fout << *(pSrc + nLength - 1);
			fout.close();
			return TRUE;
		}
		catch(...)
		{
			fout.close();
			return FALSE;
		}
	}
	
	return FALSE;
}

/** 
* @brief 将BYTE图像数据以二进制形式写入文件,nChannel表示一个像素占用几个字节
* @param[in] *fileName 文件名
* @param[in] *pSrc 图像数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nChannel 每像素字节
*/
inline BOOL WriteTxt(const char* fileName, const BYTE* pSrc, int nWidth, int nHeight, int nChannel)
{
	return ImageWrite(fileName, pSrc, nWidth, nHeight, WIDTHBYTES(8 * nWidth * nChannel));
}

// 将图像数据以二进制形式读入文件，nChannel表示一个像素占用几个字节
BYTE* ReadTxt(const char* fileName, int &nWidth, int &nHeight, int &nRowlen, int &nChannel);