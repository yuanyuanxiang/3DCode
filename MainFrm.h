/**
* @file MainFrm.h
* @brief CMainFrame 类的接口
*/

#pragma once
#include "3DCodePane.h"
#include "DlgQREncode.h"
#include "DlgQRDecode.h"

/**
* @class CMainFrame
* @brief 主框架
*/
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW 
		| FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar			m_wndMenuBar;		/**< 菜单栏 */
	CMFCToolBar			m_wndToolBar;		/**< 工具栏 */
	CMFCStatusBar		m_wndStatusBar;		/**< 状态栏 */
	CMFCToolBarImages	m_UserImages;		/**< 状态栏图像 */
	C3DCodePane			m_paneQREncode;		/**< 编码面板 */
	C3DCodePane			m_paneQRDecode;		/**< 解码面板 */

public:
	CDlgQREncode*		m_pQREncodeDlg;		/**< 编码对话框 */
	CDlgQRDecode*		m_pQRDecodeDlg;		/**< 解码对话框 */

	/// 获得编码对话框指针
	CDlgQREncode* GetEncodeDlg() { return m_pQREncodeDlg; }
	/// 获得解码对话框指针
	CDlgQRDecode* GetDecodeDlg() { return m_pQRDecodeDlg; }
	/// 获取状态栏
	CMFCStatusBar& GetStatusBar() { return m_wndStatusBar; }

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};