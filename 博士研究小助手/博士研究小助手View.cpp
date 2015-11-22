// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// ��ʿ�о�С����View.cpp : C��ʿ�о�С����View ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "��ʿ�о�С����.h"
#endif

#include "��ʿ�о�С����Doc.h"
#include "��ʿ�о�С����View.h"
#include "CutLargeFileDlg.h"
#include "DensyOrRarefyDataDlg.h"
#include "CalGlobalMainMagneticDlg.h"
#include "MagCalc_Version_1_0.h"
#include "MainFrm.h"


//�̺߳���
UINT Dat2GrdThreadFunc(LPVOID ThreadParam);
UINT GrdCutThreadFunc(LPVOID ThreadParam);
UINT Grd2netcdfThreadFunc(LPVOID ThreadParam);
UINT DatCutThreadFunc(LPVOID ThreadParam);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ERR(e) {MessageBox(_T("Error: %s\n"), nc_strerror(e)); /*exit(ERRCODE);*/}

// C��ʿ�о�С����View

IMPLEMENT_DYNCREATE(C��ʿ�о�С����View, CView)

BEGIN_MESSAGE_MAP(C��ʿ�о�С����View, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &C��ʿ�о�С����View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PlantBoundaryFilter, &C��ʿ�о�С����View::OnPlantboundaryfilter)
	ON_COMMAND(ID_CutLargeFile, &C��ʿ�о�С����View::OnCutlargefile)
	ON_COMMAND(ID_ComputeDx, &C��ʿ�о�С����View::OnComputedx)
	ON_COMMAND(ID_DensyOrRarefy, &C��ʿ�о�С����View::OnDensyorrarefy)
	ON_COMMAND(ID_Surfer_MultiPostMap, &C��ʿ�о�С����View::OnSurferMultipostmap)
	ON_COMMAND(ID_GlobalMainMagnetic, &C��ʿ�о�С����View::OnGlobalmainmagnetic)
	ON_COMMAND(ID_GRD2netCDF, &C��ʿ�о�С����View::OnGrd2netcdf)
	ON_COMMAND(ID_netCDF2dat, &C��ʿ�о�С����View::Onnetcdf2dat)
	ON_COMMAND(ID_GrdCut, &C��ʿ�о�С����View::OnGrdcut)
	ON_COMMAND(ID_Dat2Grd, &C��ʿ�о�С����View::OnDat2grd)
	ON_WM_CREATE()
	//�Զ�����Ϣӳ��
	ON_COMMAND(ID_Clr2Cpt, &C��ʿ�о�С����View::OnClr2cpt)
	ON_COMMAND(ID_DatCut, &C��ʿ�о�С����View::OnDatcut)
	ON_COMMAND(ID_ClrTrans, &C��ʿ�о�С����View::OnClrtrans)
END_MESSAGE_MAP()

// C��ʿ�о�С����View ����/����

C��ʿ�о�С����View::C��ʿ�о�С����View()
	: m_temp_pathname(_T(""))
	, m_ProgressMin(0)
	, m_ProgressMax(100)
	, m_CurrentProgressPos(0)
{
	// TODO:  �ڴ˴���ӹ������

}

C��ʿ�о�С����View::~C��ʿ�о�С����View()
{
	//1���ж��Զ�����߳��Ƿ�ִ�����
	//DWORD dwThreadExitCode;
	//GetExitCodeThread(m_CurrentThread, &dwThreadExitCode);
	//if (dwThreadExitCode==STILL_ACTIVE)
	//{
	//	MessageBox(_T("�̻߳�������"));
	//}
	//TerminateThread(m_CurrentThread, dwThreadExitCode);
}

BOOL C��ʿ�о�С����View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

ostream& operator<<(ostream &out, GRDDATA_SURFER_MFC value)
{
	out << "DSAA" << endl;
	out << value.columns << "\t" << value.rows << endl;
	for (int i = 0; i < 3; i++)
	{
		out << value.bounds[i * 2] << "\t" << value.bounds[i * 2 + 1] << endl;
	}
	for (int i = 0; i < value.rows; i++)
	{
		for (int j = 0; j < value.columns; j++)
		{
			out << value.data[j + i*value.columns] << "  ";
		}
		out << endl;
	}
	return out;
}

ifstream &operator>>(ifstream &fin, GRDDATA_SURFER_MFC& value)
{
	if (value.Flag_bin)//�����ƶ�ȡ
	{
		//AfxMessageBox(_T("��ȡ������"));
		char dsbb[4];
		fin.read((char*)(dsbb), sizeof(char) * 4);
		/*if (dsbb[4] != 'B')
		{
			value.Flag_bin = false;
			return fin;
		}*/
		fin.read((char*)&(value.columns), sizeof(short));
		fin.read((char*)&(value.rows), sizeof(short));
		fin.read((char*)value.bounds, sizeof(double) * 6);
		int datanum = value.rows*value.columns;
		if (value.data)
		{
			delete[] value.data;
		}value.data = new float[datanum];
		fin.read((char*)value.data, sizeof(float) *datanum);
		//CString str; str.Format(_T("%d %f "), datanum, value.data[datanum - 2]);
		//AfxMessageBox(str);
	}
	else
	{
		//AfxMessageBox(_T("��ȡ�ı�"));
		char dsaa[10];
		fin >> dsaa;
		if (dsaa[3] != 'A')
		{
			value.Flag_bin = true;
			return fin;
		}
		fin >> value.columns >> value.rows;
		for (int i = 0; i < 3; i++)
		{
			fin >> value.bounds[i * 2] >> value.bounds[i * 2 + 1];
		}
		if (value.data)
		{
			delete[] value.data;
		}value.data = new float[value.rows*value.columns];
		for (int i = 0; i < value.rows; i++)
		{
			for (int j = 0; j < value.columns; j++)
			{
				fin >> value.data[j + i*value.columns];
			}
		}
	}
	
	return fin;
}
// C��ʿ�о�С����View ����

void C��ʿ�о�С����View::OnDraw(CDC* /*pDC*/)
{
	C��ʿ�о�С����Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// C��ʿ�о�С����View ��ӡ


void C��ʿ�о�С����View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL C��ʿ�о�С����View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void C��ʿ�о�С����View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void C��ʿ�о�С����View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}

void C��ʿ�о�С����View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void C��ʿ�о�С����View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// C��ʿ�о�С����View ���

#ifdef _DEBUG
void C��ʿ�о�С����View::AssertValid() const
{
	CView::AssertValid();
}

void C��ʿ�о�С����View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C��ʿ�о�С����Doc* C��ʿ�о�С����View::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C��ʿ�о�С����Doc)));
	return (C��ʿ�о�С����Doc*)m_pDocument;
}
#endif //_DEBUG


// C��ʿ�о�С����View ��Ϣ�������


void C��ʿ�о�С����View::OnPlantboundaryfilter()
{
	// TODO:  �ڴ���������������
	CString filter = _T("ȫ����߽��ļ�(dig.txt)|*.dig.txt|�����ļ� (*.*)|*.*|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		ExtractGlobalPlateBoundary(OpenFileDlg.GetPathName(), OpenFileDlg.GetFolderPath());
		UpdateData(false);
	}
}

