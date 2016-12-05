
// 3DCodeDoc.h : C3DCodeDoc 类的接口
//


#pragma once
#include "CyImage.h"

class C3DCodeView;

class C3DCodeDoc : public CDocument
{
protected: // 仅从序列化创建
	C3DCodeDoc();
	DECLARE_DYNCREATE(C3DCodeDoc)

// 特性
private:
	// 编解码图像
	CyImage* m_pImage;

	/// 感兴趣区域
	RoiRect m_roi;

	// 锁定图像
	BOOL m_bImageLocked;

	// 编解码数据
	CString m_strData;

public:
	// 获得图像指针
	CyImage* GetImage();

	// 获取感兴趣区域
	RoiRect& GetROI();

	// 判断图像为空
	BOOL ImageIsNull();

	// 判断图像是否锁定
	BOOL ImageIsLocked();

	// 获取图像大小
	const CSize GetImageSize();

	// 获取视图
	C3DCodeView* GetView();

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~C3DCodeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
