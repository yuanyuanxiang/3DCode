#pragma once


// C3DCodePane
// 这是一个包含对话框控件的CDockablePane，对话框上面有滚动视图
// 修改此文件将会影响3dcode和InvisibleCode两个项目
// 2015年12月11日特此说明
// 

class CViewParent;
class CPaneScrollView;

// 滚动视图的ID
#define ID_ENCODE_VIEW 9999

class C3DCodePane : public CDockablePane
{
	DECLARE_DYNAMIC(C3DCodePane)
public:
	CViewParent*		m_pViewParent;
	CPaneScrollView*	m_pEncodeView;

public:
	C3DCodePane();
	virtual ~C3DCodePane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


/************************************************************************/
/*                          CViewParent 框架                            */
/************************************************************************/


class CViewParent : public CFrameWnd
{
	DECLARE_DYNCREATE(CViewParent)
public:
	CViewParent();
	virtual ~CViewParent();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


/************************************************************************/
/*                     CPaneScrollView 视图                             */
/************************************************************************/


//class CDlgQREncode;

class CPaneScrollView : public CScrollView
{
	DECLARE_DYNCREATE(CPaneScrollView)

public:
	CSize m_szChildSize;
	CWnd* m_pChildWnd;
	void AddChildWnd(CWnd* pChild, int nWidth = 100, int nHeight = 100);

	CPaneScrollView();
	virtual ~CPaneScrollView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};