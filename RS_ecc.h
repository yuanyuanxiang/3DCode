#pragma once

/************************************************************************/
// @file		RS_ecc.h
// @brief		RS 纠错编码
// @version		1.0 
// @date		2016/9/14
// @note		可以跨平台
// @author		袁沅祥
// 2016/8/26 建立此文档
/************************************************************************/

// 2016/9/21 提取字符串中的bits流
void GetBitsStream(BYTE *pSrc, int *pDest, int nCount);

// 已弃用：计算字符每个二进制位，存在bits里面，从低位到高位排列
void Byte2Bits(BYTE ch, int bits[8]);

// 已弃用：计算整数的32个二进制位(前n个)
void Int2Bits(int num, int bits[32], int nPrev = 32);

// 计算8个bit位对应的BYTE
int Bits2Byte(int bits[8]);

// 已弃用 old版本：将字符串按4bits分割
void SplitByteBy4Bits_old(BYTE* dataPtr, int dataLen, int* dst, int dstLen);

// 将字符串按4bits分割
void SplitByteBy4Bits(BYTE* dataPtr, int dataLen, int* dst, int dstLen);

// 将数据按4bits位数合并
void MergedIntBy4Bits(int* dataSrc, int totalBits);

// 已弃用 old版本：字符串按2bits分割
void SplitByteBy2Bits_old(BYTE* dataPtr, int dataLen, int* dst, int dstLen);

// 字符串按2bits分割
void SplitByteBy2Bits(BYTE* dataPtr, int dataLen, int* dst, int dstLen);

// 将数据按2bits位数合并
void MergedIntBy2Bits(int* dataSrc, int totalBits);

// 获取RS4编码的比特总数
int RS4BitsLen(int dataLen, int ecLevel);

// 4bit符号大小的RS纠错编码
int* RS4Encode(BYTE* dataPtr, int dataLen, int &bitsLen, int ecLevel);

// 4bit符号大小的RS纠错解码
int* RS4Decode(int* dataSrc, int dataLen, int ecLevel);

// 2bit符号大小的RS纠错编码
void RS2Encode(BYTE *dataPtr, int dataLen, int *result);

// 2bit符号大小的RS纠错解码
void RS2Decode(int *dataSrc, int dataLen, int *result);

// 编码头：2bit符号大小的RS纠错编码
void RS2Encode16Bits(BYTE dataPtr[2], int result[48]);

// 将48位进行RS解码，得到2字节
void RS2Decode16Bits(int dataSrc[48], int result[16]);

// 编码头：4bit符号大小的RS纠错编码
void RS2Encode14Bits(BYTE dataPtr[2], int result[42]);

// 将48位进行RS解码，得到14位
void RS2Decode14Bits(int dataSrc[42], int result[14]);