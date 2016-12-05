#include "stdafx.h"
#include "zlibmgr.h"
#include <string>


CZlibMgr::CZlibMgr()
{
}


CZlibMgr::~CZlibMgr()
{
}


/* - 压缩字符串 - 
const char* pcContentBuf：待压缩字符串
char* pcCompBuf：压缩后字符串
unsigned long& ulCompLen：压缩后字符串长度
*/
bool CZlibMgr::Compress(const char* pcContentBuf, char* pcCompBuf, unsigned long& ulCompLen)
{
	if (NULL == pcContentBuf)
	{
		return false;
	}

	uLong sourceLen = strlen(pcContentBuf);
	if (0 == sourceLen)
	{
		return false;
	}

	// 初始化zlib缓存数据
	memset(m_zlibBuffer, 0, MAXBUFFERSIZE);
	uLong comprLen = sizeof(m_zlibBuffer) / sizeof(m_zlibBuffer[0]);

	int err = compress(m_zlibBuffer, &comprLen, (const Bytef*)pcContentBuf, sourceLen);
	if (err != Z_OK)
	{
		TRACE(" * 压缩出错: %d\n", err);
		return false;
	}

	TRACE(" * 原尺寸:%ld, 压缩后:%ld, 压缩比:%.3f\n", sourceLen, comprLen, 1.0 * comprLen / sourceLen);
	memcpy(pcCompBuf, m_zlibBuffer, comprLen);
	ulCompLen = comprLen;

	return true;
}


/* - 解压缩字符串 - 
const char* pcCompBuf：压缩后字符串
char* pcUnCompBuf：解压后字符串
unsigned long ulCompLen：压缩后字符串长度
返回值：返回解压后字符串长度
*/
uLong CZlibMgr::UnCompress(const char* pcCompBuf, char* pcUnCompBuf, unsigned long ulCompLen)
{
	if (pcCompBuf == NULL || strlen(pcCompBuf) == 0)
	{
		return 0;
	}

	// 初始化zlib缓存数据
	memset(m_zlibBuffer, 0, MAXBUFFERSIZE);
	uLong sourceLen = MAXBUFFERSIZE;

	int err = uncompress(m_zlibBuffer, &sourceLen, (const Bytef *)pcCompBuf, ulCompLen);
	if (err != Z_OK)
	{
		TRACE(" * 解压出错: %d\n", err);
		return 0;
	}

	TRACE(" * 原尺寸:%ld, 压缩后:%ld, 压缩比:%.3f\n", sourceLen, ulCompLen, 1.0 * ulCompLen / sourceLen);
	memcpy(pcUnCompBuf, m_zlibBuffer, sourceLen + 1);
	pcUnCompBuf[sourceLen] = 0;

	return sourceLen;
}