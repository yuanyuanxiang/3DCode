#include "stdafx.h"
#include "CyImage.h"
#include <new>

#include <algorithm>
using namespace std;

#include "FileIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/** - 为8位色图像设置颜色表 - */
void SetColorTabFor8BitImage(CImage *pImage)
{
	ASSERT(pImage && !pImage->IsNull() && 8 == pImage->GetBPP());
	if (8 != pImage->GetBPP())
		return;
	RGBQUAD ColorTab[256];
	for (int i = 0; i < 256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
		ColorTab[i].rgbReserved = 0;
	}
	pImage->SetColorTable(0, 256, ColorTab);
}


// 从file path获取file name(默认带后缀)
CString GetFileName(CString strFilePath, BOOL bHasPostfix)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('\\');
	CString name = strFilePath.Right(nLength - nPos - 1);
	// 获取没有后缀的文件名
	if (bHasPostfix == FALSE)
	{
		int nDotPos = name.ReverseFind('.');
		name = name.Left(nDotPos);
	}
	return name;
}


// 从file path获取file ext
CString GetFileExt(CString strFilePath)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('.');
	CString ext = strFilePath.Right(nLength - nPos - 1);
	return ext;
}


// 数字转为CString
CString Num2String(int nNum)
{
	CString str;
	str.Format(_T("%d"), nNum);
	return str;
}


/** 在目标图像挖出一块矩形用源图像替换，LOGO必须大于3通道 */
void ReplacedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect)
{
	return MixedLogo(pDstImage, pSrcImage, LogoRect, 0);
}


/** 在目标图像挖出一块矩形与源图像混合，dst_rate指明DstImage所占比例 */
void MixedLogo(CyImage* pDstImage, CyImage* pSrcImage, CLogoRect LogoRect, float dst_rate)
{
	// 当中一个为空就返回
	if (pSrcImage->IsNull() || pSrcImage->GetWidth() < 23 || pDstImage->IsNull())
		return;
	// 当Logo太大时返回
	if (LogoRect.Width() > pDstImage->GetWidth() || LogoRect.Height() > pDstImage->GetHeight())
		return;
	ASSERT(pSrcImage->GetChannel() >= 3);
	int nDstRowlen = pDstImage->GetFloatDataRowlen();
	int nDstChannel = pDstImage->GetChannel();
	int nLogoWidth = LogoRect.Width();
	int nLogoHeight = LogoRect.Height();
	ImageTransform pSrc = pSrcImage->Zoom(nLogoWidth, nLogoHeight, TRUE);
	int nSrcChannel = pSrcImage->GetChannel();
	int nSrcRowlen = nLogoWidth * nSrcChannel;
	float *pDst = pDstImage->GetFloatDataHead();
	for (int i = 0; i < nLogoHeight; ++i)
	{
		float* pRow = pDst + LogoRect.left * nDstChannel + (i + LogoRect.top) * nDstRowlen;
		const float* pBits = pSrc.GetImage() + i * nSrcRowlen;
		for (int j = 0; j < nLogoWidth; ++j)
		{
			float* pCur = pRow + j * nDstChannel;
			const float* p = pBits + j * nSrcChannel;
			pCur[0] = dst_rate * pCur[0] + (1 - dst_rate) * p[0];
			pCur[1] = dst_rate * pCur[1] + (1 - dst_rate) * p[1];
			pCur[2] = dst_rate * pCur[2] + (1 - dst_rate) * p[2];
		}
	}

	pDstImage->MemcpyFloatToByte();
}


/** - 默认的构造函数 - */
CyImage::CyImage()
{
	CImage::CImage();
	m_pFloatData = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
}


/** - 析构函数 - */
CyImage::~CyImage()
{
	SAFE_DELETE(m_pFloatData);
	CImage::~CImage();
}


/** - 拷贝构造函数 - */
CyImage::CyImage(const CyImage &pImage)
{
	m_pFloatData = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
	BOOL nRet = Create(pImage.GetWidth(), pImage.GetHeight(), pImage.GetBPP(), 0);
	ASSERT(nRet);
	float *pDst = GetFloatDataHead();
	float *pSrc = pImage.GetFloatDataHead();
	memcpy(pDst, pSrc, GetHeight() * GetFloatDataRowlen() * sizeof(float));
	MemcpyFloatToByte();
}


