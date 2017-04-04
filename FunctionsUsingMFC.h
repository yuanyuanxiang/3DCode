#ifdef _AFX

#pragma once

/** 
* @file FunctionsUsingMFC.h
* @brief 用到了MFC的函数，只能在WIN32下面调用
* @date 2016/8/24 建立此文档
*/

// 设置窗口显示数字
void SetWindowInt(CWnd* pWnd, int nInt);

// 设置窗口显示浮点数
void SetWindowFloat(CWnd* pWnd, float val);

// 取得窗口所显示的数字
int GetWindowInt(CWnd* pWnd);

// 取得窗口所显示的浮点数
double GetWindowFloat(CWnd* pWnd);

#endif // _AFX