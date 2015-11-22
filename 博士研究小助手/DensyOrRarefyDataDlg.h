#pragma once
#include "afxwin.h"


// CDensyOrRarefyDataDlg 对话框

class CDensyOrRarefyDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDensyOrRarefyDataDlg)

public:
	CDensyOrRarefyDataDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDensyOrRarefyDataDlg();

// 对话框数据
	enum { IDD = IDD_DensyOrRarefy };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_MeanDx;
	double m_SetNewDx;
	CEdit m_MeanDxEdit;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_DataColumnNum;
};