/** - 赋值操作运算 - */
CyImage CyImage::operator = (const CyImage &pImage)
{
	// 自己给自己赋值
	if (&pImage == this)
		return *this;
	Destroy();
	m_pFloatData = NULL;
	m_fMaximum = 0;
	m_fMinimum = 0;
	BOOL nRet = Create(pImage.GetWidth(), pImage.GetHeight(), pImage.GetBPP(), 0);
	ASSERT(nRet);
	float *pDst = GetFloatDataHead();
	float *pSrc = pImage.GetFloatDataHead();
	memcpy(pDst, pSrc, GetHeight() * GetFloatDataRowlen() * sizeof(float));
	MemcpyFloatToByte();
	return *this;
}


/** - 获取浮点数据的头指针 - */
float* CyImage::GetFloatDataHead() const
{
	return m_pFloatData;
}


/** - 获取图像每行浮点数据个数 - */
int CyImage::GetFloatDataRowlen() const
{
	return GetWidth() * GetChannel();
}


/** - 为图像拷贝浮点数据 - 
* @param[in] *pSrc 数据源
* @param[in] nRowlen 数据源每行浮点个数
* @param[in] nHeight 数据源的高度
* @return 真或假
*/
BOOL CyImage::SetFloatData(float* pSrc, int nRowlen, int nHeight)
{
	if (m_pFloatData == NULL || nRowlen != GetFloatDataRowlen() || nHeight != GetHeight())
		return FALSE;

	memcpy(m_pFloatData, pSrc, nRowlen * nHeight * sizeof(float));

	return TRUE;
}


/** - 获取浮点数据的最大值 - */
float CyImage::GetMaximum() const
{
	return m_fMaximum;
}


/** - 获取浮点数据的最小值 - */
float CyImage::GetMinimum() const
{
	return m_fMinimum;
}


/** - 计算浮点数据的最大、最小值 - */
void CyImage::ComputeMinMax()
{
	// 寻找最大最小值
	pair<float*, float*> minmax_pair = minmax_element(m_pFloatData, 
		m_pFloatData + GetHeight() * GetFloatDataRowlen());

	m_fMinimum = *minmax_pair.first;
	m_fMaximum = *minmax_pair.second;
}


/** - 获得图像数据块的首地址 - */
BYTE* CyImage::GetHeadAddress() const
{
	return (BYTE*) GetBits() + GetPitch() * (GetHeight() - 1);
}


/** - 获得图像某一行的首地址 - */
BYTE* CyImage::GetLineAddress(int nLineID) const
{
	return GetHeadAddress() + nLineID * abs(GetPitch());
}


/** - 获得图像的通道个数 - */
int CyImage::GetChannel() const
{
	return GetBPP() / 8;
}


/** - 获得图像数据每行的字节数 - */
int CyImage::GetRowlen() const
{
	return abs(GetPitch());
}


/** - 获得图像数据总的字节数 - */
int CyImage::GetLength() const
{
	return GetHeight() * abs(GetPitch());
}


/** - 获得图像的对角线的长度 - */
int CyImage::GetDiagLength() const
{
	return (int)ceil(sqrt(1.0f * GetWidth() * GetWidth() + GetHeight() * GetHeight()));
}


/** - 获得图像的宽度、高度 - */
void CyImage::GetInfomation(int &nWidth, int &nHeight) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
}


/** - 获得图像的宽度、高度、每行字节数 - */
void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
}


/** - 获得图像的宽度、高度、每行字节数、每像素字节数 - */
void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
	nBPP = GetBPP();
}


/** - 获得图像的宽度、高度、每行字节数、每像素字节数、通道个数 - */
void CyImage::GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel) const
{
	nWidth = GetWidth();
	nHeight = GetHeight();
	nRowlen = abs(GetPitch());
	nBPP = GetBPP();
	nChannel = nBPP / 8;
}


/** - 初始化浮点数据 - */
BOOL CyImage::InitFloatData()
{
	SAFE_DELETE(m_pFloatData);
	int nHeight = GetHeight();
	int nRowlen = GetFloatDataRowlen();
	try
	{
		m_pFloatData = new float[nHeight * nRowlen];
	}
	catch (const std::bad_alloc)
	{
		return FALSE;
	}
	memset(m_pFloatData, 0, nHeight * nRowlen * sizeof(float));
	return TRUE;
}