int C��ʿ�о�С����View::ExtractGlobalPlateBoundary(CString infilename,CString pathname)
{
	CStdioFile fp;
	CString str_Line;
	if (fp.Open(infilename, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("�� ") + infilename + _T(" ʧ��"));
		return 0;
	}
	char pbName[100];
	PlateBoundaryStructuct temppbstruct;
	CString flag_str = _T("*** end of line segment ***");
	double temp_lon, temp_lat;
	char douhao;
	vector<PlateBoundaryStructuct> plateboundaryvector;
	while (fp.ReadString(str_Line))
	{
		if (str_Line == flag_str)//����������һ���лس������˳�
		{
			break;
		}
		sscanf(str_Line, "%s", pbName);
		temppbstruct.lat.clear();
		temppbstruct.lon.clear();
		temppbstruct.name.Format(_T("%s"),pbName);
		while (fp.ReadString(str_Line))
		{
			if (str_Line == flag_str)
			{
				//MessageBox(str_Line);
				plateboundaryvector.push_back(temppbstruct);
				//MessageBox(temppbstruct.name);
				break;
			}
			else
			{
				sscanf(str_Line,"%lf%c%lf",&temp_lon,&douhao,&temp_lat);
				temppbstruct.lon.push_back(temp_lon);
				temppbstruct.lat.push_back(temp_lat);
			}
		}
	}fp.Close();
	//�����ļ���
	CString foldName = _T("\\��ȡĿ����߽�");
	if (!PathIsDirectory(pathname))
	{
		MessageBox(pathname+_T("\nĿ¼������"));
		return 0;
		//::CreateDirectory(foldName, NULL);
	}
	foldName = pathname+foldName;
	if (!PathIsDirectory(foldName))
	{
		::CreateDirectory(foldName, NULL);
	}
	else
	{
		MessageBox(pathname+_T("\n���ݱ���Ŀ¼����"));
	}
	//�����ļ�
	for (int i = 0; i < plateboundaryvector.size(); i++)
	{
		CString filename = foldName+_T("\\")+plateboundaryvector[i].name+_T(".txt");
		FILE* fpout = NULL;
		if ((fpout = fopen(filename,"w"))==NULL)
		{
			MessageBox(_T("�� ")+filename+_T(" ʧ��"));
			return 0;
		}
		for (int j = 0; j < plateboundaryvector[i].lon.size(); j++)
		{
			fprintf(fpout,"%lf\t%lf\n",plateboundaryvector[i].lon[j],plateboundaryvector[i].lat[j]);
		}
		//�ر��ļ�
		fclose(fpout);
	}
	MessageBox(_T("��ȡ���"));
	return 0;
}


void C��ʿ�о�С����View::OnCutlargefile()
{
	// TODO:  �ڴ���������������
	CString filter = _T("�ı��ļ�|*.txt|�ı��ļ� DAT|*.dat|�����ļ� (*.*)|*.*|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		CutLargeFile(OpenFileDlg.GetFileName(), OpenFileDlg.GetFolderPath());
		UpdateData(false);
	}
}


int C��ʿ�о�С����View::CutLargeFile(CString infilename, CString pathname)
{
	CStdioFile fp;
	CString str_Line;
	CString infile = pathname + _T("\\") + infilename;
	if (fp.Open(infile, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("�� ") + infile + _T(" ʧ��"));
		return 0;
	}
	long TotalNum_Line = 0;
	while (fp.ReadString(str_Line))
	{
		TotalNum_Line++;
	}
	//���������Ϣ
	const int max_line = 99999;
	CCutLargeFileDlg cutfiledlg;
	cutfiledlg.m_Num_Line = max_line;
	cutfiledlg.m_Num_File = TotalNum_Line / cutfiledlg.m_Num_Line;
	cutfiledlg.m_TalNum_Line = TotalNum_Line;
	if (cutfiledlg.DoModal()!=IDOK)
	{
		fp.Close();
		return 0;
	}
	int num_file = cutfiledlg.m_Num_File;
	int num_Line = cutfiledlg.m_Num_Line;
	//������ļ�
	CString foldName = _T("\\�ָ�Ŀ¼");
	if (!PathIsDirectory(pathname))
	{
		MessageBox(pathname + _T("\nĿ¼������"));
		return 0;
		//::CreateDirectory(foldName, NULL);
	}
	foldName = pathname + foldName;
	if (!PathIsDirectory(foldName))
	{
		::CreateDirectory(foldName, NULL);
	}
	else
	{
		MessageBox(foldName + _T("\n���ݱ���Ŀ¼����"));
	}
	//

	//�����ļ�
	fp.SeekToBegin();//ָ��ص�ͷ��
	for (int i = 0; i < num_file-1; i++)
	{
		CString filename;
		filename.Format(_T("%s\\%s_%d.%s"), foldName, infilename, i, infilename.Right(3));
		FILE* fpout = NULL;
		if ((fpout = fopen(filename, "w")) == NULL)
		{
			MessageBox(_T("�� ") + filename + _T(" ʧ��"));
			return 0;
		}
		for (int j = 0; j < num_Line; j++)
		{
			fp.ReadString(str_Line);
			fprintf(fpout, "%s\n", (PCSTR)(_bstr_t)(PCTSTR)str_Line);
		}
		fclose(fpout);
	}
	CString filename;
	filename.Format(_T("%s\\%s_%d.%s"), foldName, infilename, num_file-1, infilename.Right(3));
	FILE* fpout = NULL;
	if ((fpout = fopen(filename, "w")) == NULL)
	{
		MessageBox(_T("�� ") + filename + _T(" ʧ��"));
		return 0;
	}
	while (fp.ReadString(str_Line))
	{
		fprintf(fpout, "%s\n", (PCSTR)(_bstr_t)(PCTSTR)str_Line);
	}
	fclose(fpout);
	fp.Close();

	MessageBox(_T("�ָ����"));
	return 0;
}


void C��ʿ�о�С����View::OnComputedx()
{
	// TODO:  �ڴ���������������
	CString filter = _T("�ı��ļ�|*.txt|�ı��ļ� DAT|*.dat|�����ļ� (*.*)|*.*|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		ComputeDx(OpenFileDlg.GetFileName(), OpenFileDlg.GetFolderPath(),OpenFileDlg.GetFileExt());
	}
}


int C��ʿ�о�С����View::ComputeDx(CString filename, CString pathname,CString extname)
{
	CStdioFile fp;
	CString str_Line;
	CString infile = pathname + _T("\\") + filename;
	if (fp.Open(infile, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("�� ") + infile + _T(" ʧ��"));
		return 0;
	}
	long TotalNum_Line = 0;
	while (fp.ReadString(str_Line))
	{
		TotalNum_Line++;
	}fp.SeekToBegin();

	struct Gravstruct
	{
		double x, y,Distance;//dDistance����������ĵ��
		CString otherdatastr;
	};
	Gravstruct* gravdata = new Gravstruct[TotalNum_Line];
	//fp.ReadString(str_Line);
	//sscanf(str_Line, "%lf%lf", &gravdata[0].x, &gravdata[0].y);
	//gravdata[0].otherdatastr = str_Line;
	//gravdata[0].Distance = 0;//��һ����ĵ��Ϊ0
	double xx2, yy2;
	for (int i = 0; i < TotalNum_Line; i++)
	{
		fp.ReadString(str_Line);
		sscanf(str_Line,"%lf%lf",&gravdata[i].x,&gravdata[i].y);
		gravdata[i].otherdatastr = str_Line;
		xx2 = gravdata[i].x - gravdata[0].x; xx2 = xx2*xx2;
		yy2 = gravdata[i].y - gravdata[0].y; yy2 = yy2*yy2;
		gravdata[i].Distance = sqrt(xx2 + yy2);
	}fp.Close();

	//������ļ�
	CString outfile = pathname + _T("\\") + filename.Left(filename.GetLength() - extname.GetLength()-1)+_T("_�ӵ��.")+extname;
	FILE* fpout = NULL;
	if ((fpout = fopen(outfile,"w"))==NULL)
	{
		MessageBox(_T("�� ")+outfile+_T(" ʧ��"));
		return 0;
	}
	for (int i = 0; i < TotalNum_Line; i++)
	{
		fprintf(fpout, "%.1f\t%s\n", gravdata[i].Distance, (PCSTR)(_bstr_t)(PCTSTR)gravdata[i].otherdatastr);
	}

	fclose(fpout);
	delete[] gravdata;
	return 0;
}


void C��ʿ�о�С����View::OnDensyorrarefy()
{
	MessageBox(_T("�˹����Ѿ��ڲ�ʿ�о�����Matlab�汾��ʵ���ˣ�����matlab�汾"));
	return;
	// TODO:  �ڴ���������������
	CString filter = _T("�ı��ļ�|*.txt|�ı��ļ� DAT|*.dat|�����ļ� (*.*)|*.*|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		
		DensyOrRarefy(OpenFileDlg.GetFileName(), OpenFileDlg.GetFolderPath(), OpenFileDlg.GetFileExt());
	}
}


