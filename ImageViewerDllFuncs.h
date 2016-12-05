#include "stdafx.h"

#ifdef _AFX

/****************************************************************
	一、文档说明
		动态链接库 ImageViewer.dll 接口函数声明

	二、制 作 人
		首都师范大学 袁沅祥 2015年4月22日

	三、使用方法

		1、加载链接库:	LoadImageViewerDll(HINSTANCE&);
		2、寻找函数:	FindImageViewerFun(HINSTANCE&, TYPE*);
		3、调用函数:	ImageViewerProc(TYPE*, int, int, int);
		4、释放链接库:	FreeLibrary(HINSTANCE).
	四、简便方法
		直接调用:		PopImageViewerDlg(T*, int, int, int);
*****************************************************************/

bool LoadImageViewerDll(HINSTANCE &hDll)
{
	hDll = LoadLibrary(_T("ImageViewer.dll"));
	if (hDll == NULL)
		return false;
	return true;
}

// 1 CImage*
typedef void (*lpImageViewer)(CImage*);
lpImageViewer ImageViewer;
bool InitImageViewerFun(HINSTANCE &hDll)
{
	ImageViewer = (lpImageViewer)GetProcAddress(hDll, "ImageViewer");
	if (ImageViewer == NULL)
		return false;
	return true;
}

// 2 BYTE*
typedef void (*lpBYTEViewer)(BYTE*, int, int, int);
lpBYTEViewer BYTEViewer;
bool FindImageViewerFun(HINSTANCE &hDll, BYTE*)
{
	BYTEViewer = (lpBYTEViewer)GetProcAddress(hDll, "BYTEViewer");
	if (BYTEViewer == NULL)
		return false;
	return true;
}

// char*
typedef void (*lpCharViewer)(char*, int, int, int);
lpCharViewer CharViewer;
bool FindImageViewerFun(HINSTANCE &hDll, char*)
{
	CharViewer = (lpCharViewer)GetProcAddress(hDll, "BYTEViewer");
	if (CharViewer == NULL)
		return false;
	return true;
}

// 3 int*
typedef void (*lpIntViewer)(int*, int, int, int);
lpIntViewer IntViewer;
bool FindImageViewerFun(HINSTANCE &hDll, int*)
{
	IntViewer = (lpIntViewer)GetProcAddress(hDll, "IntViewer");
	if (IntViewer == NULL)
		return false;
	return true;
}

// 4 float*
typedef void (*lpFloatViewer)(float*, int, int, int);
lpFloatViewer FloatViewer;
bool FindImageViewerFun(HINSTANCE &hDll, float*)
{
	FloatViewer = (lpFloatViewer)GetProcAddress(hDll, "FloatViewer");
	if (FloatViewer == NULL)
		return false;
	return true;
}

// 5 double*
typedef void (*lpDoubleViewer)(double*, int, int, int);
lpDoubleViewer DoubleViewer;
bool FindImageViewerFun(HINSTANCE &hDll, double*)
{
	DoubleViewer = (lpDoubleViewer)GetProcAddress(hDll, "DoubleViewer");
	if (DoubleViewer == NULL)
		return false;
	return true;
}

// 6 CImage*
typedef void (*lpCImageViewer)(CImage*, int, int, int);
lpCImageViewer CImageViewer;
bool FindImageViewerFun(HINSTANCE &hDll, CImage*)
{
	CImageViewer = (lpCImageViewer)GetProcAddress(hDll, "CImageViewer");
	if (CImageViewer == NULL)
		return false;
	return true;
}

// 为了方便使用而进行重载：ptr - 被显示的数据, width - 宽度, height - 高度, rowlen - 每行字节数。
void ImageViewerProc(BYTE* ptr, int width, int height, int rowlen)	{	BYTEViewer(ptr, width, height, rowlen);}
void ImageViewerProc(char* ptr, int width, int height, int rowlen)	{	CharViewer(ptr, width, height, rowlen);}
void ImageViewerProc(int* ptr, int width, int height, int rowlen)	{	IntViewer(ptr, width, height, rowlen);}
void ImageViewerProc(float* ptr, int width, int height, int rowlen)	{	FloatViewer(ptr, width, height, rowlen);}
void ImageViewerProc(double* ptr, int width, int height, int rowlen){	DoubleViewer(ptr, width, height, rowlen);}
void ImageViewerProc(CImage* ptr, int width, int height, int rowlen){	CImageViewer(ptr, width, height, rowlen);}

// 弹出图像浏览对话框：ptr - 被显示的数据, width - 宽度, height - 高度, rowlen - 每行字节数。
template <typename Type> void PopImageViewerDlg(Type* ptr, int width, int height, int rowlen)
{
	HINSTANCE hDll = NULL;
	if (LoadImageViewerDll(hDll) == false)
	{
		AfxMessageBox(_T("\"ImageViewer.dll\"加载没有成功！"));
		return;
	}
	if (FindImageViewerFun(hDll, ptr) == false)
	{
		AfxMessageBox(_T("\"ImageViewer.dll\"函数寻找失败！"));
		FreeLibrary(hDll);
		return;
	}
	else
	{
		ImageViewerProc(ptr, width, height, rowlen);
	}
	FreeLibrary(hDll);
}

#endif