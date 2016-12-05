#include "stdafx.h"
#include "DecodeImage.h"
#include "CodeTransform.h"

/************************************************************************/
/*             若将此文件包含进项目，请在预处理器添加此项               */
/************************************************************************/
#ifdef USEING_ZXING  // 使用ZXing库解码

#include "ZXingDecoder.h"
#include "ImageReader.h"

/*
函数功能：解码图像
CyImage* pImage				二维码
CString &strDecodeString	解码信息
int &ncLength				数据长度
float2 pos[4]				位置信息
float &fModuleSize			模块大小
int &nLevel					纠错率
int &nVersion				编码版本
int &nMaskingNo				掩码版本
BOOL use_hybrid				使用混合二值化
BOOL try_harder				强力搜索解码
*/
BOOL DecodeImage(CyImage* pImage, CString &strDecodeString, int &ncLength, float2 pos[4], float &fModuleSize, int &nLevel, int &nVersion, int &nMaskingNo, BOOL use_hybrid, BOOL try_harder)
{
	char* decode_string = reinterpret_cast<char*> (GetDecodeString(pImage));
	if (decode_string == NULL)
		return FALSE;

	int width, height, channel;
	pImage->GetInfomation(width, height);
	channel = pImage->GetChannel();
	Ref<LuminanceSource> source;

	try
	{
		source = ImageReader::create(decode_string, width, height, channel);
		SAFE_DELETE(decode_string);
	}
	catch (const zxing::IllegalArgumentException &e)
	{
		cerr << e.what() << " (ignoring)" << endl;
		return FALSE;
	}

	vector<Ref<Result> > results;
	Ref<Binarizer> binarizer;
	string cell_result;
	try
	{
		if (use_hybrid)
			binarizer = new HybridBinarizer(source);
		else
			binarizer = new GlobalHistogramBinarizer(source);

		DecodeHints hints(DecodeHints::QR_CODE_HINT);
		hints.setTryHarder(TRUE == try_harder);
		Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
		float ppt[9] = { 0 };
		results = decode(binary, hints, ppt, fModuleSize, nLevel, nVersion, nMaskingNo);
	}
	catch (const ReaderException& e)
	{
		cell_result = "zxing::ReaderException:" + string(e.what());
		TRACE(" * %s\n", cell_result.c_str());
		return FALSE;
	}
	catch (const zxing::IllegalArgumentException& e)
	{
		cell_result = "zxing::IllegalArgumentException:" + string(e.what());
		TRACE(" * %s\n", cell_result.c_str());
		return FALSE;
	}
	catch (const zxing::ZXingException& e)
	{
		cell_result = "zxing::ZXingException: " + string(e.what());
		TRACE(" * %s\n", cell_result.c_str());
		return FALSE;
	}
	catch (const std::exception& e)
	{
		cell_result = "std::exception: " + string(e.what());
		TRACE(" * %s\n", cell_result.c_str());
		return FALSE;
	}
	catch(...)
	{
		TRACE(" * 提示: 解析二维码时发生异常，但是该异常尚未被捕获。\n");
	}

	int Num = results.size();
	if (Num > 1)
	{
		TRACE(" * Info: 解析发现不止一个二维码，但只输出第一个信息。\n");
	}
	if (Num > 0)
	{
		string temp = results[0]->getText()->getText();
		const char *chr = temp.c_str();
		strDecodeString = UTF8Convert2Unicode((char*)chr, ncLength);

		TRACE(" * Decode: %s \n", strDecodeString);
		TRACE(" * Format: %s \n", BarcodeFormat::barcodeFormatNames[results[0]->getBarcodeFormat()]);
		TRACE(" * Windows坐标系统，原点是图像的左上角。\n");

		for (int j = 0; j < results[0]->getResultPoints()->size(); j++)
		{
			pos[j].x = results[0]->getResultPoints()[j]->getX();
			pos[j].y = results[0]->getResultPoints()[j]->getY();
			TRACE(" * Point[%d]:(%f, %f) \n", j, pos[j].x, pos[j].y);
		}
	}
	return TRUE;
}


/* 从图像获取解码字串 */
BYTE* GetDecodeString(CyImage* pImage)
{
	if (pImage->IsNull())
		return NULL;
	BYTE* pHead = pImage->GetHeadAddress();
	int nWidth, nHeight, nRowlen, nBPP;
	nWidth = nHeight = nRowlen = nBPP = 0;
	pImage->GetInfomation(nWidth, nHeight, nRowlen, nBPP);
	if (nBPP != 8 && nBPP != 24 && nBPP != 32)
	{
		TRACE(" * 警告: 图像位深度小于8或者为16，不被支持。\n");
		return NULL;
	}
	int NewRowlen = nWidth * 4;
	BYTE *pDst = new BYTE[nHeight * NewRowlen];
	switch (nBPP)
	{
	case 8:
#pragma omp parallel for
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				pDst[    4 * j + i * NewRowlen] = pHead[j + i * nRowlen];
				pDst[1 + 4 * j + i * NewRowlen] = pHead[j + i * nRowlen];
				pDst[2 + 4 * j + i * NewRowlen] = pHead[j + i * nRowlen];
				pDst[3 + 4 * j + i * NewRowlen] = 0;// Alpha
			}
		}
		break;
	case 24:
#pragma omp parallel for
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				pDst[    4 * j + i * NewRowlen] = pHead[    3 * j + i * nRowlen];
				pDst[1 + 4 * j + i * NewRowlen] = pHead[1 + 3 * j + i * nRowlen];
				pDst[2 + 4 * j + i * NewRowlen] = pHead[2 + 3 * j + i * nRowlen];
				pDst[3 + 4 * j + i * NewRowlen] = 0;// Alpha
			}
		}
		break;
	case 32:
		memcpy(pDst, pHead, nHeight * NewRowlen * sizeof(BYTE));
		break;
	default:
		ZeroMemory(pDst, nHeight * NewRowlen * sizeof(BYTE));
		break;
	}

	return pDst;
}


/* 从图像获取解码字串 */
void GetDecodeString(BYTE* pDst, CyImage* pImage)
{
	int nHegiht = pImage->GetHeight();
	int nRowlen = pImage->GetRowlen();

	BYTE* head = pImage->GetHeadAddress();

#pragma omp parallel for
	for (int r = 0; r < nHegiht; ++r)
	{
		memcpy(pDst + r * nRowlen, head + (nHegiht - 1 - r) * nRowlen, nRowlen * sizeof(BYTE));
	}
}


#endif