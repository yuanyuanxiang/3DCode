// DlgQRDecode.cpp : 实现文件
//

#include "stdafx.h"
#include "3DCode.h"
#include "DlgQRDecode.h"
#include "afxdialogex.h"

#include "3DCodeDoc.h"
#include "AfxGlobal.h"
#include "FunctionsUsingMFC.h"
#include "CodeTransform.h"

#include "DecodeFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgQRDecode 对话框

IMPLEMENT_DYNAMIC(CDlgQRDecode, CDialogEx)

CDlgQRDecode::CDlgQRDecode(CWnd* pParent) : CDialogEx(CDlgQRDecode::IDD, pParent)
	, m_bUseHybrid(TRUE)
	, m_bTryHarder(TRUE)
{
	m_fModuleSize = 4.f;
	m_nLevel = 0;
	m_nVersion = 1;
	m_nMaskingNo = 0;
	m_pImage = NULL;

	// 编码颜色
	m_BackgroundColor = RGB(0, 0, 0);
	m_ForegroundColor = RGB(255, 255, 255);
	m_QREncodeColor1 = RGB(255, 0, 0);
	m_QREncodeColor2 = RGB(0, 255, 0);
	m_ncLength = 0;
}

CDlgQRDecode::~CDlgQRDecode()
{
}

void CDlgQRDecode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_HYBRID, m_bUseHybrid);
	DDX_Check(pDX, IDC_CHECK_TRY_HARDER, m_bTryHarder);
	DDX_Control(pDX, IDC_CHECK_USE_HYBRID, m_checkUseHybrid);
	DDX_Control(pDX, IDC_CHECK_TRY_HARDER, m_checkTryHarder);
}


void CDlgQRDecode::SetImage()
{
	m_pImage = AfxGetImage();
	C3DCodeDoc* pDoc = (C3DCodeDoc*)AfxGetActiveDoc();
	if (pDoc != NULL)
	{
		m_pImage = pDoc->GetImage();
		m_roi = pDoc->GetROI();
	}
	else
	{
		m_pImage = NULL;
		m_roi = RoiRect(0, 0, 0, 0);
	}
}

/// 解码当前图像
BOOL CDlgQRDecode::Decode()
{
	SetImage();
	if (m_pImage == NULL || m_pImage->IsNull())
		return FALSE;

	// 更新或者获取控件关联变量
	m_strPublicString.Empty();
	m_strPrivateString.Empty();
	m_bUseHybrid = m_checkUseHybrid.GetCheck();
	m_bTryHarder = m_checkTryHarder.GetCheck();	
	// 获取解码参数
	int nWidth = 0, nHeight = 0, nRowlen = 0;
	// m_pImage->ChangeBPP(32);
	m_pImage->GetInfomation(nWidth, nHeight, nRowlen);
	int nChannel = m_pImage->GetChannel();
	int nFloatRowlen = m_pImage->GetFloatDataRowlen();
	BYTE *pHead = m_pImage->GetHeadAddress();

	int strLen = 0, nInnerecLevel = 0, nInnerMask = 0;
	char* pDst[2] = { 0 };
	// 解码整幅图像
	BOOL success = DecodeWholeImage(pDst, pHead, nWidth, nHeight, nChannel,
		m_fPos, m_fModuleSize, m_nLevel, m_nVersion, m_nMaskingNo, 
		m_bUseHybrid, m_bTryHarder, 
		m_ForegroundColor, m_BackgroundColor, 
		m_QREncodeColor1, m_QREncodeColor2, 
		strLen, nInnerecLevel, nInnerMask, 
		m_roi);
	if (pDst[0] == NULL)
	{
		GetDlgItem(IDC_EDITSOURCEDATA_PUBLIC)->SetWindowText(_T("*** Decode failed ***"));
		return FALSE;
	}
	m_strPublicString = UTF8Convert2Unicode(pDst[0], m_ncLength);
	SAFE_DELETE(pDst[0]);
	UpdateDecodeInfo();
	if (pDst[1] != NULL)
	{
		m_strPrivateString = UTF8Convert2Unicode(pDst[1], m_ncLength);
		SAFE_DELETE(pDst[1]);
		GetDlgItem(IDC_EDITSOURCEDATA_PRIVATE)->SetWindowText(m_strPrivateString);
		SetWindowInt(GetDlgItem(IDC_EDIT_INNER_ECLEVEL), nInnerecLevel);
		SetWindowInt(GetDlgItem(IDC_EDIT_INNER_MASK), nInnerMask);
		UpdateecLevelTipText(nInnerecLevel);
		return TRUE;
	}
	else
	{
		GetDlgItem(IDC_EDITSOURCEDATA_PRIVATE)->SetWindowText(_T("*** Decode failed ***"));
	}
	return TRUE;
}