int C��ʿ�о�С����View::DensyOrRarefy(CString filename, CString pathname, CString extname)
{
	CStdioFile fp;
	CString str_Line;
	CString infile = pathname + _T("\\") + filename;
	if (fp.Open(infile, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("�� ") + infile + _T(" ʧ��"));
		return 0;
	}
	long TotalNum_Line = 0;
	double TotalLength;
	while (fp.ReadString(str_Line))
	{
		TotalNum_Line++;
		sscanf(str_Line, "%lf", &TotalLength);
	}fp.SeekToBegin();
	fp.ReadString(str_Line);
	int DataColumnNum = 0;
	//��ȡһ�����ݸ��ݿո����Tab�ж��ж���������
	char seperatorchr;
	int datacolumnNum = 0;
	int num_repeat = 0;
	for (int i = 0; i < str_Line.GetLength(); i++)
	{
		sscanf(str_Line.Right(str_Line.GetLength()-i),"%c",&seperatorchr);
		
		if (seperatorchr == 32 || seperatorchr == 9)
		{
			//MessageBox(_T("�ָ���"));
			if (num_repeat==0)
			{
				datacolumnNum++;
			}
			num_repeat++;
		}
		else
		{
			num_repeat = 0;
		}
	}
	//-----
	fp.SeekToBegin();
	
	double MeanDistance = TotalLength / TotalNum_Line;
	//�Ի�����ʾ��Ϣ������ʾ������
	CDensyOrRarefyDataDlg densyorrarefyDlg;
	densyorrarefyDlg.m_MeanDx = MeanDistance;
	densyorrarefyDlg.m_SetNewDx = MeanDistance;
	densyorrarefyDlg.m_DataColumnNum = datacolumnNum;
	if (densyorrarefyDlg.DoModal()!=IDOK)
	{
		return 0;
	}
	//��ȡ���õĵ�������
	MeanDistance = densyorrarefyDlg.m_MeanDx;
	datacolumnNum = densyorrarefyDlg.m_DataColumnNum;


	return 0;
}

int C��ʿ�о�С����View::WriteGrdTonetCDF(CString path,CString grdfilename,CString extname)
{
	CString filename_nc = path + _T("\\") + grdfilename.Left(grdfilename.GetLength() - extname.GetLength()) + _T("nc");
	CString infilename = path +_T("\\")+ grdfilename;

	//--------------������--------------------
	CString xaxisName = _T("Longitude");
	CString yaxisName = _T("Latitude");
	CString xaxis_units = _T("degree");
	CString UNITS = _T("Units");
	CString yaxis_units = _T("degree");
	CString data_name = _T("gravity");
	CString data_units = _T("mGal");
	int dims_data = 2;
	int retval, ncid, x_dimid, y_dimid, x_varid, y_varid, datavarid;
	//----------------------------------------
	//1. ��ȡgrd����
	GRDDATA_SURFER_MFC grddata = {};
	ifstream fin;
	fin.open(infilename);
	if (!fin)
	{
		MessageBox(_T("�� ")+infilename + _T(" ʧ��"));
		return 0;
	}
	int d;
	grddata.Flag_bin = false;//��ʼ�����ı��ļ���ȡ
	fin >> grddata;
	fin.close();
	//CString str; str.Format(_T("nx:%d  ny:%d"), grddata.columns,grddata.rows); MessageBox(str);
	if (grddata.Flag_bin)//�����ı�����û��DSAA�����������ƶ�ȡ���ж��Ƿ���DSBB
	{
		//MessageBox(_T("�п����Ƕ������ļ�"));
		ifstream fin_bin;
		fin_bin.open(infilename, std::ifstream::binary);//�����Ʒ�ʽ��
		if (!fin_bin)
		{
			MessageBox(_T("�� ") + infilename + _T(" ʧ��"));
			return 0;
		}
		fin_bin >> grddata;
		fin_bin.close();
		if (!grddata.Flag_bin)//����ö����ƶ�ȡҲû��DSBB���򲻷��ϸ�ʽҪ���˳�
		{
			MessageBox(_T("�ļ���ʽ����ȷ���Ȳ���DSAAҲ����DSBB"));
			return 0;
		}
	}
	//׼������
	double *x = new double[grddata.columns];
	double* y = new double[grddata.rows];
	double dx = (grddata.bounds[1] - grddata.bounds[0]) / (grddata.columns-1);
	double dy = (grddata.bounds[3] - grddata.bounds[2]) / (grddata.rows-1);
	for (int i = 0; i < grddata.columns; i++)
	{
		x[i] = grddata.bounds[0] + dx*i;
	}
	for (int i = 0; i < grddata.rows; i++)
	{
		y[i] = grddata.bounds[2] + dy*i;
	}
	//2. д��netCDF��������nc�ļ�
	
	if ((retval = nc_create((_bstr_t)filename_nc, NC_CLOBBER, &ncid)))
		ERR(retval);
	//3. ����ά��
	if ((retval = nc_def_dim(ncid, (_bstr_t)yaxisName, grddata.rows, &y_dimid)))
		ERR(retval);
	if ((retval = nc_def_dim(ncid, (_bstr_t)xaxisName, grddata.columns, &x_dimid)))
		ERR(retval);
	//4. �������������
	if ((retval = nc_def_var(ncid, (_bstr_t)yaxisName, NC_DOUBLE, 1, &y_dimid, &y_varid)))
		ERR(retval);
	if ((retval = nc_def_var(ncid, (_bstr_t)xaxisName, NC_DOUBLE, 1, &x_dimid, &x_varid)))
		ERR(retval);
	
	//5. �����������������
	if ((retval = nc_put_att_text(ncid, y_varid, (_bstr_t)UNITS, strlen((_bstr_t)yaxis_units), (_bstr_t)yaxis_units)))
		ERR(retval);
	if ((retval = nc_put_att_text(ncid, x_varid, (_bstr_t)UNITS, strlen((_bstr_t)xaxis_units), (_bstr_t)xaxis_units)))
		ERR(retval);
	
	//6. ����netCDF����
	int dims[2]; dims[0] = y_dimid; dims[1] = x_dimid;
	if ((retval = nc_def_var(ncid, data_name, NC_DOUBLE, dims_data,
		dims, &datavarid)))
		ERR(retval);
	//7. ������������
	if ((retval = nc_put_att_text(ncid, datavarid, (_bstr_t)UNITS,
		strlen((_bstr_t)data_units), (_bstr_t)data_units)))
		ERR(retval);
	//8. ��������
	if ((retval = nc_enddef(ncid)))
		ERR(retval);
	//9 д���ݡ�������
	if ((retval = nc_put_var_double(ncid, x_varid, &x[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(ncid, y_varid, &y[0])))
		ERR(retval);
	//10 д���ݡ�������
	if ((retval = nc_put_var_float(ncid, datavarid, &grddata.data[0])))
		ERR(retval);
	//11. �ر��ļ�
	if ((retval = nc_close(ncid)))
		ERR(retval);
	//�ͷ�ָ����Դ
	delete[] grddata.data;
	return 1;
}
int CALLBACK BrowseCallBackFun2(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:  //ѡ���ļ��жԻ����ʼ�� 
							//����Ĭ��·��ΪlpData��'D:\' 
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		//��STATUSTEXT������ʾ��ǰ·�� 
		::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, lpData);
		//����ѡ���ļ��жԻ���ı��� 
		::SetWindowText(hwnd, TEXT("��ѡ�����ݴ洢·��"));
		break;
	case BFFM_SELCHANGED:   //ѡ���ļ��б��ʱ 
	{
		TCHAR pszPath[MAX_PATH];
		//��ȡ��ǰѡ��·�� 
		SHGetPathFromIDList((LPCITEMIDLIST)lParam, pszPath);
		//��STATUSTEXT������ʾ��ǰ·�� 
		::SendMessage(hwnd, BFFM_SETSTATUSTEXT, TRUE, (LPARAM)pszPath);
	}
	break;
	}
	return 0;
}

