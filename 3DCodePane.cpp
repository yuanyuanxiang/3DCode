// QREncodePane.cpp : 实现文件
//

#include "stdafx.h"
//#include "3DCode.h"
#include "3DCodePane.h"
//#include "DlgQREncode.h"

// 注释掉无关头文件，因为被多个项目使用

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// C3DCodePane

IMPLEMENT_DYNAMIC(C3DCodePane, CDockablePane)

C3DCodePane::C3DCodePane()
{
	m_pViewParent = NULL;
	m_pEncodeView = NULL;
}

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

BOOL C3DCodePane::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


/************************************************************************/
/*                            CViewParent                               */
/************************************************************************/


IMPLEMENT_DYNCREATE(CViewParent, CFrameWnd)

CViewParent::CViewParent()
{

}

CViewParent::~CViewParent()
{
	TRACE(" * Pane的父框架被析构。\n");
}


BEGIN_MESSAGE_MAP(CViewParent, CFrameWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CViewParent 消息处理程序


BOOL CViewParent::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


/************************************************************************/
/*                         CPaneScrollView                              */
/************************************************************************/


IMPLEMENT_DYNCREATE(CPaneScrollView, CScrollView)

CPaneScrollView::CPaneScrollView()
{
	m_pChildWnd = NULL;
	m_szChildSize = CSize(100, 100);
}

CPaneScrollView::~CPaneScrollView()
{
	TRACE(" * Pane滚动视图被析构。\n");
}

BEGIN_MESSAGE_MAP(CPaneScrollView, CScrollView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CPaneScrollView 绘图

void CPaneScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, m_szChildSize);
}

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

void CPaneScrollView::AddChildWnd(CWnd* pChild, int nWidth, int nHeight)
{
	if(pChild != NULL && pChild->GetSafeHwnd())
	{
		m_pChildWnd = pChild;
		m_pChildWnd->SetParent(this);
	}

	m_szChildSize = CSize(nWidth, nHeight);
	SetScrollSizes(MM_TEXT, m_szChildSize);
}

BOOL CPaneScrollView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}