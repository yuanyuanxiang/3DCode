/************************************************************************/
// 该函数对ZXing QR解码模块进行测试
// 代码拷贝自项目TestDecode的主函数
// 2016/9/1 建立此文档
// 2017/4/28 修改
/************************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include <fstream>
#include <Windows.h>
#include "DecodeFuncs.h"
#include "FileIO.h"
#include <time.h>

/************************************************************************/
// 这是解码的测试程序 - 对一副彩色二维码图像进行解码
// 该二维码图像以二进制形式保存(由WriteBinaryFile函数生成)

// 将utf-8编码转换成Unicode编码
wchar_t* UTF8Convert2UnicodeEx(const char* strUtf8, int &ncLength)
{
	ncLength = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
	if (ncLength == 0)
		return NULL;
	wchar_t *strUnicode = new wchar_t[ncLength];
	ncLength = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, strUnicode, ncLength);
	return strUnicode;
}

// 对文件test进行解码测试
int DecodeTest(const char *test = "./ZXingTest.txt");

int main()
{
	// 进行多次解码操作，查找耗时的代码
	int start = clock();
	int nSuccessCount = 0, nColorCount = 0;
	const int TIMES = 10000;
	for (int i = 0; i < TIMES; ++i)
	{
		TRACE(" * 第%d次解码。\n", i+1);
		int result = DecodeTest();
		if (result)
		{
			nSuccessCount++;
		}
		if (2 == result)
		{
			nColorCount++;
		}
		else
		{
			TRACE(" * 第%d次解码失败!\n", i+1);
		}
	}
	int stop = clock();
	double totaltime = (double)(stop - start)/CLOCKS_PER_SEC;
	nSuccessCount = max(nSuccessCount, 1);
	TRACE(" * 解码成功:%d(%d)次, 占%.3f\n * 平均耗时：%.3f秒", nColorCount, nSuccessCount, 
		1.0 * nColorCount / nSuccessCount, totaltime / TIMES);
	return getchar();
}

/**
* @brief 对指定文件进行解码测试
* @return 解码失败返回0，成功返回1或2，2表示解码彩色成功.
*/
int DecodeTest(const char *test)
{
	// 图像信息
	int nRowlen = 0, nHeight = 0, nChannel = 0;

	// 外层二维码信息
	float m_fModuleSize = 0.f;
	int nLevel = 0, nVersion = 0, nMaskingNo = 0;
	int ncLength = 0, _ncLength = 0;

	// 图像宽度
	int nWidth = 0;
	BYTE *pSrc = ReadTxt(test, nWidth, nHeight, nRowlen, nChannel);
	if (pSrc == NULL)
		return 0;

	BarCodeInfo qr, inner;
	ImageSrc pImage(pSrc, nWidth, nHeight, nChannel);
	BOOL success = DecodeWholeImage(DecodeSrc(pImage, TRUE, TRUE), &qr, &inner);

	SAFE_DELETE(pSrc);
	wchar_t *pOuter = UTF8Convert2UnicodeEx(qr.m_pData, ncLength);
	wchar_t *pInner = UTF8Convert2UnicodeEx(inner.m_pData, ncLength);
	SAFE_DELETE(pOuter);
	SAFE_DELETE(pInner);
	return success;
}