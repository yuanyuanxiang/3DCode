#include "stdafx.h"
#include "CodeTransform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }	//安全删除指针p
#endif

// 将Unicode编码转换成ANSI编码(需要手动delete转换结果)
char* UnicodeConvert2ANSI(LPCWCH strUnicode, int &ncLength)
{
	ncLength = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	char *strANSI = new char[ncLength];
	ncLength = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strANSI, ncLength, NULL, NULL);
	strANSI[ncLength - 1] = 0;
	return strANSI;
}


// 将ANSI编码转换成Unicode编码
CString ANSIConvert2Unicode(char *strANSI, int &ncLength)
{
	ncLength = MultiByteToWideChar(CP_ACP, 0, strANSI, -1, NULL, 0);
	wchar_t *strUnicode = new wchar_t[ncLength];
	ncLength = MultiByteToWideChar(CP_ACP, 0, strANSI, -1, strUnicode, ncLength * sizeof(wchar_t));
	CString strDst(strUnicode);
	SAFE_DELETE(strUnicode);
	return strDst;
}


// 将Unicode编码转换成UTF-8编码(需要手动delete转换结果)
char* UnicodeConvert2UTF8(LPCWCH strUnicode, int &ncLength)
{
	ncLength = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	// 并没有输入字符
	if (ncLength == 1)
		return NULL;
	char *strUtf8 = new char[ncLength];
	ncLength = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUtf8, ncLength, NULL, NULL);
	strUtf8[ncLength - 1] = 0;
	return strUtf8;
}


// 将utf-8编码转换成Unicode编码
CString UTF8Convert2Unicode(char* strUtf8, int &ncLength)
{
	ncLength = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
	WCHAR *strUnicode = new WCHAR[ncLength];
	ncLength = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, strUnicode, ncLength);
	CString strDst(strUnicode);
	SAFE_DELETE(strUnicode);
	return strDst;
}


// 将ANSI编码转换成UTF-8编码
char* ANSIConvert2UTF8(char* strANSI, int &ncLength)
{
	CString strUnicode = ANSIConvert2Unicode(strANSI, ncLength);
	char *strUtf8 = UnicodeConvert2UTF8(strUnicode, ncLength);
	return strUtf8;
}


// 将UTF8编码转换成ANSI编码
char* UTF8Convert2ANSI(char* strUtf8, int &ncLength)
{
	CString strUnicode = UTF8Convert2Unicode(strUtf8, ncLength);
	char *strANSI = UnicodeConvert2ANSI(strUnicode, ncLength);
	return strANSI;
}