/** - 拷贝数据源创建图像 - 
* @param[in] *pSrc 源数据
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nBPP 位深度
* @return 成功或失败
* @note pSrc的数据长度务必与所创建图像字节数一致，即:
		length(pSrc) = 所创建的图像总的字节数
*/
BOOL CyImage::Create(const BYTE* pSrc, int nWidth, int nHeight, int nBPP) throw()
{
	Destroy();// 先销毁图像数据
	if (FALSE == CImage::Create(nWidth, nHeight, nBPP, 0UL))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	memcpy(GetHeadAddress(), pSrc, GetLength() * sizeof(BYTE));
	InitFloatData();
	MemcpyByteToFloat();
	return TRUE;
}


/** - 创建图像 - 
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nBPP 图像位深度
* @param[in] dwFlags 图像初值
* @return 成功或失败
*/
BOOL CyImage::Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags) throw()
{
	Destroy();
	if (FALSE == CImage::Create(nWidth, nHeight, nBPP, dwFlags))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	InitFloatData();
	MemcpyByteToFloat();
	return TRUE;
}


/** - 从数据源创建图像 - 
* @param[in] *pSrc 数据源
* @param[in] nWidth 图像宽度
* @param[in] nHeight 图像高度
* @param[in] nRowlen 数据源每行数据个数
* @return 成功或失败
*/
BOOL CyImage::Create(const float* pSrc, int nWidth, int nHeight, int nRowlen) throw()
{
	if(NULL == pSrc)
		return FALSE;
	Destroy();
	int nBPP = 8 * nRowlen / nWidth;
	if (FALSE == CImage::Create(nWidth, nHeight, nBPP))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	InitFloatData();
	memcpy(m_pFloatData, pSrc, nHeight * nRowlen * sizeof(float));
	MemcpyFloatToByte();
	return TRUE;
}


/** - 从ImageSrc创建图像 - */
BOOL CyImage::Create(const ImageSrc *pSrc) throw()
{
	Destroy();// 先销毁图像数据
	if (FALSE == CImage::Create(pSrc->GetWidth(), pSrc->GetHeight(), pSrc->GetBPP(), 0UL))
		return FALSE;
	if (GetBPP() == 8)
		SetColorTabFor8BitImage(this);
	memcpy(GetHeadAddress(), pSrc->GetHeadAddress(), GetLength() * sizeof(BYTE));
	InitFloatData();
	MemcpyByteToFloat();
	return TRUE;
}


/** - 从ImageTransform创建图像 - */
BOOL CyImage::Create(const ImageTransform *pSrc) throw()
{
	if(NULL == pSrc)
		return FALSE;
	Destroy();
	int nBPP = 8 * pSrc->GetRowlen() / pSrc->GetWidth();
	if (FALSE == CImage::Create(pSrc->GetWidth(), pSrc->GetHeight(), nBPP))
		return FALSE;
	if (nBPP == 8)
		SetColorTabFor8BitImage(this);
	InitFloatData();
	memcpy(m_pFloatData, pSrc, pSrc->GetHeight() * pSrc->GetRowlen() * sizeof(float));
	MemcpyFloatToByte();
	return TRUE;
}


/** - 加载图像 - */
HRESULT CyImage::Load(LPCTSTR pszFileName) throw()
{
	// 如果文件为TXT
	CString strPathName(pszFileName);
	CString strExt = GetFileExt(strPathName);
	strExt.MakeUpper();
	if (strExt == _T("TXT"))
	{
		int nWidth = 0, nHeight = 0, nRowlen = 0, nChannel = 0;
		USES_CONVERSION;
		BYTE *pSrc = ReadTxt(W2A(strPathName), nWidth, nHeight, nRowlen, nChannel);
		if (pSrc == NULL)
			return E_FAIL;
		Create(nWidth, nHeight, 8 * nChannel, 0UL);
		memcpy(GetHeadAddress(), pSrc, nRowlen * nHeight);
		MemcpyByteToFloat();
		SAFE_DELETE(pSrc);
		return S_OK;
	}
	// 摧毁当前图像
	Destroy();
	// 加载进图像
	HRESULT hr = CImage::Load(pszFileName);
	if (hr != S_OK)
		return hr;
	InitFloatData();
	MemcpyByteToFloat();
	return S_OK;
}


