#include "stdafx.h"
#include "AfxGlobal.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 获取活动文档
CDocument* AfxGetActiveDoc()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CChildFrame* pChildFrame = (CChildFrame*)pMainFrame->GetActiveFrame();
	CDocument* pDoc = pChildFrame->GetActiveDocument();
	return pDoc;
}

// 获取活动视图
CView* AfxGetActiveView()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CChildFrame* pChildFrame = (CChildFrame*)pMainFrame->GetActiveFrame();
	CView* pView = pChildFrame->GetActiveView();
	return pView;
}