#ifdef _AFX

#pragma once

/************************************************************************/
// 全局函数
// AfxGetActiveDoc 获取程序主文档
// AfxGetActiveView 获取程序活动视图
/************************************************************************/

// 获取活动文档的指针
extern CDocument* AfxGetActiveDoc();

// 获取活动视图的指针
extern CView* AfxGetActiveView();

#endif