/** - 同步数据 - */
void CyImage::MemcpyByteToFloat()
{
	BYTE* pHead = GetHeadAddress();
	float fRate = (m_fMaximum - m_fMinimum) / 255.f;
	float fMinimum = m_fMinimum;
	if (m_fMaximum == m_fMinimum)
	{
		fRate = 1.f;
		fMinimum = 0;
	}

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();
	int nFloatRowlen = GetFloatDataRowlen();
	if (NULL == m_pFloatData)
	{
		m_pFloatData = new float[nHeight * nFloatRowlen];
	}
	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, z = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
				m_pFloatData[k + x + z] = fRate * pHead[k + x + y] + fMinimum;
		}
	}
	ComputeMinMax();
}


/** - 同步数据 - */
void CyImage::MemcpyFloatToByte()
{
	ComputeMinMax();
	BYTE* pHead = GetHeadAddress();
	// 如果浮点数据是常量
	if (m_fMaximum == m_fMinimum)
	{
		TRACE(" * 警告：浮点数据全部是一样的!\n");
		BYTE val = (0 <= m_fMinimum && m_fMinimum < 256) ? (BYTE)m_fMinimum : 0;
		memset(pHead, val, GetHeight() * GetRowlen() * sizeof(BYTE));
		return;
	}

	float fRate = 255.f / (m_fMaximum - m_fMinimum);

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();
	int nFloatRowlen = GetFloatDataRowlen();

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, z = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
				pHead[k + x + y] = BYTE(fRate * (m_pFloatData[k + x + z] - m_fMinimum));
		}
	}
}


/** - 同步数据 - 
* @param[in] lower 下限
* @param[in] upper 上限
* @return 无
* @note 该函数会将超界的元素设为边界值
*/
void CyImage::MemcpyFloatToByteBounded(float lower, float upper)
{
	BYTE* pHead = GetHeadAddress();

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();
	int nFloatRowlen = GetFloatDataRowlen();

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y = j * nRowlen, z = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
			{
				if (m_pFloatData[k + x + z] < lower)
					pHead[k + x + j * nRowlen] = (BYTE)lower;
				else if (m_pFloatData[k + x + z] > upper)
					pHead[k + x + y] = (BYTE)upper;
				else
					pHead[k + x + y] = (BYTE)m_pFloatData[k + x + z];
			}
		}
	}
}


/** - 对图像旋转一个角度 - */
void CyImage::Rotate(float degree)
{
	float angle = RAD(degree);
	InitFloatData();
	MemcpyByteToFloat();
	ImageTransform it(m_pFloatData, GetWidth(), GetHeight(), GetChannel());
	ImageTransform pDst = it.ImageRotate(PositionTransform(angle, 0, 0), CLogoRect());
	Create(pDst, pDst.GetWidth(), pDst.GetHeight(), pDst.GetRowlen());
}


/** - 旋转图像，并返回旋转得到的图像数据 - 
* @param[in] angle 旋转角度
* @return 新图像数据块
*/
ImageTransform CyImage::Rotate(const PositionTransform &pt, CLogoRect &dstArea) const
{
	ImageTransform it(m_pFloatData, GetWidth(), GetHeight(), GetChannel());
	return it.ImageRotate(pt, dstArea);
}


/** - 缩放图像 -
* @param[in] NewWidth 新图像宽度
* @param[in] NewHeight 新图像高度
* @param[in] bNeededReturn 需要返回新图像数据块头指针
* @return 新图像数据块
*/
ImageTransform CyImage::Zoom(int NewWidth, int NewHeight, int bNeededReturn) const
{
	ImageTransform it(m_pFloatData, GetWidth(), GetHeight(), GetChannel());
	return it.ImageZoom(NewWidth, NewHeight);
}


/** - 缩放图像 -
* @param[in] rate 缩放比例
* @return 无
*/
void CyImage::Zoom(float rate)
{
	int NewWidth = int(GetWidth() * rate);
	int NewHeight = int(GetHeight() * rate);
	Zoom(NewWidth, NewHeight);
}


/** - 缩放图像 -
* @param[in] NewWidth 新图像宽度
* @param[in] NewHeight 新图像高度
* @return 无
*/
void CyImage::Zoom(int nNewWidth, int nNewHeight)
{
	if (nNewWidth == GetWidth() && nNewHeight == GetHeight())
		return;
	ImageTransform it(m_pFloatData, GetWidth(), GetHeight(), GetChannel());
	ImageTransform pDst = it.ImageZoom(nNewWidth, nNewHeight);
	Create(pDst, nNewWidth, nNewHeight, nNewWidth * GetChannel());
}


/** - 图像将转换为8位灰度图 - */
void CyImage::ToGray()
{
	ChangeBPP(8);
}