void C��ʿ�о�С����View::OnSurferMultipostmap()
{
	// ��C++��ȡһ��Ŀ¼�µ�������Ҫ���ļ�����Ȼ�������Щ�ļ���дsurfer script�ļ���Ȼ��ִ��
	CString path, extenName;
	vector<CString> filenamevector;
	vector<CString> pathnamevector;
	//����ļ���
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = this->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = NULL;
	bi.ulFlags = BIF_USENEWUI;
	bi.lpfn = BrowseCallBackFun2;     //�ص����� 
	bi.lParam = (LPARAM)(LPCTSTR)m_temp_pathname;  //�����ص������Ĳ���,����Ĭ��·�� 
	bi.iImage = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL)
	{
		return;
	}

	if (SHGetPathFromIDList(pidl, pszPath))
	{
		path.Format(_T("%s"),pszPath);
		m_temp_pathname = path;
		//MessageBox(path);
	}
	//�������öԻ���
	if (m_MultiSurfPostMapDlg.DoModal()!=IDOK)
	{
		return;
	}
	extenName = m_MultiSurfPostMapDlg.m_ExtenedName;
	int symfrequency = m_MultiSurfPostMapDlg.m_SymFrequency;
	double symsize = m_MultiSurfPostMapDlg.m_SymSise;
	int labelcol = m_MultiSurfPostMapDlg.m_LableCol;
	double labelfontsize = m_MultiSurfPostMapDlg.m_LabelFontSize;
	CString baselayerdatafile = m_MultiSurfPostMapDlg.m_AreaRangeFile;
	//2. ���ú����õ����Ŀ¼�����е��ļ���
	int fileNum = GetAllFileName(path, extenName, pathnamevector, filenamevector);
	if (fileNum == 0)
	{
		CString errorinfo;
		errorinfo.Format(_T("��Ŀ¼��û��%s�ļ�����ѡ����ȷ·���������öԻ�����������ȷ�ĺ�׺��"),extenName);
		MessageBox(errorinfo);
		return;
	}
	//�����ļ��д�������ɵĹ�surfer���õ��ļ�
	CString foldName = _T("\\Data�ļ�");
	CString newpath = path+foldName;
	if (!PathIsDirectory(newpath))
	{
		::CreateDirectory(newpath, NULL);
	}


	//�½��ļ���д���µ��ļ�
	
	for (int i = 0; i < fileNum; i++)
	{
		
		CStdioFile fp;
		CString str_Line;
		if (fp.Open(pathnamevector[i], CFile::modeRead) == FALSE)
		{
			MessageBox(_T("��ȡ���ļ�ʧ�ܣ� ") + pathnamevector[i]);
			return ;
		}
		vector<CString> datstrvector;
		while (fp.ReadString(str_Line))//��ȡ�ļ��������У�Ȼ��д��bln�ļ�
		{
			datstrvector.push_back(str_Line);
		}fp.Close();
	
		//��ͬһ�ļ������һ��label�У�label���ļ�������
		CString filename = newpath +_T("\\")+ filenamevector[i];
		ofstream blnoutf(filename); 
		if (!blnoutf)
		{
			MessageBox(_T("д����ļ�ʧ��: ")+ filename);
			return;
		}
		for (int j = 0; j < datstrvector.size(); j++)
		{
			blnoutf << datstrvector[j] <<"\t"<<filenamevector[i].Left(filenamevector[i].GetLength()-1-extenName.GetLength()) << "\t" << j  << endl;
		}
		blnoutf.close();
	}
	//3. д��surfer BAS�ļ�
	CString basfilename = path + _T("\\��������PostMap.bas");
	
	ofstream surfBatOF(basfilename);
	if (!surfBatOF)
	{
		MessageBox(_T("���ļ�����: ")+ path + _T("\\��������PostMap.bas"));
		return;
	}
	//3. ѭ��д��BAS�ļ�
	surfBatOF << "Sub Main" << endl;
		surfBatOF << "\tSet SurferApp=CreateObject(\"Surfer.Application\")" << endl;
		surfBatOF << "\tDim Doc As Object" << endl;
		//surfBatOF << "\tSurferApp.Visible=True" << endl;
		surfBatOF << "\tDim plot As Object" << endl;
		surfBatOF << "\tSet plot=SurferApp.Documents.Add(srfDocPlot)" << endl;
		surfBatOF << "\tSet MapFrame = plot.Shapes.AddBaseMap(\""<< baselayerdatafile <<"\")" << endl;
		surfBatOF << "\tSet BaseLayer= MapFrame.Overlays(1)" << endl;
		//surfBatOF << "\tBaseLayer.SymFrequency = 50" << endl;
		//ѭ�����ʣ���map
		for (int i = 1; i < fileNum; i++)
		{
			CString filename = newpath + _T("\\") + filenamevector[i];
			surfBatOF << "\tSet PostLayer = plot.Shapes.AddPostLayer(Map:=MapFrame, DataFileName:=\"" << filename <<"\")"<< endl;
			surfBatOF << "\tPostLayer.SymFrequency = "<<symfrequency << endl;
			//surfBatOF << "\tPostLayer.SymFrequency = 200" << endl;
			//surfBatOF << "\tPostLayer.SymbolColorMap.LoadPreset(\"Rainbow2\")" << endl;
			surfBatOF << "\tPostLayer.Symbol.Set = \"GSI Default Symbols\"" << endl;
			surfBatOF << "\tPostLayer.Symbol.Index= 12" << endl;
			surfBatOF << "\tPostLayer.Symbol.Size="<< symsize << endl;
			surfBatOF << "\tPostLayer.ActiveLabelSet = 1" << endl;
			surfBatOF << "\tPostLayer.LabCol = "<<labelcol << endl;
			surfBatOF << "\tPostLayer.LabelFont.Size = "<<labelfontsize << endl;
		}

	//	surfBatOF << "\n\tMapFrame.SetLimitsToData" << endl;
		surfBatOF << "\tSurferApp.Visible=True" << endl;
	surfBatOF << "End Sub" << endl;
	//�ر��ļ�
	surfBatOF.close();

	//����BAS�ļ�
	ShellExecute(this->m_hWnd, "open", basfilename, "", "", SW_SHOW);//�򿪼�����
}


int C��ʿ�о�С����View::GetAllFileName(CString path, CString extenName, vector<CString>& pathnamevector, vector<CString>& filenameVector)
{
	//������������
	int DataFileNum = 0;
	CFileFind finder;
	//·��
	CString strWildcard(path);
	strWildcard += _T("//*.*");
	// ��ʼ���ҵ�һ���ļ�
	BOOL bWorking = finder.FindFile(strWildcard);
	filenameVector.clear();//ʹ������ǰ�����
						   //�ж�ѭ����ֱ����һ��������
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots()) //�ж��Ƿ����ļ�
			continue;
		if (!finder.IsDirectory())
		{
			if (finder.GetFileName().Right(3) == extenName)
			{
				DataFileNum++;
				filenameVector.push_back(finder.GetFileName());
				pathnamevector.push_back(finder.GetFilePath());
			}
		}
		//if (finder.IsDirectory()) //�ж��Ƿ����ļ��� //·��
		//{
		//     CString str = finder.GetFilePath();
		//     //���������·���µ��ļ����������Լ������øú���
		//     //GetFileCount(str);
		//}
	}
	finder.Close();
	return DataFileNum;
}
UINT Grd2netcdfThreadFunc(LPVOID ThreadParam)
{
	C��ʿ�о�С����View * pView = (C��ʿ�о�С����View *)ThreadParam;
	pView->Grd2netcdf();
	return 0;
}
void C��ʿ�о�С����View::OnGrd2netcdf()
{
	// TODO:  �ڴ���������������
	if (IDOK == AfxMessageBox(_T("�˹���Surfer13�Ѿ����ˣ����Ϊ�Ϳ���\n����Ҫִ�д˹�����"), MB_OK | MB_OKCANCEL))
	{
		AfxBeginThread((AFX_THREADPROC)Grd2netcdfThreadFunc, this);
	}
	
}

