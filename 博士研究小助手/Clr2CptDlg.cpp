// Clr2CptDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "��ʿ�о�С����.h"
#include "Clr2CptDlg.h"
#include "afxdialogex.h"
#include "fstream"
using namespace std;

// CClr2CptDlg �Ի���

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


// CClr2CptDlg ��Ϣ�������


void CClr2CptDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//1.2 ��ȡgrd�ļ�
	CString filter = _T("Surfer������Ϣ�ļ� txt|*.txt|Surfer�������� grd|*.grd|�����ļ�||");
	CString pathname, infilename, extename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	OpenFileDlg.m_ofn.lpstrTitle = _T("��ȡsurfer grid info�ļ�");
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
	//���ݺ�׺���ж������ַ�����ȡ
	if (extename==_T("txt"))//��ȡgrd info�ļ���ȡ���ֵ��Сֵ
	{
		ifstream if_grdinfo(infilename);
		if (!if_grdinfo)
		{
			MessageBox(_T("��ȡgrid info�ļ�ʧ�ܣ�") + infilename);
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
				//MessageBox(_T("��Сֵ"));
				sscanf(linechr, "%s%lf", firststr,&m_Zmin);
			}
			else if (firststr==_T("Maximum:"))
			{
				//MessageBox(_T("���ֵ"));
				sscanf(linechr, "%s%lf", firststr, &m_Zmax);
				break;
			}
		}
		if_grdinfo.close();
	}
	else if (extename==_T("grd"))//��ȡgrd�ļ���ȡ���ֵ��Сֵ
	{
		ifstream if_grd(infilename);
		if (!if_grd)
		{
			MessageBox(_T("��ȡgrd�ļ�ʧ�ܣ�") + infilename);
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

	//��ȡsurfer grid info�ļ���ȡ���ֵ��Сֵ

}