/** - 保存图像 - */
HRESULT CyImage::Save(LPCTSTR pszFileName, REFGUID guidFileType) const throw()
{
	// 尝试保存为txt文件
	if (GetFileExt(pszFileName).CompareNoCase(_T("txt")) == 0)
	{
		USES_CONVERSION;
		if (WriteTxt(W2A(pszFileName), GetHeadAddress(), GetWidth(), GetHeight(), GetChannel()))
			return S_OK;
	}
	HRESULT hr = CImage::Save(pszFileName, guidFileType);
	return hr;
}


#ifdef _AFX // MFC 环境下编译


/** - 加载图像 - 弹出选择文件对话框 */
HRESULT CyImage::Load(CWnd* pParentWnd) throw()
{
	// 过滤器
	CString strFilter = L"所有图像|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*.JFIF;*.GIF;*.TIF;"\
		L"*.TIFF;*.PNG;*.ICO|BMP (*.BMP;*.DIB;*.RLE)|*.BMP;*.DIB;*.RLE|JPEG (*.JPG;*.JPEG;"	\
		L"*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|GIF (*.GIF)|*.GIF|图标 (*.ICO)|*.ICO|所有文件|*.*||";

	// 文件对话框
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, strFilter, pParentWnd);
	hFileDlg.m_ofn.nFilterIndex = 1;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("打开图像\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if (hFileDlg.DoModal() != IDOK)
		return E_FAIL;

	// 调用其他Load函数
	CString strFilePath = hFileDlg.GetPathName();
	return Load(strFilePath);
}


/** - 保存图像 - */
BOOL CyImage::Save(REFGUID guidFileType) const throw()
{
	return Save((CWnd*)NULL, guidFileType);
}


