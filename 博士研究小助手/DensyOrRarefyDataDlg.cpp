// DensyOrRarefyDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "博士研究小助手.h"
#include "DensyOrRarefyDataDlg.h"
#include "afxdialogex.h"


// CDensyOrRarefyDataDlg 对话框

IMPLEMENT_DYNAMIC(CDensyOrRarefyDataDlg, CDialogEx)

CDensyOrRarefyDataDlg::CDensyOrRarefyDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDensyOrRarefyDataDlg::IDD, pParent)
	, m_MeanDx(0)
	, m_SetNewDx(0)
	, m_DataColumnNum(0)
{

}

CDensyOrRarefyDataDlg::~CDensyOrRarefyDataDlg()
{
}

void CDensyOrRarefyDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_MeanDx);
	DDX_Text(pDX, IDC_EDIT2, m_SetNewDx);
	DDX_Control(pDX, IDC_EDIT1, m_MeanDxEdit);
	DDX_Text(pDX, IDC_EDIT3, m_DataColumnNum);
}


BEGIN_MESSAGE_MAP(CDensyOrRarefyDataDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDensyOrRarefyDataDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDensyOrRarefyDataDlg 消息处理程序


void CDensyOrRarefyDataDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
	CDialogEx::OnOK();
}


BOOL CDensyOrRarefyDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_MeanDxEdit.SetReadOnly(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
