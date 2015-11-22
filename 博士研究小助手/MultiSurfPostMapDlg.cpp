// MultiSurfPostMapDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��ʿ�о�С����.h"
#include "MultiSurfPostMapDlg.h"
#include "afxdialogex.h"
#include "fstream"
using namespace std;
// CMultiSurfPostMapDlg �Ի���

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


// CMultiSurfPostMapDlg ��Ϣ�������


void CMultiSurfPostMapDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	if (m_AreaRangeFile==_T(""))
	{
		MessageBox(_T("�о�����Χ�ļ�������Ϊ��"));
		return;
	}
	ifstream inf(m_AreaRangeFile);
	if (!inf)
	{
		MessageBox(_T("�Բ�����ѡ����о�����Χbln�ļ�����ȷ"));
		return;
	}
	CDialogEx::OnOK();
}


void CMultiSurfPostMapDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString blnfilter = _T("�о�����Χ�ļ�(*.bln)|*.bln|");
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