void C��ʿ�о�С����View::OnGlobalmainmagnetic()
{
	if (m_CalGlobalMainMagDlg.DoModal() != IDOK)
	{
		return;
	}
	//�����ļ�
	CString outfilename;
	switch (m_CalGlobalMainMagDlg.m_ComponentType)
	{
	case COMPONENT_T:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_T.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_X:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_X.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_Y:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_Y.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_Z:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_Z.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_H:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_H.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_D:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_D.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_I:
		outfilename.Format(_T("�ش�ģ��%d��_%d��_I.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	}
	
	CFileDialog SaveFileDlg(FALSE,
		_T(".grd"),
		outfilename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		TEXT("Surfer Grd Text (*.grd)|*.grd||"),
		NULL
		);
	if (SaveFileDlg.DoModal()!=IDOK)
	{
		return;
	}
	
	if(!CalMag(m_CalGlobalMainMagDlg.m_CofFilename, m_CalGlobalMainMagDlg.m_ComponentType, m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder,
		m_CalGlobalMainMagDlg.m_LonMin,	m_CalGlobalMainMagDlg.m_LonMax, m_CalGlobalMainMagDlg.m_dLon, 
		m_CalGlobalMainMagDlg.m_LatMin, m_CalGlobalMainMagDlg.m_LatMax, m_CalGlobalMainMagDlg.m_dLat,
		SaveFileDlg.GetPathName()))return;
	//���ѡ����GMT��ͼ��ִ��GMT��ͼ����
	if (m_CalGlobalMainMagDlg.m_IsGMTplot)
	{
		CString outgrdfile = SaveFileDlg.GetPathName();
		CString extname = SaveFileDlg.GetFileExt();
		//1.1ת��Ϊnc��ʽ
		WriteGrdTonetCDF(SaveFileDlg.GetFolderPath(), SaveFileDlg.GetFileName(), SaveFileDlg.GetFileExt());
		CString filename_nc = SaveFileDlg.GetFolderPath() + _T("\\") + SaveFileDlg.GetFileName().Left(SaveFileDlg.GetFileName().GetLength() - extname.GetLength()) + _T("nc");
		switch (m_CalGlobalMainMagDlg.m_GMTProjectionType)
		{
		case JN_PROJ:
		{
			//����bat�ļ�
			CString batfile = SaveFileDlg.GetFolderPath() + _T("\\JNͶӰ.bat");
			ofstream ofbat(batfile);
			if (!ofbat)
			{
				MessageBox(_T("����JNͶӰbat�ļ�ʧ��\n") + batfile);
				return;
			}
			//1.2 �㼯��ͼ����
			ofbat << "gmtset MAP_FRAME_TYPE fancy\n";
			ofbat << "gmtset PLOT_DEGREE_FORMAT ddd:mm:ssF\n";
			ofbat << "set psf=" << outgrdfile.Left(outgrdfile.GetLength() - extname.GetLength() - 1) << ".ps\n";
			ofbat << "set inf=" << filename_nc << "\n";
			ofbat << "set incpt=rainbow\n";
			ofbat << "set cptf=tmp.cpt\n";
			ofbat << "del %psf%\n";
			ofbat << "gmt psbasemap -Rg -JN0/5i -B120/30:." << m_CalGlobalMainMagDlg.m_StartOrder << "-" << m_CalGlobalMainMagDlg.m_EndOrder << ": -Y17 -P -K >>%psf%\n";
			ofbat << "grd2cpt %inf% -C%incpt% -Z -V >%cptf%\n";
			ofbat << "grdimage %inf% -R -J -C%cptf% -E135 -O -K -V >> %psf%\n";
			ofbat << "gmt pscoast -R -J -A10000 -Wthinnest -Dc  -O -K >>%psf%\n";
			ofbat << "psscale -D6.3/-1/10/.4h -C%cptf% -Baf500 -By+lnT -O -K -I -V -E>> %psf%\n";
			ofbat << "del *.cpt *.history *.conf \n";
			if (m_CalGlobalMainMagDlg.m_IsDelNcFile)
			{
				ofbat << "del " << filename_nc << "\n";
			}
			ofbat << "gsview32 %psf%\n";
			//ofbat<< "pause";
			ofbat.close();
			ShellExecute(this->m_hWnd, "open", (_bstr_t)batfile, "", "", SW_HIDE);//
		}
			break;
		case JW_PROJ:
		{
			//����bat�ļ�
			CString batfile = SaveFileDlg.GetFolderPath() + _T("\\JWͶӰ.bat");
			ofstream ofbat(batfile);
			if (!ofbat)
			{
				MessageBox(_T("����JWͶӰbat�ļ�ʧ��\n") + batfile);
				return;
			}
			//1.2 �㼯��ͼ����
			ofbat << "gmtset MAP_FRAME_TYPE fancy\n";
			ofbat << "gmtset PLOT_DEGREE_FORMAT ddd:mm:ssF\n";
			ofbat << "set psf=" << outgrdfile.Left(outgrdfile.GetLength() - extname.GetLength() - 1) << ".ps\n";
			ofbat << "set inf=" << filename_nc << "\n";
			ofbat << "set incpt=rainbow\n";
			ofbat << "set cptf=tmp.cpt\n";
			ofbat << "del %psf%\n";
			ofbat << "gmt psbasemap -JW0/7i -Rg -B120/30:." << m_CalGlobalMainMagDlg.m_StartOrder << "-" << m_CalGlobalMainMagDlg.m_EndOrder << ": -Y10 -P -K >>%psf%\n";
			ofbat << "grd2cpt %inf% -C%incpt% -Z -V >%cptf%\n";
			ofbat << "grdimage %inf% -R -JW -C%cptf% -O -K -E135 -V >> %psf%\n";
			ofbat << "psscale -D7.8/-0.5/15/.4h -C%cptf% -Baf500 -By+lnT -O -K -I -V -E>> %psf%\n";
			ofbat << "gmt pscoast -R -J -A10000 -Wthinnest -Dc  -O -K >>%psf%\n";
			ofbat << "REM ����ͶӰ\n";
			ofbat << "psbasemap -R-180/180/45/90 -JG0/90/3i -Y10 -Ba30g30/a30g15 -O -K>> %psf%\n";
			ofbat << "grdimage %inf% -R -J -C%cptf% -O -K -E135 -V >> %psf%\n";
			ofbat << "gmt pscoast -R -J -A10000 -Wthinnest -Dc  -O -K >>%psf%\n";
			ofbat << "REM �ϼ�ͶӰ\n";
			ofbat << "psbasemap -R-180/180/-90/-60 -JS0/-90/3i -X9.5 -Ba30g30/a30g15 -O -K >> %psf%\n";
			ofbat << "grdimage %inf% -R -J -C%cptf% -E135 -O -K -V >> %psf%\n";
			ofbat << "gmt pscoast -R -J -A10000 -Wthinnest -Dc  -O >>%psf%\n";
			ofbat << "del *.cpt *.history *.conf\n";
			if (m_CalGlobalMainMagDlg.m_IsDelNcFile)
			{
				ofbat << "del " << filename_nc << "\n";
			}
			ofbat << "gsview32 %psf%\n";
			//ofbat<< "pause";
			ofbat.close();
			ShellExecute(this->m_hWnd, "open", (_bstr_t)batfile, "", "", SW_HIDE);//
		}
		break;
		}
		
	}
	else
	{
		MessageBox(_T("�������"));
	}
	
}

void C��ʿ�о�С����View::Onnetcdf2dat()
{
	// TODO: �ڴ���������������
	CString filter = _T("netCDF nc�ļ�|*.nc|�����ļ�|*.*|");
	CString pathname, infilename, extename;
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
		infilename = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return;
	}
	//1.1 дnc2dat������
	CString gmtcommandfile = pathname+_T("\\nc2dat.bat");
	CString datfilename = infilename.Left(infilename.GetLength()-extename.GetLength())+_T("dat");
	//CString grdfilename= infilename.Left(infilename.GetLength() - extename.GetLength()-1) + _T("_surf.grd");
	ofstream of_bat(gmtcommandfile);
	if (!of_bat)
	{
		MessageBox(_T("nc2batʱ����bat�ļ�ʧ��"));
		return;
	}
	else
	{
		of_bat << "gmt grd2xyz " << infilename << " > " << datfilename << "\n"<<"del "<< gmtcommandfile;
		of_bat.close();
	}
	//1.2 ִ������
	HINSTANCE hd;
	hd=ShellExecute(this->m_hWnd, "open", (_bstr_t)gmtcommandfile, "", "", SW_SHOW);//
	//1.3 ��datת��Ϊgrd
	
	
}


void C��ʿ�о�С����View::OnGrdcut()
{
	AfxBeginThread((AFX_THREADPROC)GrdCutThreadFunc, this);
	
}


int C��ʿ�о�С����View::Dat2Grd_Surfer()
{
	CString filter = _T("����dat�ļ� dat|*.dat|�����ļ�|*.*|");
	CString pathname, datfile, extename;
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
		datfile = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return 0;
	}
	CString grdfile = datfile.Left(datfile.GetLength() - extename.GetLength() - 1) + _T("_surf.grd");
	//Dat2Grd_Surfer(infilename, grdfilename);
	//��ȡ����
	m_ProgressBar->SetPos(20);
	vector<vector<double>> grddata_vec;
	vector<double> rowvec;
	double x, y, z, xmin, xmax, ymin, ymax, zmin, zmax, x0, y0,x1,y1,x2,y2;
	FILE* fp;
	if ((fp = fopen((_bstr_t)datfile, "r")) == NULL)
	{
		AfxMessageBox(_T("dat2grd:��") + datfile + _T("ʧ��"));
		return 0;
	}
	fscanf(fp, "%lf%lf%lf", &x, &y, &z);
	x0 = x; y0 = y; x1 = x; y1 = y;
	xmin = x; xmax = x; ymin = y; ymax = y; zmin = z; zmax = z;
	fscanf(fp, "%lf%lf%lf", &x2, &y2, &z);
	UpdateData(true);
	//m_Progress.SetRange(0, m_rows);
	//��ʵ����һ�������dat�ļ���ת��Ϊgrd�ܼ򵥣�ֻ��Ҫ��ȡ�����������ԣ�x��y��ֵ�Լ���һ�����λ�þ���
	vector<int>colsvector;
	int cols = 0;
	rewind(fp);//�ص��ļ�ͷ
	while (!feof(fp))
	{
		fscanf(fp, "%lf%lf%lf", &x, &y, &z);
		if (x<xmin)xmin = x;
		if (x>xmax)xmax = x;
		if (y<ymin)ymin = y;
		if (y>ymax)ymax = y;
		if (z<zmin)zmin = z;
		if (z>zmax)zmax = z;
		
		if (y == y0)
		{
			cols++;
			rowvec.push_back(z);
		}
		else
		{
			colsvector.push_back(cols);
			grddata_vec.push_back(rowvec);
			rowvec.clear();
			cols = 0;
			y0 = y;
		}
	}
	colsvector.push_back(cols);//���һ��
	grddata_vec.push_back(rowvec);
	fclose(fp);
	//��ֵ��Ҫ����
	int rows = colsvector.size();
	cols = colsvector[0];
	int ArrangeType = 0;
	if (x1==x2)
	{
		//MessageBox(_T("����Ϊ��"));
		if (x1==xmin)
		{
		//	MessageBox(_T("��������"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("��������"));
				ArrangeType = COL_LEFT2RIGHT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("��������"));
				ArrangeType = COL_LEFT2RIGHT_UP2DOWN;
			}
		}
		else
		{
		//	MessageBox(_T("��������"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("��������"));
				ArrangeType = COL_RIGHT2LEFT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("��������"));
				ArrangeType = COL_RIGHT2LEFT_UP2DOWN;
			}
		}
		
	}
	else if (y2==y1)
	{
	//	MessageBox(_T("����Ϊ��"));
		if (x1==xmin)
		{
		//	MessageBox(_T("��������"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("��������"));
				ArrangeType = ROW_LEFT2RIGHT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("��������"));
				ArrangeType = ROW_LEFT2RIGHT_UP2DOWN;
			}
		}
		else
		{
		//	MessageBox(_T("��������"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("��������"));
				ArrangeType = ROW_RIGHT2LEFT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("��������"));
				ArrangeType = ROW_RIGHT2LEFT_UP2DOWN;
			}
		}
		
	}
	//CString str; str.Format(_T("%d  %d  \n%lf %lf\n%lf %lf\n%lf %lf\n"), colsvector[0], colsvector.size(), xmin, xmax, ymin, ymax, zmin, zmax); MessageBox(str);
	m_ProgressBar->SetPos(m_ProgressMax);
	//���

	CFileDialog SaveFileDlg(FALSE,
		_T(".grd"),
		grdfile,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		TEXT("grd (*.grd)|*.grd|"),
		NULL
		);
	CString OutFileName;
	if (IDOK == SaveFileDlg.DoModal())
	{
		OutFileName = SaveFileDlg.GetPathName();
	}
	else
	{
		return 0;
	}

	FILE* fpout;
	if ((fpout = fopen((_bstr_t)OutFileName, "w")) == NULL)
	{
		AfxMessageBox(_T("��") + OutFileName + _T("ʧ��"));
		return 0;
	}
	//m_Progress.SetRange(0, m_rows);
	fprintf(fpout, "DSAA\n%d %d\n%lf %lf\n%lf %lf\n%lf %lf\n", grddata_vec[0].size(), grddata_vec.size(), xmin, xmax, ymin, ymax, zmin, zmax);
	int num_persetp = grddata_vec.size() / m_ProgressMax;
	if (num_persetp<1)
	{
		num_persetp = 1;
	}
	switch (ArrangeType)
	{
	case ROW_LEFT2RIGHT_DOWN2UP:
	{
		for (int i = 0; i <rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				fprintf(fpout, "%lf ", grddata_vec[i][j]);
			}
			fprintf(fpout, "\n");
			if (i%num_persetp)
			{
				m_ProgressBar->SetPos((int)(i / num_persetp));
			}
		}
	}
		break;
	case ROW_LEFT2RIGHT_UP2DOWN:
	{
		for (int i = 0; i <rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				fprintf(fpout, "%lf ", grddata_vec[rows-i-1][j]);
			}
			fprintf(fpout, "\n");
			if (i%num_persetp)
			{
				m_ProgressBar->SetPos((int)(i / num_persetp));
			}
		}
	}
		break;
	case ROW_RIGHT2LEFT_DOWN2UP:
	{
		for (int i = 0; i <rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				fprintf(fpout, "%lf ", grddata_vec[i][cols-1-j]);
			}
			fprintf(fpout, "\n");
			if (i%num_persetp)
			{
				m_ProgressBar->SetPos((int)(i / num_persetp));
			}
		}
	}
		break;
	case ROW_RIGHT2LEFT_UP2DOWN:
	{
		for (int i = 0; i <rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				fprintf(fpout, "%lf ", grddata_vec[rows-1-i][cols-1-j]);
			}
			fprintf(fpout, "\n");
			if (i%num_persetp)
			{
				m_ProgressBar->SetPos((int)(i / num_persetp));
			}
		}
	}
		break;
	case COL_LEFT2RIGHT_DOWN2UP:
	{
		for (int j = 0; j <cols; j++)
		{
			for (int i = 0; i < rows; i++)
			{
				fprintf(fpout, "%lf ", grddata_vec[i][j]);
			}
			fprintf(fpout, "\n");
			if (j%num_persetp)
			{
				m_ProgressBar->SetPos((int)(j / num_persetp));
			}
		}
	}
			break;
	case COL_LEFT2RIGHT_UP2DOWN:
	{
		for (int j = 0; j <cols; j++)
		{
			for (int i = 0; i < rows; i++)
			{
				fprintf(fpout, "%lf ", grddata_vec[i][cols-1-j]);
			}
			fprintf(fpout, "\n");
			if (j%num_persetp)
			{
				m_ProgressBar->SetPos((int)(j / num_persetp));
			}
		}
	}
		break;
	case COL_RIGHT2LEFT_DOWN2UP:
	{
		for (int j = 0; j <cols; j++)
		{
			for (int i = 0; i < rows; i++)
			{
				fprintf(fpout, "%lf ", grddata_vec[rows-1-i][j]);
			}
			fprintf(fpout, "\n");
			if (j%num_persetp)
			{
				m_ProgressBar->SetPos((int)(j / num_persetp));
			}
		}
	}
		break;
	case COL_RIGHT2LEFT_UP2DOWN:
	{
		for (int j = 0; j <cols; j++)
		{
			for (int i = 0; i < rows; i++)
			{
				fprintf(fpout, "%lf ", grddata_vec[rows - 1 - i][cols-1-j]);
			}
			fprintf(fpout, "\n");
			if (j%num_persetp)
			{
				m_ProgressBar->SetPos((int)(j / num_persetp));
			}
		}
	}
		break;
	default:
	{
		for (int i = grddata_vec.size() - 1; i >= 0; i--)
		{
			for (int j = 0; j < grddata_vec[0].size(); j++)
			{
				fprintf(fpout, "%lf ", grddata_vec[i][j]);
			}
			fprintf(fpout, "\n");
			if (i%num_persetp)
			{
				m_ProgressBar->SetPos((int)(i / num_persetp));
			}
		}
	}
		break;
	}
	fclose(fpout);
	m_ProgressBar->SetPos(0);
	MessageBox(_T("ת�����"));
	
	return 0;
}

