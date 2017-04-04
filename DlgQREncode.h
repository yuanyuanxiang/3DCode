/**
* @file DlgQREncode.h
* @brief CDlgQREncode 类的接口
*/

#pragma once

#include "EncodeParent.h"

/**
* @class CDlgQREncode
* @brief CDlgQREncode 对话框
*/
class CDlgQREncode : public CEncodeParent
{
public:
	// 设置图像
	virtual BOOL SetImage();
	// 刷新视图
	virtual void Repaint();
};