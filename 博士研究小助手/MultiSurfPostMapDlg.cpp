// MultiSurfPostMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "博士研究小助手.h"
#include "MultiSurfPostMapDlg.h"
#include "afxdialogex.h"
#include "fstream"
using namespace std;
// CMultiSurfPostMapDlg 对话框

IMPLEMENT_DYNAMIC(CMultiSurfPostMapDlg, CDialogEx)

CMultiSurfPostMapDlg::CMultiSurfPostMapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MultiSurPostMapDlg, pParent)
	, m_ExtenedName(_T("xyz"))
	, m_SymFrequency(200)
	, m_SymSise(0.004)
	, m_LableCol(4)
	, m_LabelFontSize(0.1)
	, m_AreaRangeFile(_T(""))
{

}

CMultiSurfPostMapDlg::~CMultiSurfPostMapDlg()
{
}

void CMultiSurfPostMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ExtenedName);
	DDX_Text(pDX, IDC_EDIT4, m_SymFrequency);
	DDX_Text(pDX, IDC_EDIT5, m_SymSise);
	DDX_Text(pDX, IDC_EDIT6, m_LableCol);
	DDX_Text(pDX, IDC_EDIT7, m_LabelFontSize);
	DDX_Text(pDX, IDC_EDIT8, m_AreaRangeFile);
}


BEGIN_MESSAGE_MAP(CMultiSurfPostMapDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMultiSurfPostMapDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CMultiSurfPostMapDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMultiSurfPostMapDlg 消息处理程序


void CMultiSurfPostMapDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_AreaRangeFile==_T(""))
	{
		MessageBox(_T("研究区域范围文件名不能为空"));
		return;
	}
	ifstream inf(m_AreaRangeFile);
	if (!inf)
	{
		MessageBox(_T("对不起，您选择的研究区域范围bln文件不正确"));
		return;
	}
	CDialogEx::OnOK();
}


void CMultiSurfPostMapDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString blnfilter = _T("研究区域范围文件(*.bln)|*.bln|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		blnfilter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		m_AreaRangeFile=OpenFileDlg.GetPathName();
		UpdateData(false);
	}
}