UINT Dat2GrdThreadFunc(LPVOID ThreadParam)
{
	C��ʿ�о�С����View * pView = (C��ʿ�о�С����View *)ThreadParam;
	pView->Dat2Grd_Surfer();
	return 0;
}
void C��ʿ�о�С����View::OnDat2grd()
{
	AfxBeginThread((AFX_THREADPROC)Dat2GrdThreadFunc, this);
}


int C��ʿ�о�С����View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������  AfxBeginThread((AFX_THREADPROC)UpdateFigureThrFunAfterRead,this);
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	m_ProgressBar= DYNAMIC_DOWNCAST(CMFCRibbonProgressBar, pMainFrm->m_wndRibbonBar.FindByID(ID_PROGRESS1));
	m_ProgressBar->SetRange(m_ProgressMin, m_ProgressMax);
	m_ProgressBar->SetPos(m_CurrentProgressPos);
	return 0;
}


void C��ʿ�о�С����View::OnClr2cpt()
{
	// TODO: �ڴ���������������
	if (m_Clr2CptDlg.DoModal()!=IDOK)
	{
		return;
	}
	double zmin = m_Clr2CptDlg.m_Zmin;
	double zmax = m_Clr2CptDlg.m_Zmax;
	//1.2 ��ȡcrl�ļ�
	CString filter = _T("Surferɫ���ļ� clr|*.clr|�����ļ�|*.*|");
	CString pathname, clrfile, extename;
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
		clrfile = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return ;
	}
	CString cptfile = clrfile.Left(clrfile.GetLength() - extename.GetLength()) + _T("cpt");
	Clr2Cpt(clrfile,cptfile,zmin,zmax);
}


