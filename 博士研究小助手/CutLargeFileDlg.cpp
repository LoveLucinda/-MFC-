// CutLargeFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "博士研究小助手.h"
#include "CutLargeFileDlg.h"
#include "afxdialogex.h"


// CCutLargeFileDlg 对话框

IMPLEMENT_DYNAMIC(CCutLargeFileDlg, CDialogEx)

CCutLargeFileDlg::CCutLargeFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCutLargeFileDlg::IDD, pParent)
	, m_Num_Line(0)
	, m_Num_File(0)
	, m_TalNum_Line(0)
{

}

CCutLargeFileDlg::~CCutLargeFileDlg()
{
}

void CCutLargeFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Num_File);
	DDV_MinMaxInt(pDX, m_Num_File, 0, 999999999);
	DDX_Text(pDX, IDC_EDIT2, m_Num_Line);
	DDV_MinMaxInt(pDX, m_Num_Line , 0, 1000000);
}


BEGIN_MESSAGE_MAP(CCutLargeFileDlg, CDialogEx)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CCutLargeFileDlg::OnEnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CCutLargeFileDlg::OnEnKillfocusEdit1)
	ON_BN_CLICKED(IDOK, &CCutLargeFileDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCutLargeFileDlg 消息处理程序


void CCutLargeFileDlg::OnEnKillfocusEdit2()
{
	// TODO:  在此添加控件通知处理程序代码
	if (UpdateData(true))
	{
		m_Num_File = m_TalNum_Line / m_Num_Line;
	}
	UpdateData(false);
}


void CCutLargeFileDlg::OnEnKillfocusEdit1()
{
	// TODO:  在此添加控件通知处理程序代码
	if (UpdateData(true))
	{
		m_Num_Line = m_TalNum_Line / m_Num_File;
	}
	UpdateData(false);
}


void CCutLargeFileDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
	CDialogEx::OnOK();
}
