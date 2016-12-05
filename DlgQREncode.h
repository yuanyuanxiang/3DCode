#pragma once

#include "EncodeParent.h"

// CDlgQREncode 对话框

class CDlgQREncode : public CEncodeParent
{
public:
	// 设置图像
	virtual BOOL SetImage();
	// 刷新视图
	virtual void Repaint();
};