int C��ʿ�о�С����View::Clr2Cpt(CString clrfile, CString cptfile, double zmin, double zmax)
{
	double zLength = fabs(zmin - zmax);
	struct CLRSTRUCT
	{
		double percent, value;
		int R, G, B, A;
	};
	//1.1 ��ȡclr�ļ�
	ifstream if_clr(clrfile);
	if (!if_clr)
	{
		MessageBox(_T("��ȡclr�ļ�ʧ�ܣ�")+clrfile);
		return 0;
	}
	CLRSTRUCT clr_temp;
	vector<CLRSTRUCT> clrVector;
	char tempchar[20];
	if_clr.getline(tempchar, 20);
	while (if_clr>>clr_temp.percent && if_clr >> clr_temp.R && if_clr >> clr_temp.G && if_clr >> clr_temp.B && if_clr >> clr_temp.A)
	{
		clr_temp.value = clr_temp.percent/100.0*zLength + zmin;
		clrVector.push_back(clr_temp);
	}if_clr.close();
	
	//1.2д��cpt�ļ�
	ofstream of_cpt(cptfile);
	if (!of_cpt)
	{
		MessageBox(_T("д��cpt�ļ�ʧ�ܣ�") + cptfile);
		return 0;
	}
	of_cpt << "# Converts clr format to cpt format\n";
	of_cpt << "# Converted to the cpt format by Zhikui Guo\n";
	of_cpt << "# Sequential palette with "<<clrVector.size()<<" colours\n";
	of_cpt << "# COLOR_MODEL=RGBA\n";
	for (int i = 1; i < clrVector.size(); i++)
	{
		of_cpt << clrVector[i-1].value << " " << clrVector[i-1].R << " " << clrVector[i-1].G << " " << clrVector[i-1].B<<"\t";
		of_cpt << clrVector[i].value << " " << clrVector[i].R << " " << clrVector[i].G << " " << clrVector[i].B << "\n";
	}
	of_cpt.close();
	return 0;
}


void C��ʿ�о�С����View::OnDatcut()
{
	// TODO: �ڴ���������������
	AfxBeginThread((AFX_THREADPROC)DatCutThreadFunc, this);
}


int C��ʿ�о�С����View::GmtCutnetCDF(CString infilename, CString outfilename, double xmin, double xmax, double ymin, double ymax)
{
	CString gmtcommandfile =  _T("grdcut.bat");
	
	ofstream of_bat(gmtcommandfile);
	if (!of_bat)
	{
		MessageBox(_T("nc2batʱ����bat�ļ�ʧ��"));
		return 0;
	}
	else
	{
		of_bat << "gmt grdcut " << infilename << " -G" << outfilename << " -R" << xmin << "/"<< xmax << "/" << ymin << "/" << ymax << " -V\n"
			<< "del *.history " << gmtcommandfile;
		of_bat.close();
	}
	//1.2 ִ������
	ShellExecute(this->m_hWnd, "open", (_bstr_t)gmtcommandfile, "", "", SW_SHOW);//
	return 0;
}


int C��ʿ�о�С����View::GrdCut_Surfer(CString infilename, CString outfilename, double xmin, double xmax, double ymin, double ymax)
{
	//1.1 ��ȡgrd����
	char dsaa[20];
	CString dsaaStr;
	int rows,cols;
	double bounds[6];
	ifstream fin_grd(infilename);
	if (!fin_grd)
	{
		MessageBox(_T("��ȡgrd�����ļ�ʧ��: ")+infilename);
		return 0;
	}
	fin_grd.getline(dsaa, 20);
	dsaaStr = (PCSTR)(_bstr_t)(PCTSTR)dsaa;
	if (dsaaStr.Left(4)!=_T("DSAA"))
	{
		MessageBox(_T("��ѡ����ļ�����Surfer Text Grid�ļ�\n")+infilename);
		fin_grd.close();
		return 0;
	}
	
	//��ȡͷ��Ϣ
	fin_grd >> cols >> rows;
	for (int i = 0; i < 6; i++)
	{
		fin_grd >> bounds[i];
	}
	//1.2 ���ݸ����ķ�Χȷ�������������
	double dx = (bounds[1] - bounds[0]) / (cols - 1);
	double dy = (bounds[3] - bounds[2]) / (rows - 1);
	int index_xmin, index_xmax, index_ymin, index_ymax;
	//1.2.1 x��Сֵ
	double symbol = xmin - bounds[0];
	if (symbol <= 0)
	{
		index_xmin = 0;
	}
	else
	{
		index_xmin = (int)(symbol / dx);
		if (index_xmin >= cols)
		{
			CString strinfo;
			strinfo.Format(_T("�Բ�������ȡ����������������֮��\nxmax_data:%lf;xmin_cut:%lf"), bounds[1], xmin);
			MessageBox(strinfo);
			return 0;
		}
	}
	//1.2.2 x���ֵ
	symbol = bounds[1] - xmax;
	if (symbol <= 0)
	{
		index_xmax = cols;
	}
	else
	{
		index_xmax = cols - (int)(symbol / dx);
		if (index_xmax>cols)
		{
			CString strinfo;
			strinfo.Format(_T("�Բ�������ȡ����������������֮��\nxmin_data:%lf;xmax_cut:%lf"), bounds[0], xmax);
			MessageBox(strinfo);
			return 0;
		}
	}
	//1.2.3 y��Сֵ
	symbol = ymin - bounds[2];
	if (symbol <= 0)
	{
		index_ymin = 0;
	}
	else
	{
		index_ymin = (int)(symbol / dy);
		if (index_ymin >= rows)
		{
			CString strinfo;
			strinfo.Format(_T("�Բ�������ȡ����������������֮��\nymax_data:%lf;ymin_cut:%lf"), bounds[3], ymin);
			MessageBox(strinfo);
			return 0;
		}
	}
	//1.2.4 y���ֵ
	symbol = bounds[3] - ymax;
	if (symbol <= 0)
	{
		index_ymax = rows;
	}
	else
	{
		index_ymax = rows - (int)(symbol / dy);
		if (index_ymax>rows)
		{
			CString strinfo;
			strinfo.Format(_T("�Բ�������ȡ����������������֮��\nymin_data:%lf;ymax_cut:%lf"), bounds[2], ymax);
			MessageBox(strinfo);
			return 0;
		}
	}
	//CString str; str.Format(_T("%d %d\n%d  %d"), index_xmin, index_xmax, index_ymin, index_ymax); MessageBox(str);
	
	//1.3 ���ݸ�����Χ�����ά����
	int newrows = (index_ymax - index_ymin);
	int nrewcols = (index_xmax - index_xmin);
	int prosStep =(int)( rows / m_ProgressMax);
	if (prosStep<1)
	{
		prosStep = 1;
	}
	double** data = new double*[newrows];
	for (int i = 0; i < newrows; i++)
	{
		data[i] = new double[nrewcols];
	}
	double tempdouble;
	for (int i = 0; i < rows; i++)
	{
		if (i>=index_ymin && i<index_ymax)
		{
			for (int j = 0; j < cols; j++)
			{
				if (j>=index_xmin && j<index_xmax)
				{
					fin_grd >> data[i-index_ymin][j-index_xmin];
				}
				else
				{
					fin_grd >> tempdouble;
				}
				
				//CString str; str.Format(_T("%d %d"), rows,cols); MessageBox(str);
			}
		}
		else
		{
			for (int j = 0; j < cols; j++)
			{
				fin_grd >> tempdouble;
			}
		}
		
		if (!(i%prosStep))
		{
			m_ProgressBar->SetPos(i/prosStep);
		}
	}fin_grd.close();
	m_ProgressBar->SetPos(m_ProgressMax);
	//1.3 ���grd���ļ�
	//�������ֵ��Сֵ
	bounds[4] = data[0][0], bounds[5] = data[0][0];
	for (int i = 0; i < newrows; i++)
	{
		for (int j = 0; j < nrewcols; j++)
		{
			if (data[i][j] < bounds[4])bounds[4] = data[i][j];
			if (data[i][j] > bounds[5])bounds[5] = data[i][j];
		}
	}
	bounds[0] = xmin; bounds[1] = xmax;
	bounds[2] = ymin; bounds[3] = ymax;
	ofstream fout_grd(outfilename);
	if (!fout_grd)
	{
		MessageBox(_T("���grd�����ļ�ʧ��: ") + infilename);
		return 0;
	}
	m_ProgressBar->SetPos(0);
	int per_step = newrows / m_ProgressMax;
	if (per_step<1)
	{
		per_step = 1;
	}
	fout_grd << "DSAA\n" << nrewcols << "\t" << newrows << "\n";
	for (int i = 0; i < 6; i=i+2)
	{
		fout_grd << bounds[i] << "\t" << bounds[i + 1] << "\n";
	}
	for (int i = 0; i < newrows; i++)
	{
		for (int j = 0; j < nrewcols; j++)
		{
			fout_grd << data[i][j]<<" ";
		}
		fout_grd << "\n";
		if (!i%per_step)
		{
			m_ProgressBar->SetPos(i/per_step);
		}
	}fout_grd.close();
	//�ͷ�ָ��
	for (int i = 0; i < newrows; i++)
	{
		delete[] data[i];
	}delete[] data;

	MessageBox(_T("��ȡ�ɹ�"));
	m_ProgressBar->SetPos(0);
	return 0;
}
UINT DatCutThreadFunc(LPVOID ThreadParam)
{
	C��ʿ�о�С����View * pView = (C��ʿ�о�С����View *)ThreadParam;
	pView->DatCut();
	return 0;
}

