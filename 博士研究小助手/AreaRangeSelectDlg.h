#pragma once


// CAreaRangeSelectDlg �Ի���

class CAreaRangeSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAreaRangeSelectDlg)

public:
	CAreaRangeSelectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAreaRangeSelectDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AreaRangeDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_Xmin;
	double m_Xmax;
	double m_Ymin;
	double m_Ymax;
	afx_msg void OnBnClickedOk();
};
