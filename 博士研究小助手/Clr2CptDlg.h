#pragma once


// CClr2CptDlg �Ի���

class CClr2CptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CClr2CptDlg)

public:
	CClr2CptDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClr2CptDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Clr2CptDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_Zmin;
	double m_Zmax;
	afx_msg void OnBnClickedButton1();
};
