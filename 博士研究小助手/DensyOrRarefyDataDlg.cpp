// DensyOrRarefyDataDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��ʿ�о�С����.h"
#include "DensyOrRarefyDataDlg.h"
#include "afxdialogex.h"


// CDensyOrRarefyDataDlg �Ի���

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


// CDensyOrRarefyDataDlg ��Ϣ�������


void CDensyOrRarefyDataDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	CDialogEx::OnOK();
}


BOOL CDensyOrRarefyDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_MeanDxEdit.SetReadOnly(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
