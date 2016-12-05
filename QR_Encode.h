// QR_Encode.h : CQR_Encode
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

/************************************************************************/
// QR码编码的头文件
// 如果需要移植到Linux，请#define ANDROID_CPP
// 2016/8/24 注
/************************************************************************/

#if !defined(AFX_QR_ENCODE_H)
#define AFX_QR_ENCODE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataTypes.h"

/////////////////////////////////////////////////////////////////////////////
//

// 错误纠错等级
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// 编码模式
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// QR版本分类
#define QR_VRESION_S	0 // 1 ~ 9
#define QR_VRESION_M	1 // 10 ~ 26
#define QR_VRESION_L	2 // 27 ~ 40

#define MAX_ALLCODEWORD	 3706 // 最大全部编码数据
#define MAX_DATACODEWORD 2956 // 最大可编码数据(40-L)
#define MAX_CODEBLOCK	  153 // 最大数据码块
#define MAX_MODULESIZE	  177 // 最大二维码尺寸

// QR 边界
#define QR_MARGIN	4


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// 里德所罗门编码块
	int ncAllCodeWord;	// 
	int ncDataCodeWord;	// 

} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
// QR版本信息结构体

typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // 版本号(1~40)
	int ncAllCodeWord; // 

	// 错误纠错率(0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	// 

	int ncAlignPoint;	// 
	int nAlignPoint[6];	// 

	RS_BLOCKINFO RS_BlockInfo1[4]; // (1)
	RS_BLOCKINFO RS_BlockInfo2[4]; // (2)

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// CQR_Encode 头文件

class CQR_Encode
{
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		// 纠错水平
	int m_nVersion;		// QR版本
	BOOL m_bAutoExtent;	// 自动扩展
	int m_nMaskingNo;	// 掩码版本

public:
	int m_nSymbleSize;	// 二维码大小
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// bit5:
	// bit4:
	// bit1:
	// bit0:
	// 20h

private:
	int m_ncDataCodeWordBit; // 
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; // 

	int m_ncDataBlock;// 
	BYTE m_byBlockMode[MAX_DATACODEWORD];// 
	int m_nBlockLength[MAX_DATACODEWORD];// 

	int m_ncAllCodeWord; // 
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // 
	BYTE m_byRSWork[MAX_CODEBLOCK]; // 

public:

#ifndef ANDROID_CPP
	/* 编码数据 */
	BOOL EncodeData(int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, LPCSTR lpsSource, int ncSource = 0);
#endif
	
	/* 字符串已转换为utf-8 */
	BOOL EncodeDataUtf8(int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, char* lpsSource, int ncLength);

private:
#ifndef ANDROID_CPP
	/* 获取编码版本 */
	int GetEncodeVersion(int nVersion, LPCSTR lpsSource, int ncLength);
#endif

	/* 字符串已转换为utf-8 */
	int GetEncodeVersionUtf8(int nVersion, char* lpsSource, int ncLength);

#ifndef ANDROID_CPP
	/* 编码数据 */
	BOOL EncodeSourceData(LPCSTR lpsSource, int ncLength, int nVerGroup);
#endif

	/* 字符串已转换为utf-8 */
	BOOL EncodeSourceDataUtf8(char *lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	BOOL IsNumeralData(unsigned char c);
	BOOL IsAlphabetData(unsigned char c);
	BOOL IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(LPBYTE lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QR_ENCODE_H)
