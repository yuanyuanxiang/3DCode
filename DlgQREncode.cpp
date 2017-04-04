// DlgQREncode.cpp : 实现文件
//

#include "stdafx.h"
#include "3DCode.h"
#include "DlgQREncode.h"
#include "AfxGlobal.h"
#include "3DCodeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgQREncode 对话框

/// 获取当前活动文档的图像指针，用于生成二维码
BOOL CDlgQREncode::SetImage()
{
	m_pImage = AfxGetImage();
	return TRUE;
}


/// 重新刷新视图
void CDlgQREncode::Repaint()
{
	/// @remark 2016/9/15 将Repaint设为虚函数，以实现子类的多态特性
	C3DCodeView* pView = (C3DCodeView*)AfxGetActiveView();
	pView->Invalidate(TRUE);
}