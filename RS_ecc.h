#pragma once

/**
* @file			RS_ecc.h
* @brief		RS 纠错编码
* @version		1.0 
* @date			2016/9/14
* @note			可以跨平台
* @author		袁沅祥
* 2016/8/26		建立此文档
* 2017/4/4		修改此文档，删除弃用项
*/

// 将01比特流翻转
void InverseBitsStream(int *pBits, int nLength);

// 提取字符串中的bits流
void GetBitsStream(const BYTE *pSrc, int *pDest, int nCount);

// 计算8个bit位对应的BYTE
int Bits2Byte(const int bits[8]);

// 将字符串按4bits分割
void SplitByteBy4Bits(const BYTE* dataPtr, int dataLen, int* dst, int dstLen);

// 将数据按4bits位数合并
void MergedIntBy4Bits(int* dataSrc, int totalBits);

// 字符串按2bits分割
void SplitByteBy2Bits(const BYTE* dataPtr, int dataLen, int* dst, int dstLen);

// 将数据按2bits位数合并
void MergedIntBy2Bits(int* dataSrc, int totalBits);

// 获取RS4编码的比特总数
int RS4BitsLen(int dataLen, int ecLevel, int & bitsLen);

// 4bit符号大小的RS纠错编码
int* RS4Encode(const BYTE* dataPtr, int dataLen, int bitsLen, int ecLevel);

// 4bit符号大小的RS纠错解码
int* RS4Decode(int* dataSrc, int dataLen, int bitsLen, int ecLevel);

// 2bit符号大小的RS纠错编码
void RS2Encode(const BYTE *dataPtr, int dataLen, int *result);

// 2bit符号大小的RS纠错解码
bool RS2Decode(const int *dataSrc, int dataLen, int *result);