/** - 保存图像 - 
* @details 
guidFileType
The file type to save the image as. Can be one of the following:
ImageFormatBMP   An uncompressed bitmap image.
ImageFormatPNG   A Portable Network Graphic (PNG) compressed image.
ImageFormatJPEG   A JPEG compressed image.
ImageFormatGIF   A GIF compressed image.
Note   For a complete list of constants, see Image File Format Constants in the Platform SDK.
*/
BOOL CyImage::Save(CWnd* pParentWnd, REFGUID guidFileType) const throw()
{
	// 过滤器
	CString strFilter = L"所有图像|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*."	\
		L"JFIF;*.GIF;*.TIF;*.TIFF;*.PNG;*.ICO|BMP (*.BMP;*.DIB;*.RLE)|*.BMP;"\
		L"*.DIB;*.RLE|JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;"	\
		L"*.JFIF|GIF (*.GIF)|*.GIF|图标 (*.ICO)|*.ICO|所有文件|*.*||";

	// 获取系统时间
	SYSTEMTIME CurTime;
	CString strDate, strTime;
	GetLocalTime(&CurTime);
	strDate.Format(_T("%4d年%2d月%2d日"), CurTime.wYear, CurTime.wMonth, CurTime.wDay);
	strTime.Format(_T("%2d时%2d分%2d秒"), CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	// 文件对话框
	CFileDialog hFileDlg(FALSE, _T(".BMP"), strDate + strTime, OFN_FILEMUSTEXIST 
		| OFN_PATHMUSTEXIST | OFN_READONLY, strFilter, pParentWnd);
	hFileDlg.m_ofn.nFilterIndex = 1;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("保存图像\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if (hFileDlg.DoModal() != IDOK)
		return TRUE;

	// 获取保存路径
	CString strFile = hFileDlg.GetFileName();
	CString strPath = hFileDlg.GetPathName();
	CString strExt = hFileDlg.GetFileExt();
	if (strFile.IsEmpty())
		strPath = strPath + strDate + strTime;
	if (strExt.IsEmpty())
	{
		switch (hFileDlg.m_ofn.nFilterIndex)
		{
		case 1 :strExt = _T("BMP"); break;
		case 2 :strExt = _T("BMP"); break;
		case 3 :strExt = _T("JPG"); break;
		case 4 :strExt = _T("GIF"); break;
		case 5 :strExt = _T("ICO"); break;
		case 6 :strExt = _T("JPG"); break;
		default:strExt = _T("JPG"); break;
		}
		strPath = strPath + _T(".") + strExt;
	}
	
	return SUCCEEDED(Save(strPath, guidFileType));
}


#endif // _AFX


/** - 安全销毁所关联的位图 - */
void CyImage::Destroy() throw()
{
	if (!IsNull())
	{
		CImage::Destroy();
		SAFE_DELETE(m_pFloatData);
	}
}


/** - 水平翻转图像 - */
void CyImage::FlipH()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = GetChannel();
	int nRowlen = GetRowlen();

	BYTE* temp = new BYTE[nHeight * nRowlen];
	BYTE* head = GetHeadAddress();
	memcpy(temp, head, nHeight * nRowlen * sizeof(BYTE));

	for (int r = 0; r < nHeight; ++r)
	{
		int y = r * nRowlen;
		for (int c = 0; c < nWidth; ++c)
		{
			memcpy(head + c *nChannel + y, temp + (nWidth - 1 - c) * nChannel + y, 
				nChannel * sizeof(BYTE));
		}
	}
	SAFE_DELETE(temp);
}


/** - 垂直翻转图像 - */
void CyImage::FlipV()
{
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();

	BYTE* temp = new BYTE[nHeight * nRowlen * sizeof(BYTE)];
	BYTE* head = GetHeadAddress();
	memcpy(temp, head, nHeight * nRowlen * sizeof(BYTE));

	for (int r = 0; r < nHeight; ++r)
	{
		memcpy(head + r * nRowlen, temp + (nHeight - 1 - r) * nRowlen, nRowlen * sizeof(BYTE));
	}
	SAFE_DELETE(temp);
}


/** - 转置图像 - */
void CyImage::Transpose()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();
	int nChannel = GetChannel();
	InitFloatData();
	MemcpyByteToFloat();
	int nFloatRowlen = GetFloatDataRowlen();

	float* temp = new float[nHeight * nFloatRowlen];
	memcpy(temp, m_pFloatData, nHeight * nFloatRowlen * sizeof(float));
	BYTE* temp2 = new BYTE[nHeight * nRowlen];
	memcpy(temp2, GetHeadAddress(), nHeight * nRowlen * sizeof(BYTE));
	Create(nHeight, nWidth, nChannel * 8, 0UL);

	int nNewWidth = GetWidth();
	int nNewHeight = GetHeight();
	int nNewFloatRowlen = GetFloatDataRowlen();
	BYTE *pHead = GetHeadAddress();
	int nNewRowlen = GetRowlen();
	for (int i = 0; i < nNewHeight; ++i)
	{
		int y = i * nNewRowlen, z = i * nNewFloatRowlen;
		for (int j = 0; j < nNewWidth; ++j)
		{
			int x = (nNewWidth - 1 - j) * nChannel, x0 = i * nChannel;
			int y0 = j * nRowlen, z0 = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
			{
				m_pFloatData[k + x + z] = temp[k + x0 + z0];
				pHead[k + x + y] = temp2[k + x0 + y0];
			}
		}
	}
	SAFE_DELETE(temp);
	SAFE_DELETE(temp2);
}


/** - 向右转置图像 - */
void CyImage::TransposeR()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();
	int nChannel = GetChannel();
	InitFloatData();
	MemcpyByteToFloat();
	int nFloatRowlen = GetFloatDataRowlen();

	float* temp = new float[nHeight * nFloatRowlen];
	memcpy(temp, m_pFloatData, nHeight * nFloatRowlen * sizeof(float));
	BYTE* temp2 = new BYTE[nHeight * nRowlen];
	memcpy(temp2, GetHeadAddress(), nHeight * nRowlen * sizeof(BYTE));
	Create(nHeight, nWidth, nChannel * 8, 0UL);

	int nNewWidth = GetWidth();
	int nNewHeight = GetHeight();
	int nNewFloatRowlen = GetFloatDataRowlen();
	BYTE *pHead = GetHeadAddress();
	int nNewRowlen = GetRowlen();
	for (int i = 0; i < nNewHeight; ++i)
	{
		int y = (nNewHeight - 1 - i) * nNewRowlen;
		int z = (nNewHeight - 1 - i) * nNewFloatRowlen;
		for (int j = 0; j < nNewWidth; ++j)
		{
			int x = j * nChannel, x0 = i * nChannel;
			int y0 = j * nRowlen, z0 = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
			{
				m_pFloatData[k + x + z] = temp[k + x0 + z0];
				pHead[k + x + y] = temp2[k + x0 + y0];
			}
		}
	}
	SAFE_DELETE(temp);
	SAFE_DELETE(temp2);
}


