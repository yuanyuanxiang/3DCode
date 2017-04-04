#include "stdafx.h"
#include "FunctionsUsingMFC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/** 设置窗口显示整数 */
void SetWindowInt(CWnd* pWnd, int nInt)
{
	CString text;
	text.Format(_T("%d"), nInt);
	pWnd->SetWindowText(text);
}


/** 设置窗口显示小数 */
void SetWindowFloat(CWnd* pWnd, float val)
{
	CString text;
	text.Format(_T("%.3f"), val);
	pWnd->SetWindowText(text);
}


/** 取得窗口所显示整数 */
int GetWindowInt(CWnd* pWnd)
{
	CString text;
	pWnd->GetWindowText(text);
	return _ttoi(text);
}


/** 取得窗口所显示的浮点数 */
double GetWindowFloat(CWnd* pWnd)
{
	CString text;
	pWnd->GetWindowText(text);
	return _ttof(text);
}