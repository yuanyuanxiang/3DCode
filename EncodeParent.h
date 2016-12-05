#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"

#include "CyImage.h"
#include "QR_Encode.h"
#include <vector>
using namespace std;

// CEncodeParent 对话框

#define MIXED_RATE				0.5 //混合比率

#define BOUNDING_MODE_REPLACED	0	//替代绑定

#define BOUNDING_MODE_MIXED		1	//混合绑定

#define PIXEL_MAX_SIZE			32	//像素最大大小

class CEncodeParent : public CDialogEx
{
	DECLARE_DYNAMIC(CEncodeParent)

public:
	CEncodeParent(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEncodeParent();

	CyImage*						m_pImage;				//二维码
	CyImage*						m_pLogo;				//标识
	// 编码颜色
	COLORREF						m_BackgroundColor;		//背景色
	COLORREF						m_ForegroundColor;		//前景色
	COLORREF						m_QREncodeColor1;		//码元一
	COLORREF						m_QREncodeColor2;		//码元二
	// 符号/像素 大小
	int								m_nSymbleSize;			//符号尺寸
	int								m_nPixelSize;			//像素大小
	int								m_nEnableMixed;			//开启混合绑定
	BOOL							m_bHasALogo;			//拥有LOGO
	CLogoRect						m_LogoRect;				//logo区域矩形
	CLogoRect						m_realLogoRect;			//真实的logo矩形
	vector<CPixelPoint>				m_ForegroundElemsIndex;	//前景色的索引
	int								m_nInnerEcLevel;		//RS纠错能力
	int								m_nInnerMaskNo;			//彩色掩码版本
	int								m_nInnerVersion;		//彩色编码版本

public:
	// 虚方法
	virtual BOOL SetImage();								// 设置图像
	virtual BOOL Encode();									// 编码数据
	virtual BOOL EncodePrivateData(CQR_Encode* pQR_Encode);	// 编码密文
	virtual void InitInnerEncodingParameters();				// 初始化内层编码参数
	virtual void UpdateecLevelTipText();					// 更新滑动条信息提示
	afx_msg virtual void OnCbnSelchangeCombolevel2();		// 改变纠错等级时调用
	virtual void Addlogo();									// 添加标识
															// 初始化logo画图区
	virtual void InitLogoRect(float border1 = 1 / 2.7f, float border2 = 1 / 2.7f);

	// 控制logo大小
	float							m_fBorder1;
	float							m_fBorder2;

	// 提示条
	CToolTipCtrl					m_ToolTip;				// 工具提示条
	void ShowImageSize();									// 显示当前图像大小
	BOOL IsColorsEncodeAble();								// 彩色是否可以编码
	virtual void Repaint();									// 重新绘制/刷新视图

	// 对话框数据
	enum { IDD = IDD_QRENCODE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombolevel();
	afx_msg void OnCbnSelchangeComboversion();
	afx_msg void OnBnClickedCheckautoextent();
	afx_msg void OnCbnSelchangeCombomaskingno();
	afx_msg void OnEnChangeEditmodulesize();
	afx_msg void OnBnClickedImportLogo();
	afx_msg void OnEnChangeEditsourcedataPublic();
	afx_msg void OnEnChangeEditsourcedataPrivate();
	// 改变颜色
	afx_msg void OnEnChangeForegroundR();
	afx_msg void OnEnChangeForegroundG();
	afx_msg void OnEnChangeForegroundB();
	afx_msg void OnEnChangeBackgroundR();
	afx_msg void OnEnChangeBackgroundG();
	afx_msg void OnEnChangeBackgroundB();
	afx_msg void OnEnChangeEnCodeColor1R();
	afx_msg void OnEnChangeEnCodeColor1G();
	afx_msg void OnEnChangeEnCodeColor1B();
	afx_msg void OnEnChangeEnCodeColor2R();
	afx_msg void OnEnChangeEnCodeColor2G();
	afx_msg void OnEnChangeEnCodeColor2B();
	CComboBox m_comboLevel;
	CComboBox m_comboVersion;
	CStatic m_staticVersion;
	CButton m_checkAutoExtent;
	CComboBox m_comboMaskingNo;
	CStatic m_staticMaskingNo;
	CEdit m_editModuleSize;
	CSpinButtonCtrl m_spinModuleSize;
	CStatic m_staticBmpSize;
	CEdit m_editSourceDataPublic;
	CEdit m_editSourceDataPrivate;
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBnClickedBnForeground();
	afx_msg void OnBnClickedBnBackground();
	afx_msg void OnBnClickedBnEncodeColor1();
	afx_msg void OnBnClickedBnEncodeColor2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDeltaposSpinmodulesize(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_sliderWidth;
	CSliderCtrl m_sliderHeight;
	afx_msg void OnReleasedcaptureSliderLogoWidth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnReleasedcaptureSliderLogoHeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedResetLogoSize();
	afx_msg void OnBnClickedClearLogo();
	afx_msg void OnBnClickedEnableMixed();
	CButton m_buttonEnableMixed;
	afx_msg void OnBnClickedButtonEncode();
	afx_msg void OnBnClickedButtonSave();
	CStatic m_staticQRCodeSize;
	CComboBox m_comboInnerEcLevel;
	CComboBox m_comboInnerMaskNo;
	afx_msg void OnCbnSelchangeCombomaskingno2();
	afx_msg void OnBnClickedCheckCompress();
	CButton m_BnCompress;
};