/** - 对直方图进行排序 - */
void CyImage::GetCluster(int cluster[256]) const
{
	int hist[256];
	GetGrayHist(hist);

	for (int i = 0; i < 256; ++i)
	{
		cluster[i] = i;
	}

	for (int i = 0; i < 256; i++)
	{
		for (int j = i; j < 256; ++j)
		{
			if (hist[j] > hist[i])
			{
				// 交换次序
				SWAP(hist[j], hist[i]);
				SWAP(cluster[j], cluster[i]);
			}
		}
	}
}


/** - 获取灰度直方图 - */
BOOL CyImage::GetGrayHist(int hist[256]) const
{
	ZeroMemory(hist, 256 * sizeof(int));
	int nChannel = GetChannel();
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();

	BYTE *pSrc = GetHeadAddress();
	if (nChannel == 1)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				hist[pSrc[j * nChannel + y]]++;
			}
		}
	}
	else if (nChannel == 3 || nChannel == 4)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				int index = RGB2GRAY (pSrc[2 + j * nChannel + y], 
					pSrc[1 + j * nChannel + y], pSrc[j * nChannel + y]);
				hist[index]++;
			}
		}
	}
	return TRUE;
}


/** - 获取图像直方图 - */
BOOL CyImage::GetHistogram(int hist[4][256]) const
{
	ZeroMemory(hist, 4 * 256 * sizeof(int));
	int nChannel = GetChannel();
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nRowlen = GetRowlen();

	BYTE *pSrc = GetHeadAddress();
	if (nChannel == 1)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				hist[0][pSrc[j * nChannel + y]]++;
				hist[1][pSrc[j * nChannel + y]]++;
				hist[2][pSrc[j * nChannel + y]]++;
				hist[3][pSrc[j * nChannel + y]]++;
			}
		}
	}
	else if (nChannel == 3 || nChannel == 4)
	{
		for (int i = 0; i < nHeight; ++i)
		{
			int y = i * nRowlen;
			for (int j = 0; j < nWidth; ++j)
			{
				int x = j * nChannel;
				hist[2][pSrc[x + y]]++;
				hist[1][pSrc[1 + x + y]]++;
				hist[0][pSrc[2 + x + y]]++;
				int index = RGB2GRAY(pSrc[2 + x + y], pSrc[1 + x + y], pSrc[x + y]);
				hist[3][index]++;
			}
		}
	}
	return TRUE;
}


/** - 改变图像通道数 -  bpp：目标图像的位色 - 
修改成功返回TRUE，未修改返回FALSE。
*/
BOOL CyImage::ChangeBPP(UINT bpp)
{
	if(bpp != 8 && bpp != 24 && bpp != 32)
		return FALSE;

	int nBitCount = GetBPP();
	if (nBitCount == bpp) return FALSE;

	switch (nBitCount)
	{
	case 1:
		if (bpp == 8)     Bpp1To8();
		else if(bpp == 24)Bpp1To24();
		else if(bpp == 32)Bpp1To32();
		break;
	case 8:
		if (bpp == 24)    Bpp8To24();
		else if(bpp == 32)Bpp8To32();
		break;
	case 16:
		break;
	case 24:
		if (bpp == 8)     Bpp24To8();
		else if(bpp == 32)Bpp24To32();
		break;
	case 32:
		if (bpp == 8)     Bpp32To8();
		else if(bpp == 24)Bpp32To24();
		break;
	default:
		break;
	}
	MemcpyByteToFloat();
	return TRUE;
}


/// 8位色转24位色
void CyImage::Bpp8To24()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 1, nChannelNew = 3;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 8位色转32位色
void CyImage::Bpp8To32()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 1, nChannelNew = 4;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 24位色转8位色
void CyImage::Bpp24To8()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 3, nChannelNew = 1;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 8);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = RGB2GRAY(R, G, B);
		}
	}
	SAFE_DELETE(pSrc);
}

/// 24位色转32位色
void CyImage::Bpp24To32()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 3, nChannelNew = 4;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 32位色转8位色
void CyImage::Bpp32To8()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 4, nChannelNew = 1;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 8);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = RGB2GRAY(R, G, B);
		}
	}
	SAFE_DELETE(pSrc);
}

/// 32位色转24位色
void CyImage::Bpp32To24()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannel = 4, nChannelNew = 3;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + y1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 将二进制图像转换为8位色图像
void CyImage::Bpp1To8()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannelNew = 1;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(8 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 8);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + y1;
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*pDstTemp = 255 * temp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 将二进制图像转换为24位色图像
void CyImage::Bpp1To24()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannelNew = 3;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(24 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 24);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + y1;
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
	SAFE_DELETE(pSrc);
}

