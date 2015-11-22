#pragma once


// CCalGlobalMainMagneticDlg �Ի���
#define JN_PROJ	0
#define JW_PROJ	1

class CCalGlobalMainMagneticDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCalGlobalMainMagneticDlg)

public:
	CCalGlobalMainMagneticDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCalGlobalMainMagneticDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CalMainMagnetic };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_CofFilename;
	afx_msg void OnBnClickedButton1();
	int m_StartOrder;
	int m_EndOrder;
	double m_LonMin;
	double m_LonMax;
	double m_LatMin;
	double m_LatMax;
	double m_dLon;
	double m_dLat;
	afx_msg void OnBnClickedOk();
	int m_ComponentType;
	BOOL m_IsGMTplot;
	int m_GMTProjectionType;
	BOOL m_IsDelNcFile;
};