void CDlgQRDecode::UpdateDecodeInfo()
{
	// 更新控件状态
	CString ecLevel;
	switch (m_nLevel)
	{
	case 0:ecLevel = _T("L(7%)"); break;
	case 1:ecLevel = _T("M(15%)"); break;
	case 2:ecLevel = _T("Q(25%)"); break;
	case 3:ecLevel = _T("H(30%)"); break;
	default:break;
	}
	GetDlgItem(IDC_EDIT_EC_LEVEL)->SetWindowText(ecLevel);
	SetWindowInt(GetDlgItem(IDC_EDIT_QR_VERSION), m_nVersion);
	SetWindowInt(GetDlgItem(IDC_EDIT_MASK_VERSION), m_nMaskingNo);
	SetWindowFloat(GetDlgItem(IDC_EDIT_MODULESIZE), m_fModuleSize);
	GetDlgItem(IDC_EDITSOURCEDATA_PUBLIC)->SetWindowText(m_strPublicString);
}


BEGIN_MESSAGE_MAP(CDlgQRDecode, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DECODE, &CDlgQRDecode::OnBnClickedButtonDecode)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CDlgQRDecode::OnBnClickedButtonSaveImage)
	ON_BN_CLICKED(IDC_BN_FOREGROUND, &CDlgQRDecode::OnBnClickedBnForeground)
	ON_BN_CLICKED(IDC_BN_BACKGROUND, &CDlgQRDecode::OnBnClickedBnBackground)
	ON_BN_CLICKED(IDC_BN_ENCODE_COLOR1, &CDlgQRDecode::OnBnClickedBnEncodeColor1)
	ON_BN_CLICKED(IDC_BN_ENCODE_COLOR2, &CDlgQRDecode::OnBnClickedBnEncodeColor2)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDlgQRDecode::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CDlgQRDecode 消息处理程序


void CDlgQRDecode::OnBnClickedButtonDecode()
{
	BOOL result = Decode();
	TRACE(" * 解码结果: %d\n", result);
}

BOOL CDlgQRDecode::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 工具提示
	EnableToolTips(true);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(true);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 200); //延迟200ms后显示
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 3000);//显示持续3000 ms

	m_ToolTip.AddTool(GetDlgItem(IDC_CHECK_USE_HYBRID), _T("启用混合二值化"));
	m_ToolTip.AddTool(GetDlgItem(IDC_CHECK_TRY_HARDER), _T("启用强力搜索功能"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_EC_LEVEL), _T("QR码纠错能力的大小"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_MODULESIZE), _T("模块大小通常大于1"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_QR_VERSION), _T("QR码版本从1到40"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_MASK_VERSION), _T("QR码共有8种掩码"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_DECODE), _T("对当前图像解码"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_SAVE_IMAGE), _T("保存这个二维码"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_CLEAR), _T("清除解码结果"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_INNER_ECLEVEL), _T("密文的纠错等级"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_INNER_MASK), _T("密文的掩码版本"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BN_FOREGROUND), _T("设置前景色(默认白色)"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BN_BACKGROUND), _T("设置背景色(默认黑色)"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BN_ENCODE_COLOR1), _T("设置彩色1(默认红色)"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BN_ENCODE_COLOR2), _T("设置彩色2(默认绿色)"));

	return TRUE;
}


