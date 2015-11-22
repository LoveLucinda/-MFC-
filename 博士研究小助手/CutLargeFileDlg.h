#pragma once


// CCutLargeFileDlg 对话框

class CCutLargeFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCutLargeFileDlg)

public:
	CCutLargeFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCutLargeFileDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_Num_Line;
	int m_Num_File;
	afx_msg void OnEnKillfocusEdit2();
	int m_TalNum_Line;
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnBnClickedOk();
};
