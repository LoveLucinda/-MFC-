// AreaRangeSelectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "博士研究小助手.h"
#include "AreaRangeSelectDlg.h"
#include "afxdialogex.h"


// CAreaRangeSelectDlg 对话框

IMPLEMENT_DYNAMIC(CAreaRangeSelectDlg, CDialogEx)

CAreaRangeSelectDlg::CAreaRangeSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AreaRangeDlg, pParent)
	, m_Xmin(49.3)
	, m_Xmax(49.9)
	, m_Ymin(-37.7)
	, m_Ymax(-37.5)
{

}

CAreaRangeSelectDlg::~CAreaRangeSelectDlg()
{
}

void CAreaRangeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_Xmin);
	DDX_Text(pDX, IDC_EDIT6, m_Xmax);
	DDX_Text(pDX, IDC_EDIT8, m_Ymin);
	DDX_Text(pDX, IDC_EDIT9, m_Ymax);
}


BEGIN_MESSAGE_MAP(CAreaRangeSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAreaRangeSelectDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAreaRangeSelectDlg 消息处理程序


void CAreaRangeSelectDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
