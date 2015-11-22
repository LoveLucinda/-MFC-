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
	//1.2 读取grd文件
	CString filter = _T("Surfer网格信息文件 txt|*.txt|Surfer网格数据 grd|*.grd|所有文件||");
	CString pathname, infilename, extename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	OpenFileDlg.m_ofn.lpstrTitle = _T("读取surfer grid info文件");
	if (IDOK == OpenFileDlg.DoModal())
	{
		pathname = OpenFileDlg.GetFolderPath();
		infilename = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return;
	}
	//根据后缀名判断用那种方法读取
	if (extename==_T("txt"))//读取grd info文件获取最大值最小值
	{
		ifstream if_grdinfo(infilename);
		if (!if_grdinfo)
		{
			MessageBox(_T("读取grid info文件失败：") + infilename);
			return;
		}
		char linechr[500];
		CString firststr;
		while (if_grdinfo.getline(linechr, 500))
		{
			sscanf(linechr, "%s", firststr);
			//MessageBox(firststr);
			if (firststr==_T("Minimum:"))
			{
				//MessageBox(_T("最小值"));
				sscanf(linechr, "%s%lf", firststr,&m_Zmin);
			}
			else if (firststr==_T("Maximum:"))
			{
				//MessageBox(_T("最大值"));
				sscanf(linechr, "%s%lf", firststr, &m_Zmax);
				break;
			}
		}
		if_grdinfo.close();
	}
	else if (extename==_T("grd"))//读取grd文件获取最大值最小值
	{
		ifstream if_grd(infilename);
		if (!if_grd)
		{
			MessageBox(_T("读取grd文件失败：") + infilename);
			return;
		}
		char dsaa[20];
		int temp_int;
		double temp_double;
		if_grd.getline(dsaa, 20); //MessageBox((PCTSTR)(_bstr_t)dsaa);
		if_grd >> temp_int >> temp_int >> temp_double >> temp_double >> temp_double >> temp_double;
		if_grd >> m_Zmin >> m_Zmax;
		if_grd.close();
	}

	UpdateData(false);

	//读取surfer grid info文件获取最大值最小值

}