void CDlgQRDecode::OnOK()
{
	TRACE(" * OnOK is called.\n");
	return;
}


BOOL CDlgQRDecode::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgQRDecode::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}


/// 使得绘制图像不闪烁
BOOL CDlgQRDecode::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}


void CDlgQRDecode::OnBnClickedButtonSaveImage()
{
}


void CDlgQRDecode::UpdateecLevelTipText(int nEcLevel)
{
	switch (nEcLevel)
	{
	case 1:m_ToolTip.UpdateTipText(_T("1/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	case 2:m_ToolTip.UpdateTipText(_T("2/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	case 3:m_ToolTip.UpdateTipText(_T("3/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	case 4:m_ToolTip.UpdateTipText(_T("4/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	case 5:m_ToolTip.UpdateTipText(_T("5/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	case 6:m_ToolTip.UpdateTipText(_T("6/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	case 7:m_ToolTip.UpdateTipText(_T("7/15"), GetDlgItem(IDC_EDIT_INNER_ECLEVEL)); break;
	default:break;
	}
}


void CDlgQRDecode::Reset()
{
	m_pImage = NULL;
	m_strPublicString.Empty();
	m_strPrivateString.Empty();
	GetDlgItem(IDC_EDIT_EC_LEVEL)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_MODULESIZE)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_QR_VERSION)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_MASK_VERSION)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_INNER_ECLEVEL)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_INNER_MASK)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDITSOURCEDATA_PUBLIC)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDITSOURCEDATA_PRIVATE)->SetWindowText(_T(""));
}


void CDlgQRDecode::OnBnClickedBnForeground()
{
	CColorDialog m_setClrDlg;
	// CC_RGBINIT可以让上次选择的颜色作为初始颜色显示出来
	m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	m_setClrDlg.m_cc.rgbResult = m_ForegroundColor;
	if (IDOK == m_setClrDlg.DoModal())
	{
		if (m_ForegroundColor == m_setClrDlg.m_cc.rgbResult)
			return;
		// 保存用户选择的颜色
		m_ForegroundColor = m_setClrDlg.m_cc.rgbResult;
	}
}


void CDlgQRDecode::OnBnClickedBnBackground()
{
	CColorDialog m_setClrDlg;
	m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	m_setClrDlg.m_cc.rgbResult = m_BackgroundColor;
	if (IDOK == m_setClrDlg.DoModal())
	{
		if (m_BackgroundColor == m_setClrDlg.m_cc.rgbResult)
			return;
		m_BackgroundColor = m_setClrDlg.m_cc.rgbResult;
	}
}


void CDlgQRDecode::OnBnClickedBnEncodeColor1()
{
	CColorDialog m_setClrDlg;
	m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	m_setClrDlg.m_cc.rgbResult = m_QREncodeColor1;
	if (IDOK == m_setClrDlg.DoModal())
	{
		if (m_QREncodeColor1 == m_setClrDlg.m_cc.rgbResult)
			return;
		m_QREncodeColor1 = m_setClrDlg.m_cc.rgbResult;
	}
}


void CDlgQRDecode::OnBnClickedBnEncodeColor2()
{
	CColorDialog m_setClrDlg;
	m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	m_setClrDlg.m_cc.rgbResult = m_QREncodeColor2;
	if (IDOK == m_setClrDlg.DoModal())
	{
		if (m_QREncodeColor2 == m_setClrDlg.m_cc.rgbResult)
			return;
		m_QREncodeColor2 = m_setClrDlg.m_cc.rgbResult;
	}
}


void CDlgQRDecode::OnBnClickedButtonClear()
{
	Reset();
}