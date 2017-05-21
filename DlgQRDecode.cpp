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

#include "DMDecoder.h"
#include "PDF417Decoder.h"
#include "AztecDecoder.h"

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
	m_pImage = NULL;

	// 编码颜色
	m_BackgroundColor = RGB(0, 0, 0);

	ImageInfo::Init("yuanyuanxiang");
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

	// 解码整幅图像
	ImageInfo pImage(pHead, nWidth, nHeight, nChannel, m_roi);
	BOOL success = DecodeWholeImage(DecodeSrcInfo(pImage, TRUE, TRUE), &qr, &inner);
	if (qr.m_pData == NULL)
	{
		// 尝试进行DM、PDF417、Aztec二维码解码
		DMDecoder dm;
		dm.Init(pHead, nWidth, nHeight, nChannel, m_roi);
		if (dm.Decode(m_BackgroundColor))
		{
			qr.m_pData = dm.GetData();
		}
		else
		{
			PDF417Decoder pdf417;
			pdf417.CopyOf(dm);
			if (pdf417.Decode(m_BackgroundColor))
			{
				qr.m_pData = pdf417.GetData();
			}
			else
			{
				AztecDecoder az;
				az.CopyOf(dm);
				if (az.Decode(m_BackgroundColor))
				{
					qr.m_pData = az.GetData();
				}
			}
		}
	}
	if (qr.m_pData == NULL)
	{
		GetDlgItem(IDC_EDITSOURCEDATA_PUBLIC)->SetWindowText(_T("*** Decode failed ***"));
		return FALSE;
	}
	int m_ncLength = 0;
	m_strPublicString = UTF8Convert2Unicode(qr.m_pData, m_ncLength);
	UpdateDecodeInfo();
	if (inner.m_pData != NULL)
	{
		m_strPrivateString = UTF8Convert2Unicode(inner.m_pData, m_ncLength);
		GetDlgItem(IDC_EDITSOURCEDATA_PRIVATE)->SetWindowText(m_strPrivateString);
		SetWindowInt(GetDlgItem(IDC_EDIT_INNER_ECLEVEL), inner.m_nEcLevel);
		SetWindowInt(GetDlgItem(IDC_EDIT_INNER_MASK), inner.m_nMaskingNo);
		UpdateecLevelTipText(inner.m_nEcLevel);
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
	switch (qr.m_nEcLevel)
	{
	case 0:ecLevel = _T("L(7%)"); break;
	case 1:ecLevel = _T("M(15%)"); break;
	case 2:ecLevel = _T("Q(25%)"); break;
	case 3:ecLevel = _T("H(30%)"); break;
	default:break;
	}
	GetDlgItem(IDC_EDIT_EC_LEVEL)->SetWindowText(ecLevel);
	SetWindowInt(GetDlgItem(IDC_EDIT_QR_VERSION), qr.m_nVersion);
	SetWindowInt(GetDlgItem(IDC_EDIT_MASK_VERSION), qr.m_nMaskingNo);
	SetWindowFloat(GetDlgItem(IDC_EDIT_MODULESIZE), qr.m_fModuleSize);
	GetDlgItem(IDC_EDITSOURCEDATA_PUBLIC)->SetWindowText(m_strPublicString);
}


BEGIN_MESSAGE_MAP(CDlgQRDecode, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DECODE, &CDlgQRDecode::OnBnClickedButtonDecode)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CDlgQRDecode::OnBnClickedButtonSaveImage)
	ON_BN_CLICKED(IDC_BN_BACKGROUND, &CDlgQRDecode::OnBnClickedBnBackground)
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
	m_ToolTip.AddTool(GetDlgItem(IDC_BN_BACKGROUND), _T("设置背景色(默认黑色)"));

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
	if (!m_pImage->IsNull())
	{
		CyImage *roi = m_pImage->ROI(m_roi);
		roi->Save(this);
		delete roi;
	}
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


void CDlgQRDecode::OnBnClickedButtonClear()
{
	Reset();
}