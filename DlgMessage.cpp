// DlgMessage.cpp : 实现文件
//

#include "stdafx.h"
#include "3DCode.h"
#include "DlgMessage.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgMessage 对话框

IMPLEMENT_DYNAMIC(CDlgMessage, CDialogEx)

CDlgMessage::CDlgMessage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMessage::IDD, pParent)
	, m_fInput(0)
	, m_strInputName(_T(""))
{

}

CDlgMessage::~CDlgMessage()
{
}

void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_EditInput);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_fInput);
	DDX_Control(pDX, IDC_INPUT_NAME, m_StaticInputName);
	DDX_Text(pDX, IDC_INPUT_NAME, m_strInputName);
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogEx)
END_MESSAGE_MAP()


// CDlgMessage 消息处理程序


BOOL CDlgMessage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}