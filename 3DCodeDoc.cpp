
// 3DCodeDoc.cpp : C3DCodeDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "3DCode.h"
#endif

#include "3DCodeDoc.h"
#include "3DCodeView.h"

#include <propkey.h>
#include <fstream>
#include "FileIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// C3DCodeDoc

IMPLEMENT_DYNCREATE(C3DCodeDoc, CDocument)

BEGIN_MESSAGE_MAP(C3DCodeDoc, CDocument)
END_MESSAGE_MAP()


// C3DCodeDoc 构造/析构

C3DCodeDoc::C3DCodeDoc()
{
	m_pImage = new CyImage;
	m_bImageLocked = FALSE;
}

C3DCodeDoc::~C3DCodeDoc()
{
	SAFE_DELETE(m_pImage);
}

BOOL C3DCodeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

// C3DCodeDoc 序列化

void C3DCodeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void C3DCodeDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void C3DCodeDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void C3DCodeDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// C3DCodeDoc 诊断

#ifdef _DEBUG
void C3DCodeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void C3DCodeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// C3DCodeDoc 命令


BOOL C3DCodeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	m_bImageLocked = TRUE;

	return SUCCEEDED(m_pImage->Load(lpszPathName));
}

BOOL C3DCodeDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return 	SUCCEEDED(m_pImage->Save(lpszPathName));
}


CyImage* C3DCodeDoc::GetImage()
{
	return m_pImage;
}


// 获取感兴趣区域
RoiRect& C3DCodeDoc::GetROI()
{
	return m_roi;
}


BOOL C3DCodeDoc::ImageIsNull()
{
	return m_pImage->IsNull();
}

// 因为编码会销毁现有图像，故将外部图像锁定
BOOL C3DCodeDoc::ImageIsLocked()
{
	return m_bImageLocked;
}

// 返回CSize = (nWidth, nHeight).
const CSize C3DCodeDoc::GetImageSize()
{
	if (m_pImage->IsNull())
		return CSize(0, 0);
	return CSize(m_pImage->GetWidth(), m_pImage->GetHeight());
}

// 获取文档对应的C3DCodeView活动视图
C3DCodeView* C3DCodeDoc::GetView()
{
	CView* pView = NULL;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(C3DCodeView)))
			return (C3DCodeView*)pView;
		else
			continue;
	}
	if (pView && pView->IsKindOf(RUNTIME_CLASS(C3DCodeView)))
		return (C3DCodeView*)pView;
	else
	{
		TRACE(" * 不能定位视图。\n");
		return NULL;
	}
}