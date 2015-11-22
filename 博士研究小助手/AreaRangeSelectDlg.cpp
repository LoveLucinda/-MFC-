// AreaRangeSelectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��ʿ�о�С����.h"
#include "AreaRangeSelectDlg.h"
#include "afxdialogex.h"


// CAreaRangeSelectDlg �Ի���

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


// CAreaRangeSelectDlg ��Ϣ�������


void CAreaRangeSelectDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}
