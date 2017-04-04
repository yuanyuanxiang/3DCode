/** 
* @file 3DCodeView.h
* @brief C3DCodeView 类的接口
*/

#pragma once

/**
* @class C3DCodeView
* @brief 视图
*/
class C3DCodeView : public CScrollView
{
protected: // 仅从序列化创建
	C3DCodeView();
	DECLARE_DYNCREATE(C3DCodeView)

// 特性
public:
	C3DCodeDoc* GetDocument() const;

// 操作
private:
	CRect							m_PaintRect;		/**< 绘图矩形 */
	RoiRect							m_RoiRect;			/**< 感兴趣区域*/
	float							m_fZoomRate;		/**< 放大比例 */
	BOOL							m_bLeftButtonDown;	/**< 左键按下 */

public:
	void SetPaintRect(const CSize sz);					// 设置绘图矩形大小
	void SetPaintRect(int nWidth, int nHeight);			// 设置绘图矩形宽高
	void CopyImage(CyImage* pImage);					// 拷贝图像到内存
	void PasteImage();									// 黏贴图像到文档
	void Invalidate(BOOL bErase = TRUE);				// 重绘图像

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~C3DCodeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewZoomIn();
	afx_msg void OnUpdateViewZoomIn(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomOut();
	afx_msg void OnUpdateViewZoomOut(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomDefault();
	afx_msg void OnUpdateViewZoomDefault(CCmdUI *pCmdUI);
	afx_msg void OnViewFlipH();
	afx_msg void OnUpdateViewFlipH(CCmdUI *pCmdUI);
	afx_msg void OnViewFlipV();
	afx_msg void OnUpdateViewFlipV(CCmdUI *pCmdUI);
	afx_msg void OnViewTranspose();
	afx_msg void OnUpdateViewTranspose(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditBpp();
	afx_msg void OnUpdateEditBpp(CCmdUI *pCmdUI);
	afx_msg void OnEditRotate();
	afx_msg void OnUpdateEditRotate(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // 3DCodeView.cpp 中的调试版本
inline C3DCodeDoc* C3DCodeView::GetDocument() const
   { return reinterpret_cast<C3DCodeDoc*>(m_pDocument); }
#endif