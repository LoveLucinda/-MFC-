#pragma once


// CMultiSurfPostMapDlg 对话框

class CMultiSurfPostMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiSurfPostMapDlg)

public:
	CMultiSurfPostMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMultiSurfPostMapDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MultiSurPostMapDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_ExtenedName;
	int m_SymFrequency;
	double m_SymSise;
	int m_LableCol;
	double m_LabelFontSize;
	afx_msg void OnBnClickedOk();
	CString m_AreaRangeFile;
	afx_msg void OnBnClickedButton1();
};
