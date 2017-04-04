// QREncodePane.cpp : 实现文件
//

#include "stdafx.h"
#include "3DCodePane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// C3DCodePane

IMPLEMENT_DYNAMIC(C3DCodePane, CDockablePane)

/// 构造：令成员变量为空
C3DCodePane::C3DCodePane()
{
	m_pViewParent = NULL;
	m_pEncodeView = NULL;
}

/// 默认的析构函数
C3DCodePane::~C3DCodePane()
{
	// 无需delete两个指针变量
}


BEGIN_MESSAGE_MAP(C3DCodePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// C3DCodePane 消息处理程序

/** 
* @brief 创建时调用此函数
* @details 创建视图的父框架，创建编码视图
*/
int C3DCodePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pViewParent = new CViewParent();
	m_pViewParent->Create(NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(), this);
	CRuntimeClass *pViewClass = RUNTIME_CLASS(CPaneScrollView);
	m_pEncodeView = (CPaneScrollView*)pViewClass->CreateObject();
	m_pEncodeView->Create(NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(), m_pViewParent, ID_ENCODE_VIEW, NULL);

	return 0;
}

/** 
* @brief 窗口大小改变时的响应函数
* @details 编码视图及视图父框架将跟着变
*/
void C3DCodePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if (m_pViewParent->GetSafeHwnd())
	{
		m_pViewParent->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
	}
	if (m_pEncodeView->GetSafeHwnd())
	{
		m_pEncodeView->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
	}
}

/// 背景重绘时的响应函数
BOOL C3DCodePane::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


/************************************************************************/
/*                            CViewParent                               */
/************************************************************************/


IMPLEMENT_DYNCREATE(CViewParent, CFrameWnd)

/// 默认构造函数
CViewParent::CViewParent()
{

}

/// 默认析构函数
CViewParent::~CViewParent()
{
	TRACE(" * Pane的父框架被析构。\n");
}


BEGIN_MESSAGE_MAP(CViewParent, CFrameWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CViewParent 消息处理程序

/// 背景重绘时的响应函数
BOOL CViewParent::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


/************************************************************************/
/*                         CPaneScrollView                              */
/************************************************************************/


IMPLEMENT_DYNCREATE(CPaneScrollView, CScrollView)

/// 面板滚动视图的构造函数
CPaneScrollView::CPaneScrollView()
{
	m_pChildWnd = NULL;
	m_szChildSize = CSize(100, 100);
}

/// 默认析构函数
CPaneScrollView::~CPaneScrollView()
{
	TRACE(" * Pane滚动视图被析构。\n");
}

BEGIN_MESSAGE_MAP(CPaneScrollView, CScrollView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CPaneScrollView 绘图

/// 视图初始化时调用的函数
void CPaneScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, m_szChildSize);
}

/// 视图绘制函数
void CPaneScrollView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

// CPaneScrollView 诊断

#ifdef _DEBUG
void CPaneScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CPaneScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG

/**
* @brief 为面板滚动视图添加一个子窗口
* @param[in] * pChild 子窗口指针
* @param[in] nWidth 窗口宽度
* @param[in] nHeight 窗口高度
*/
void CPaneScrollView::AddChildWnd(CWnd* pChild, int nWidth, int nHeight)
{
	if(pChild && pChild->GetSafeHwnd())
	{
		m_pChildWnd = pChild;
		m_pChildWnd->SetParent(this);
	}

	m_szChildSize = CSize(nWidth, nHeight);
	SetScrollSizes(MM_TEXT, m_szChildSize);
}

/// 背景重绘时的响应函数
BOOL CPaneScrollView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}