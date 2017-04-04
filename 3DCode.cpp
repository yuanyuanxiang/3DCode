
// 3DCode.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "3DCode.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "3DCodeDoc.h"
#include "3DCodeView.h"
#include "AfxGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// C3DCodeApp

BEGIN_MESSAGE_MAP(C3DCodeApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &C3DCodeApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &C3DCodeApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &C3DCodeApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


/// C3DCodeApp 构造
C3DCodeApp::C3DCodeApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("3DCode.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

/// 唯一的一个 C3DCodeApp 对象
C3DCodeApp theApp;


/// C3DCodeApp 初始化
BOOL C3DCodeApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Capital Normal University"));
	LoadStdProfileSettings(12);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	// CMultiDocTemplate* pDocTemplate;
	m_pDocTemplate = new CMultiDocTemplate(IDR_3DCodeTYPE,
		RUNTIME_CLASS(C3DCodeDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(C3DCodeView));
	if (!m_pDocTemplate)
		return FALSE;
	AddDocTemplate(m_pDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	// cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//m_pMainWnd->OpenClipboard();
	//EmptyClipboard();
	//CloseClipboard();

	return TRUE;
}

/// 退出
int C3DCodeApp::ExitInstance()
{
	//TODO:  处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// C3DCodeApp 消息处理程序


/** @class CAboutDlg
* @brief 用于应用程序“关于”菜单项的 CAboutDlg 对话框
*/
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	/// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

/// 标准构造函数
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

/// 对话框数据交互
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

/// 用于运行对话框的应用程序命令
void C3DCodeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/// C3DCodeApp 自定义加载/保存方法
void C3DCodeApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

/// 加载自定义状态
void C3DCodeApp::LoadCustomState()
{
}

/// 保存自定义状态
void C3DCodeApp::SaveCustomState()
{
}

/**
* @brief 打开文档
* @param[in] lpszPathName 文档名称
* @param[in] bMakeVisible 是否可见
* @return 文档指针
*/
C3DCodeDoc* C3DCodeApp::OpenDocument(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
	CDocument* pDoc = m_pDocTemplate->OpenDocumentFile(lpszPathName, bMakeVisible);
	return (C3DCodeDoc*)pDoc;
}

// C3DCodeApp 消息处理程序

/// 新建文件时的响应函数
void C3DCodeApp::OnFileNew()
{
	OpenDocument(NULL, TRUE);
}

/// 打开文件时的响应函数
void C3DCodeApp::OnFileOpen()
{
	CWinAppEx::OnFileOpen();
}

/**
* @brief 打开文档
* @param[in] lpszFileName 文档名称
* @return 文档指针
*/
CDocument* C3DCodeApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	C3DCodeDoc *pDoc = (C3DCodeDoc*)AfxGetActiveDoc();
	CyImage *pImage = AfxGetImage();
	// 已经存在一个空白文档，则加载图像
	if (pDoc && pImage->IsNull())
	{
		if (pDoc->OnOpenDocument(lpszFileName))
		{
			pDoc->SetPathName(lpszFileName);
			pDoc->GetView()->SetPaintRect(pDoc->GetImageSize());
			return pDoc;
		}
		return NULL;
	}

	return CWinAppEx::OpenDocumentFile(lpszFileName);
}


/// 获取活动文档的图像指针
CyImage* AfxGetImage()
{
	C3DCodeDoc* pDoc = (C3DCodeDoc*)AfxGetActiveDoc();

	return pDoc ? pDoc->GetImage() : NULL;
}

/**
* @brief 打开新的文档
* @param[in] lpszPathName 文档名称
* @param[in] bMakeVisible 是否可见
* @return 文档指针
* @remark 该函数是全局的
*/
extern C3DCodeDoc* AfxOpenDocument(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
	C3DCodeApp* pApp = (C3DCodeApp*)AfxGetApp();
	C3DCodeDoc* pDoc = pApp->OpenDocument(lpszPathName, bMakeVisible);
	return pDoc;
}