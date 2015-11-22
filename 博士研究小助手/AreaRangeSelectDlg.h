#pragma once


// CAreaRangeSelectDlg 对话框

class CAreaRangeSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAreaRangeSelectDlg)

public:
	CAreaRangeSelectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAreaRangeSelectDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AreaRangeDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_Xmin;
	double m_Xmax;
	double m_Ymin;
	double m_Ymax;
	afx_msg void OnBnClickedOk();
};
