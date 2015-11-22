// Clr2CptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "博士研究小助手.h"
#include "Clr2CptDlg.h"
#include "afxdialogex.h"
#include "fstream"
using namespace std;

// CClr2CptDlg 对话框

IMPLEMENT_DYNAMIC(CClr2CptDlg, CDialogEx)

CClr2CptDlg::CClr2CptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Clr2CptDlg, pParent)
	, m_Zmin(0)
	, m_Zmax(100)
{

}

CClr2CptDlg::~CClr2CptDlg()
{
}

void CClr2CptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_Zmin);
	DDX_Text(pDX, IDC_EDIT6, m_Zmax);
}


BEGIN_MESSAGE_MAP(CClr2CptDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CClr2CptDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CClr2CptDlg 消息处理程序


void CClr2CptDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//1.2 读取crl文件
	CString filter = _T("Surfer网格数据 grd|*.grd|所有文件|*.*|");
	CString pathname, grdfilename, extename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		pathname = OpenFileDlg.GetFolderPath();
		grdfilename = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return;
	}
	ifstream if_grd(grdfilename);
	if (!if_grd)
	{
		MessageBox(_T("读取clr文件失败：") + grdfilename);
		return ;
	}
	char dsaa[20];
	int temp_int;
	double temp_double;
	if_grd.getline(dsaa, 20); //MessageBox((PCTSTR)(_bstr_t)dsaa);
	if_grd >> temp_int >> temp_int >> temp_double >> temp_double >> temp_double >> temp_double;
	if_grd >> m_Zmin >> m_Zmax;
	if_grd.close();
	UpdateData(false);
}
