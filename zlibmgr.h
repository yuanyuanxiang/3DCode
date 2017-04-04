#ifndef _ZLIBMGR
#define _ZLIBMGR

/**
* @file zlibmgr.h
* @brief zlib编码解码
*/

/// 10KB
#define  MAXBUFFERSIZE 10240

#include "zlib/zlib.h"

/**
* @class CZlibMgr
* @brief zlib库接口
*/
class CZlibMgr
{
public:
	CZlibMgr();
	~CZlibMgr();

	// 压缩 - pcContentBuf 要压缩的内容;pcCompBuf 压缩后的内容;ulCompLen 压缩后的长度
	bool Compress(const char* pcContentBuf, char* pcCompBuf, unsigned long& ulCompLen);

	// 解压 - pcCompBuf 压缩的内容;pcUnCompBuf 解压后的内容;ulCompLen 压缩内容的长度
	uLong UnCompress(const char* pcCompBuf, char* pcUnCompBuf, unsigned long ulCompLen);

private:
	Byte m_zlibBuffer[MAXBUFFERSIZE];
};


#endif