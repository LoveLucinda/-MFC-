#pragma once
#include "afxwin.h"


// CDensyOrRarefyDataDlg �Ի���

class CDensyOrRarefyDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDensyOrRarefyDataDlg)

public:
	CDensyOrRarefyDataDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDensyOrRarefyDataDlg();

// �Ի�������
	enum { IDD = IDD_DensyOrRarefy };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_MeanDx;
	double m_SetNewDx;
	CEdit m_MeanDxEdit;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_DataColumnNum;
};
