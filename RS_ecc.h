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
int RS4BitsLen(int dataLen, int ecLevel);

// 4bit符号大小的RS纠错编码
int* RS4Encode(const BYTE* dataPtr, int dataLen, int bitsLen, int ecLevel);

// 4bit符号大小的RS纠错解码
int* RS4Decode(int* dataSrc, int dataLen, int bitsLen, int ecLevel);

// 2bit符号大小的RS纠错编码
void RS2Encode(const BYTE *dataPtr, int dataLen, int *result);

// 2bit符号大小的RS纠错解码
void RS2Decode(int *dataSrc, int dataLen, int *result);

// 编码头：2bit符号大小的RS纠错编码
void RS2Encode16Bits(const BYTE dataPtr[2], int result[48]);

// 将48位进行RS解码，得到2字节
bool RS2Decode16Bits(int dataSrc[48], int result[16]);

// 编码头：4bit符号大小的RS纠错编码
void RS2Encode14Bits(const BYTE dataPtr[2], int result[42]);

// 将48位进行RS解码，得到14位
bool RS2Decode14Bits(int dataSrc[42], int result[14]);