/// 将二进制图像转换为32位色图像
void CyImage::Bpp1To32()
{
	LONG lWidth = GetWidth();
	LONG lHeight = GetHeight();
	int nChannelNew = 4;
	LONG lRowLen = abs(GetPitch()), lRowLenNew = WIDTHBYTES(32 * lWidth);

	BYTE *pSrc = new BYTE[lHeight * lRowLen];
	BYTE *temp = (BYTE*)GetBits() + (GetPitch() * (GetHeight() - 1));
	memcpy(pSrc, temp, lHeight * lRowLen);

	Create(lWidth, lHeight, 32);

	BYTE *pDst = (BYTE*)GetBits() + (GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		int y1 = i * lRowLen, y2 = i * lRowLenNew;
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + y1;
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + y2;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
	SAFE_DELETE(pSrc);
}


/** - 制作备份图像 - 需要delete方法 - */
CyImage* CyImage::MakeCopy() const
{
	CyImage* pCopy = NULL;
	try
	{
		pCopy = new CyImage(*this);
	}
	catch (const std::bad_alloc)
	{
		return NULL;
	}
	
	return pCopy;
}


/** - 将图像的背景分割出来 - 
* @param[in] threahold 阈值
* @param[in] background 背景像素
* @return 背景图像
* @note 该函数返回值需要delete方法
*/
CyImage* CyImage::GetBackground(float threahold, float3 background) const
{
	CyImage* pBackground = MakeCopy();

	int nWidth = pBackground->GetWidth();
	int nHeight = pBackground->GetHeight();
	int nChannel = pBackground->GetChannel();
	int nRowlen = pBackground->GetFloatDataRowlen();
	float* pHead = pBackground->GetFloatDataHead();
	float fGray = background.ToGray();
	float fMAX = pBackground->GetMaximum();
	switch (nChannel)
	{
	case 1:// 8位色图像
		{
			for (int i = 0; i < nHeight; ++i)
			{
				int y = i * nRowlen;
				for (int j = 0; j < nWidth; ++j)
				{
					// 将前景色全部置成白色
					if (fabsf(pHead[j + y] - fGray) > threahold)
					{
						pHead[j + i * nRowlen] = fMAX;
					}
				}
			}
			break;
		}
	case 3:// 彩色图像
	case 4:
		{
			for (int i = 0; i < nHeight; ++i)
			{
				int y = i * nRowlen;
				for (int j = 0; j < nWidth; ++j)
				{
					int x = j * nChannel;
					float3 current(pHead[2 + x + y], pHead[1 + x + y], pHead[x + y]);
					if (Distance(current, background) > threahold)
					{
						pHead[x + y] = pHead[1 + x + y] = pHead[2 + x + y] = fMAX;
					}
				}
			}
			break;
		}
	default:
		break;
	}
	pBackground->MemcpyFloatToByte();
	return pBackground;
}


/** - 根据矩形区域提取感兴趣区域的图像 - */
CyImage* CyImage::ROI(CLogoRect rect) const
{
	if (rect == CLogoRect(0, 0, 0, 0))
		return MakeCopy();
	ImageTransform it(m_pFloatData, GetWidth(), GetHeight(), GetChannel());
	ImageTransform pDst = it.ImageRoi(rect);
	CyImage *pImage = NULL;
	try
	{
		pImage = new CyImage;
	}
	catch (const std::bad_alloc)
	{
		return pImage;
	}
	pImage->Create(pDst, rect.Width(), rect.Height(), rect.Width() * GetChannel());
	return pImage;
}


/** - 裁剪图像 - */
void CyImage::Cut(CLogoRect rect)
{
	/// 下述两种情况不需要裁剪
	if (rect == CLogoRect(0, 0, 0, 0))
		return;
	if (rect.left == 0 && rect.top == 0 && rect.right == GetWidth() && rect.bottom == GetHeight())
		return;
	/// 其他情况，获得感兴趣区域，并赋值给当前图像
	ImageTransform it(m_pFloatData, GetWidth(), GetHeight(), GetChannel());
	ImageTransform pDst = it.ImageRoi(rect);
	Create(pDst, rect.Width(), rect.Height(), rect.Width() * GetChannel());
}
