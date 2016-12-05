#pragma once
#include "afxwin.h"


// CDlgMessage 对话框

class CDlgMessage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMessage)

public:
	CDlgMessage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMessage();

// 对话框数据
	enum { IDD = IDD_MESSAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditInput;
	float m_fInput;
	CStatic m_StaticInputName;
	CString m_strInputName;
	virtual BOOL OnInitDialog();
};