UINT GrdCutThreadFunc(LPVOID ThreadParam)
{
	C��ʿ�о�С����View * pView = (C��ʿ�о�С����View *)ThreadParam;
	pView->GrdCut();
	return 0;
}
int C��ʿ�о�С����View::GrdCut()
{
	//1.1 ѡ�����ȥ���ļ�
	CString filter = _T("netCDF nc�ļ�|*.nc|GRD Surfer Txt Grid (*.grd)|*.grd|�����ļ�|*.*|");
	CString pathname, infilename, extename;
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
		infilename = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return 0;
	}
	//1.2 ѡ�������С
	if (m_AreaRangeSelDlg.DoModal() != IDOK)
	{
		return 0;
	}
	CString cuttedFilename;
	
	if (extename == _T("nc"))
	{
		//1.3 дgrdcut������
		cuttedFilename.Format(_T("%s_%.2f_%.2f_%.2f_%.2f.nc"), infilename.Left(infilename.GetLength() - extename.GetLength() - 1),
			m_AreaRangeSelDlg.m_Xmin, m_AreaRangeSelDlg.m_Xmax, m_AreaRangeSelDlg.m_Ymin, m_AreaRangeSelDlg.m_Ymax);
		GmtCutnetCDF(infilename, cuttedFilename, m_AreaRangeSelDlg.m_Xmin, m_AreaRangeSelDlg.m_Xmax, m_AreaRangeSelDlg.m_Ymin, m_AreaRangeSelDlg.m_Ymax);
	}
	else if (extename == _T("grd"))
	{
		cuttedFilename.Format(_T("%s_%.2f_%.2f_%.2f_%.2f.grd"), infilename.Left(infilename.GetLength() - extename.GetLength() - 1),
			m_AreaRangeSelDlg.m_Xmin, m_AreaRangeSelDlg.m_Xmax, m_AreaRangeSelDlg.m_Ymin, m_AreaRangeSelDlg.m_Ymax);
		GrdCut_Surfer(infilename, cuttedFilename, m_AreaRangeSelDlg.m_Xmin, m_AreaRangeSelDlg.m_Xmax, m_AreaRangeSelDlg.m_Ymin, m_AreaRangeSelDlg.m_Ymax);
	}
	else
	{
		MessageBox(_T("��ʽ������ѡ��nc��surfer ��grd Text��ʽ"));
		return 0;
	}
}


int C��ʿ�о�С����View::Grd2netcdf()
{
	CString filter = _T("GRD surfer Text Grid|*.grd|");
	CString infilename;
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY,
		filter,
		NULL
		);
	if (IDOK == OpenFileDlg.DoModal())
	{
		if(WriteGrdTonetCDF(OpenFileDlg.GetFolderPath(), OpenFileDlg.GetFileName(), OpenFileDlg.GetFileExt()))MessageBox(_T("ת���ɹ�"));

	}
	return 0;
}


int C��ʿ�о�С����View::DatCut()
{
	//1.1 ѡ�����ȥ���ļ�
	CString filter = _T("Dat �ļ�|*.dat|Dat Txt�ļ� (*.txt)|*.txt||");
	CString pathname, infilename, extename;
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
		infilename = OpenFileDlg.GetPathName();
		extename = OpenFileDlg.GetFileExt();
	}
	else
	{
		return 0;
	}
	//1.2 ѡ�������С
	if (m_AreaRangeSelDlg.DoModal() != IDOK)
	{
		return 0;
	}
	double xmin = m_AreaRangeSelDlg.m_Xmin, xmax = m_AreaRangeSelDlg.m_Xmax, ymin = m_AreaRangeSelDlg.m_Ymin, ymax = m_AreaRangeSelDlg.m_Ymax;
	CString cuttedFilename;
	cuttedFilename.Format(_T("%s_%.2f_%.2f_%.2f_%.2f.%s"), infilename.Left(infilename.GetLength() - extename.GetLength() - 1),
		m_AreaRangeSelDlg.m_Xmin, m_AreaRangeSelDlg.m_Xmax, m_AreaRangeSelDlg.m_Ymin, m_AreaRangeSelDlg.m_Ymax,extename);

	//��ȥdat����
	CStdioFile fin_dat;//�����ļ�
	CString str_Line;
	if (fin_dat.Open(infilename, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("��ȡdat�����ļ�ʧ��: ") + infilename);
		return 0;
	}
	ofstream fout_cutteddat(cuttedFilename);//����ļ�
	if (!fout_cutteddat)
	{
		MessageBox(_T("д��dat�����ļ�ʧ��: ") + cuttedFilename);
		return 0;
	}
	//ѭ���ж϶�ȡ�����
	double x, y;
	while (fin_dat.ReadString(str_Line))
	{
		sscanf((_bstr_t)str_Line,"%lf%lf", &x, &y);

		if (x <= xmax && x >= xmin && y <= ymax && y >= ymin)
		{
			fout_cutteddat << str_Line << "\n";
		}
	}
	//�ر��ļ�
	fin_dat.Close();
	fout_cutteddat.close();
	MessageBox(_T("��ȥ�ɹ�"));
	return 0;
}


void C��ʿ�о�С����View::OnClrtrans()
{
	// TODO: �ڴ���������������
}
