// CalGlobalMainMagneticDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "博士研究小助手.h"
#include "CalGlobalMainMagneticDlg.h"
#include "afxdialogex.h"


// CCalGlobalMainMagneticDlg 对话框

IMPLEMENT_DYNAMIC(CCalGlobalMainMagneticDlg, CDialogEx)

CCalGlobalMainMagneticDlg::CCalGlobalMainMagneticDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CalMainMagnetic, pParent)
	, m_CofFilename(_T(""))
	, m_StartOrder(1)
	, m_EndOrder(13)
	, m_LonMin(-180)
	, m_LonMax(180)
	, m_LatMin(-89)
	, m_LatMax(89)
	, m_dLon(1)
	, m_dLat(1)
	, m_ComponentType(0)
	, m_IsGMTplot(FALSE)
	, m_GMTProjectionType(0)
	, m_IsDelNcFile(FALSE)
{

}

CCalGlobalMainMagneticDlg::~CCalGlobalMainMagneticDlg()
{
}

void CCalGlobalMainMagneticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_CofFilename);
	DDX_Text(pDX, IDC_EDIT2, m_StartOrder);
	DDX_Text(pDX, IDC_EDIT4, m_EndOrder);
	DDV_MinMaxDouble(pDX, m_EndOrder, 1, 13);
	DDV_MinMaxDouble(pDX, m_StartOrder, 1, 13);
	DDX_Text(pDX, IDC_EDIT3, m_LonMin);
	DDV_MinMaxDouble(pDX, m_LonMin, -180, 180);
	DDX_Text(pDX, IDC_EDIT6, m_LonMax);
	DDV_MinMaxDouble(pDX, m_LonMax, -180, 180);
	DDX_Text(pDX, IDC_EDIT8, m_LatMin);
	DDV_MinMaxDouble(pDX, m_LatMin, -90, 90);
	DDX_Text(pDX, IDC_EDIT9, m_LatMax);
	DDV_MinMaxDouble(pDX, m_LatMax, -90, 90);
	DDX_Text(pDX, IDC_EDIT10, m_dLon);
	DDX_Text(pDX, IDC_EDIT11, m_dLat);
	DDV_MinMaxDouble(pDX, m_dLat, 0.00001, 90);
	DDV_MinMaxDouble(pDX, m_dLon, 0.00000001, 90);
	DDX_Radio(pDX, IDC_RADIO7, m_ComponentType);
	DDX_Check(pDX, IDC_CHECK1, m_IsGMTplot);
	DDX_Radio(pDX, IDC_RADIO8, m_GMTProjectionType);
	DDX_Check(pDX, IDC_CHECK2, m_IsDelNcFile);
}


BEGIN_MESSAGE_MAP(CCalGlobalMainMagneticDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CCalGlobalMainMagneticDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CCalGlobalMainMagneticDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCalGlobalMainMagneticDlg 消息处理程序


void CCalGlobalMainMagneticDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString coffile = _T("模型系数文件(*.cof)|*.cof|所有文件(*.*)|*.*|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		coffile,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		m_CofFilename = OpenFileDlg.GetPathName();
		UpdateData(false);
	}
}


void CCalGlobalMainMagneticDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	//由于这个程序不能计算极点，因此这里作为判断提醒
	double LimitValue = fabs(m_LatMax) + m_dLat;
	if (LimitValue>90)
	{
		CString info;
		info.Format(_T("根据网格间距，纬度最大值建议最大为：%lf"),m_LatMax-m_dLat);
		MessageBox(info);
		m_LatMax = m_LatMax - m_dLat;
		UpdateData(false);
		return;
	}
	LimitValue = fabs(m_LatMin) + m_dLat;
	if (LimitValue >90)
	{
		CString info;
		info.Format(_T("根据网格间距，纬度最小值建议最大为：%lf"), m_LatMin + m_dLat);
		MessageBox(info);
		m_LatMin = m_LatMin + m_dLat;
		UpdateData(false);
		return;
	}
	
	
	CDialogEx::OnOK();
}
