/**
* @file 3DCode.h 
* @brief 3DCode 应用程序的主头文件
* @author 袁沅祥
*/

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

class CyImage;

class CDlgQREncode;

class C3DCodeDoc;

class C3DCodeView;

/** @class C3DCodeApp
* @brief 有关此类的实现，请 @see 3DCode.cpp
*/
class C3DCodeApp : public CWinAppEx
{
public:
	C3DCodeApp();

	// 文档模板
	CMultiDocTemplate* m_pDocTemplate;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// 获取文档模板
	CMultiDocTemplate* GetMultiDocTemplate() { return m_pDocTemplate; }

	// 打开文档
	C3DCodeDoc* OpenDocument(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);

// 实现
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	// 消息响应函数
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnAppAbout();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

extern C3DCodeApp theApp;

// 获取活动文档的图像指针
extern CyImage* AfxGetImage();

// 打开新的文档
extern C3DCodeDoc* AfxOpenDocument(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);