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

BOOL CDlgQREncode::SetImage()
{
	m_pImage = AfxGetImage();
	return TRUE;
}


// 刷新视图
void CDlgQREncode::Repaint()
{
	// 2016/9/15 将Repaint设为虚函数，以实现子类的多态特性
	C3DCodeView* pView = (C3DCodeView*)AfxGetActiveView();
	pView->Invalidate(TRUE);
}