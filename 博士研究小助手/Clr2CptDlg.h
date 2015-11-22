#pragma once


// CClr2CptDlg 对话框

class CClr2CptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CClr2CptDlg)

public:
	CClr2CptDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClr2CptDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Clr2CptDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_Zmin;
	double m_Zmax;
	afx_msg void OnBnClickedButton1();
};
