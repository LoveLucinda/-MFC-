// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// 博士研究小助手View.cpp : C博士研究小助手View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "博士研究小助手.h"
#endif

#include "博士研究小助手Doc.h"
#include "博士研究小助手View.h"
#include "CutLargeFileDlg.h"
#include "DensyOrRarefyDataDlg.h"
#include "CalGlobalMainMagneticDlg.h"
#include "MagCalc_Version_1_0.h"
#include "MainFrm.h"


//线程函数
UINT Dat2GrdThreadFunc(LPVOID ThreadParam);
UINT GrdCutThreadFunc(LPVOID ThreadParam);
UINT Grd2netcdfThreadFunc(LPVOID ThreadParam);
UINT DatCutThreadFunc(LPVOID ThreadParam);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ERR(e) {MessageBox(_T("Error: %s\n"), nc_strerror(e)); /*exit(ERRCODE);*/}

// C博士研究小助手View

IMPLEMENT_DYNCREATE(C博士研究小助手View, CView)

BEGIN_MESSAGE_MAP(C博士研究小助手View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &C博士研究小助手View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PlantBoundaryFilter, &C博士研究小助手View::OnPlantboundaryfilter)
	ON_COMMAND(ID_CutLargeFile, &C博士研究小助手View::OnCutlargefile)
	ON_COMMAND(ID_ComputeDx, &C博士研究小助手View::OnComputedx)
	ON_COMMAND(ID_DensyOrRarefy, &C博士研究小助手View::OnDensyorrarefy)
	ON_COMMAND(ID_Surfer_MultiPostMap, &C博士研究小助手View::OnSurferMultipostmap)
	ON_COMMAND(ID_GlobalMainMagnetic, &C博士研究小助手View::OnGlobalmainmagnetic)
	ON_COMMAND(ID_GRD2netCDF, &C博士研究小助手View::OnGrd2netcdf)
	ON_COMMAND(ID_netCDF2dat, &C博士研究小助手View::Onnetcdf2dat)
	ON_COMMAND(ID_GrdCut, &C博士研究小助手View::OnGrdcut)
	ON_COMMAND(ID_Dat2Grd, &C博士研究小助手View::OnDat2grd)
	ON_WM_CREATE()
	//自定义消息映射
	ON_COMMAND(ID_Clr2Cpt, &C博士研究小助手View::OnClr2cpt)
	ON_COMMAND(ID_DatCut, &C博士研究小助手View::OnDatcut)
	ON_COMMAND(ID_ClrTrans, &C博士研究小助手View::OnClrtrans)
END_MESSAGE_MAP()

// C博士研究小助手View 构造/析构

C博士研究小助手View::C博士研究小助手View()
	: m_temp_pathname(_T(""))
	, m_ProgressMin(0)
	, m_ProgressMax(100)
	, m_CurrentProgressPos(0)
{
	// TODO:  在此处添加构造代码

}

C博士研究小助手View::~C博士研究小助手View()
{
	//1。判断自定义的线程是否执行完毕
	//DWORD dwThreadExitCode;
	//GetExitCodeThread(m_CurrentThread, &dwThreadExitCode);
	//if (dwThreadExitCode==STILL_ACTIVE)
	//{
	//	MessageBox(_T("线程还在运行"));
	//}
	//TerminateThread(m_CurrentThread, dwThreadExitCode);
}

BOOL C博士研究小助手View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

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
	if (value.Flag_bin)//二进制读取
	{
		//AfxMessageBox(_T("读取二进制"));
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
		//AfxMessageBox(_T("读取文本"));
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
// C博士研究小助手View 绘制

void C博士研究小助手View::OnDraw(CDC* /*pDC*/)
{
	C博士研究小助手Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// C博士研究小助手View 打印


void C博士研究小助手View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL C博士研究小助手View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void C博士研究小助手View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void C博士研究小助手View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void C博士研究小助手View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void C博士研究小助手View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// C博士研究小助手View 诊断

#ifdef _DEBUG
void C博士研究小助手View::AssertValid() const
{
	CView::AssertValid();
}

void C博士研究小助手View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C博士研究小助手Doc* C博士研究小助手View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C博士研究小助手Doc)));
	return (C博士研究小助手Doc*)m_pDocument;
}
#endif //_DEBUG


// C博士研究小助手View 消息处理程序


void C博士研究小助手View::OnPlantboundaryfilter()
{
	// TODO:  在此添加命令处理程序代码
	CString filter = _T("全球板块边界文件(dig.txt)|*.dig.txt|所有文件 (*.*)|*.*|");
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

int C博士研究小助手View::ExtractGlobalPlateBoundary(CString infilename,CString pathname)
{
	CStdioFile fp;
	CString str_Line;
	if (fp.Open(infilename, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("打开 ") + infilename + _T(" 失败"));
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
		if (str_Line == flag_str)//如果读到最后一行有回车，则退出
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
	//创建文件夹
	CString foldName = _T("\\提取目标板块边界");
	if (!PathIsDirectory(pathname))
	{
		MessageBox(pathname+_T("\n目录不存在"));
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
		MessageBox(pathname+_T("\n数据保存目录存在"));
	}
	//保存文件
	for (int i = 0; i < plateboundaryvector.size(); i++)
	{
		CString filename = foldName+_T("\\")+plateboundaryvector[i].name+_T(".txt");
		FILE* fpout = NULL;
		if ((fpout = fopen(filename,"w"))==NULL)
		{
			MessageBox(_T("打开 ")+filename+_T(" 失败"));
			return 0;
		}
		for (int j = 0; j < plateboundaryvector[i].lon.size(); j++)
		{
			fprintf(fpout,"%lf\t%lf\n",plateboundaryvector[i].lon[j],plateboundaryvector[i].lat[j]);
		}
		//关闭文件
		fclose(fpout);
	}
	MessageBox(_T("提取完成"));
	return 0;
}


void C博士研究小助手View::OnCutlargefile()
{
	// TODO:  在此添加命令处理程序代码
	CString filter = _T("文本文件|*.txt|文本文件 DAT|*.dat|所有文件 (*.*)|*.*|");
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


int C博士研究小助手View::CutLargeFile(CString infilename, CString pathname)
{
	CStdioFile fp;
	CString str_Line;
	CString infile = pathname + _T("\\") + infilename;
	if (fp.Open(infile, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("打开 ") + infile + _T(" 失败"));
		return 0;
	}
	long TotalNum_Line = 0;
	while (fp.ReadString(str_Line))
	{
		TotalNum_Line++;
	}
	//输出行数信息
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
	//输出到文件
	CString foldName = _T("\\分割目录");
	if (!PathIsDirectory(pathname))
	{
		MessageBox(pathname + _T("\n目录不存在"));
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
		MessageBox(foldName + _T("\n数据保存目录存在"));
	}
	//

	//保存文件
	fp.SeekToBegin();//指针回到头上
	for (int i = 0; i < num_file-1; i++)
	{
		CString filename;
		filename.Format(_T("%s\\%s_%d.%s"), foldName, infilename, i, infilename.Right(3));
		FILE* fpout = NULL;
		if ((fpout = fopen(filename, "w")) == NULL)
		{
			MessageBox(_T("打开 ") + filename + _T(" 失败"));
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
		MessageBox(_T("打开 ") + filename + _T(" 失败"));
		return 0;
	}
	while (fp.ReadString(str_Line))
	{
		fprintf(fpout, "%s\n", (PCSTR)(_bstr_t)(PCTSTR)str_Line);
	}
	fclose(fpout);
	fp.Close();

	MessageBox(_T("分割完毕"));
	return 0;
}


void C博士研究小助手View::OnComputedx()
{
	// TODO:  在此添加命令处理程序代码
	CString filter = _T("文本文件|*.txt|文本文件 DAT|*.dat|所有文件 (*.*)|*.*|");
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


int C博士研究小助手View::ComputeDx(CString filename, CString pathname,CString extname)
{
	CStdioFile fp;
	CString str_Line;
	CString infile = pathname + _T("\\") + filename;
	if (fp.Open(infile, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("打开 ") + infile + _T(" 失败"));
		return 0;
	}
	long TotalNum_Line = 0;
	while (fp.ReadString(str_Line))
	{
		TotalNum_Line++;
	}fp.SeekToBegin();

	struct Gravstruct
	{
		double x, y,Distance;//dDistance是相邻两点的点距
		CString otherdatastr;
	};
	Gravstruct* gravdata = new Gravstruct[TotalNum_Line];
	//fp.ReadString(str_Line);
	//sscanf(str_Line, "%lf%lf", &gravdata[0].x, &gravdata[0].y);
	//gravdata[0].otherdatastr = str_Line;
	//gravdata[0].Distance = 0;//第一个点的点距为0
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

	//输出到文件
	CString outfile = pathname + _T("\\") + filename.Left(filename.GetLength() - extname.GetLength()-1)+_T("_加点距.")+extname;
	FILE* fpout = NULL;
	if ((fpout = fopen(outfile,"w"))==NULL)
	{
		MessageBox(_T("打开 ")+outfile+_T(" 失败"));
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


void C博士研究小助手View::OnDensyorrarefy()
{
	MessageBox(_T("此功能已经在博士研究助手Matlab版本上实现了，请用matlab版本"));
	return;
	// TODO:  在此添加命令处理程序代码
	CString filter = _T("文本文件|*.txt|文本文件 DAT|*.dat|所有文件 (*.*)|*.*|");
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


int C博士研究小助手View::DensyOrRarefy(CString filename, CString pathname, CString extname)
{
	CStdioFile fp;
	CString str_Line;
	CString infile = pathname + _T("\\") + filename;
	if (fp.Open(infile, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("打开 ") + infile + _T(" 失败"));
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
	//读取一行数据根据空格或者Tab判断有多少列数据
	char seperatorchr;
	int datacolumnNum = 0;
	int num_repeat = 0;
	for (int i = 0; i < str_Line.GetLength(); i++)
	{
		sscanf(str_Line.Right(str_Line.GetLength()-i),"%c",&seperatorchr);
		
		if (seperatorchr == 32 || seperatorchr == 9)
		{
			//MessageBox(_T("分隔符"));
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
	//对话框显示信息，并提示输入点距
	CDensyOrRarefyDataDlg densyorrarefyDlg;
	densyorrarefyDlg.m_MeanDx = MeanDistance;
	densyorrarefyDlg.m_SetNewDx = MeanDistance;
	densyorrarefyDlg.m_DataColumnNum = datacolumnNum;
	if (densyorrarefyDlg.DoModal()!=IDOK)
	{
		return 0;
	}
	//获取设置的点距和列数
	MeanDistance = densyorrarefyDlg.m_MeanDx;
	datacolumnNum = densyorrarefyDlg.m_DataColumnNum;


	return 0;
}

int C博士研究小助手View::WriteGrdTonetCDF(CString path,CString grdfilename,CString extname)
{
	CString filename_nc = path + _T("\\") + grdfilename.Left(grdfilename.GetLength() - extname.GetLength()) + _T("nc");
	CString infilename = path +_T("\\")+ grdfilename;

	//--------------参数区--------------------
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
	//1. 读取grd数据
	GRDDATA_SURFER_MFC grddata = {};
	ifstream fin;
	fin.open(infilename);
	if (!fin)
	{
		MessageBox(_T("打开 ")+infilename + _T(" 失败"));
		return 0;
	}
	int d;
	grddata.Flag_bin = false;//开始当做文本文件读取
	fin >> grddata;
	fin.close();
	//CString str; str.Format(_T("nx:%d  ny:%d"), grddata.columns,grddata.rows); MessageBox(str);
	if (grddata.Flag_bin)//不是文本，即没有DSAA，则当做二进制读取，判断是否有DSBB
	{
		//MessageBox(_T("有可能是二进制文件"));
		ifstream fin_bin;
		fin_bin.open(infilename, std::ifstream::binary);//二进制方式打开
		if (!fin_bin)
		{
			MessageBox(_T("打开 ") + infilename + _T(" 失败"));
			return 0;
		}
		fin_bin >> grddata;
		fin_bin.close();
		if (!grddata.Flag_bin)//如果用二进制读取也没有DSBB，则不符合格式要求，退出
		{
			MessageBox(_T("文件格式不正确，既不是DSAA也不是DSBB"));
			return 0;
		}
	}
	//准备数据
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
	//2. 写入netCDF——创建nc文件
	
	if ((retval = nc_create((_bstr_t)filename_nc, NC_CLOBBER, &ncid)))
		ERR(retval);
	//3. 定义维度
	if ((retval = nc_def_dim(ncid, (_bstr_t)yaxisName, grddata.rows, &y_dimid)))
		ERR(retval);
	if ((retval = nc_def_dim(ncid, (_bstr_t)xaxisName, grddata.columns, &x_dimid)))
		ERR(retval);
	//4. 定义坐标轴变量
	if ((retval = nc_def_var(ncid, (_bstr_t)yaxisName, NC_DOUBLE, 1, &y_dimid, &y_varid)))
		ERR(retval);
	if ((retval = nc_def_var(ncid, (_bstr_t)xaxisName, NC_DOUBLE, 1, &x_dimid, &x_varid)))
		ERR(retval);
	
	//5. 定义坐标变量的属性
	if ((retval = nc_put_att_text(ncid, y_varid, (_bstr_t)UNITS, strlen((_bstr_t)yaxis_units), (_bstr_t)yaxis_units)))
		ERR(retval);
	if ((retval = nc_put_att_text(ncid, x_varid, (_bstr_t)UNITS, strlen((_bstr_t)xaxis_units), (_bstr_t)xaxis_units)))
		ERR(retval);
	
	//6. 定义netCDF变量
	int dims[2]; dims[0] = y_dimid; dims[1] = x_dimid;
	if ((retval = nc_def_var(ncid, data_name, NC_DOUBLE, dims_data,
		dims, &datavarid)))
		ERR(retval);
	//7. 定义数据属性
	if ((retval = nc_put_att_text(ncid, datavarid, (_bstr_t)UNITS,
		strlen((_bstr_t)data_units), (_bstr_t)data_units)))
		ERR(retval);
	//8. 结束定义
	if ((retval = nc_enddef(ncid)))
		ERR(retval);
	//9 写数据——坐标
	if ((retval = nc_put_var_double(ncid, x_varid, &x[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(ncid, y_varid, &y[0])))
		ERR(retval);
	//10 写数据——变量
	if ((retval = nc_put_var_float(ncid, datavarid, &grddata.data[0])))
		ERR(retval);
	//11. 关闭文件
	if ((retval = nc_close(ncid)))
		ERR(retval);
	//释放指针资源
	delete[] grddata.data;
	return 1;
}
int CALLBACK BrowseCallBackFun2(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:  //选择文件夹对话框初始化 
							//设置默认路径为lpData即'D:\' 
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		//在STATUSTEXT区域显示当前路径 
		::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, lpData);
		//设置选择文件夹对话框的标题 
		::SetWindowText(hwnd, TEXT("请选择数据存储路径"));
		break;
	case BFFM_SELCHANGED:   //选择文件夹变更时 
	{
		TCHAR pszPath[MAX_PATH];
		//获取当前选择路径 
		SHGetPathFromIDList((LPCITEMIDLIST)lParam, pszPath);
		//在STATUSTEXT区域显示当前路径 
		::SendMessage(hwnd, BFFM_SETSTATUSTEXT, TRUE, (LPARAM)pszPath);
	}
	break;
	}
	return 0;
}

void C博士研究小助手View::OnSurferMultipostmap()
{
	// 用C++获取一个目录下的所有需要的文件名，然后根据这些文件名写surfer script文件，然后执行
	CString path, extenName;
	vector<CString> filenamevector;
	vector<CString> pathnamevector;
	//浏览文件夹
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = this->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = NULL;
	bi.ulFlags = BIF_USENEWUI;
	bi.lpfn = BrowseCallBackFun2;     //回调函数 
	bi.lParam = (LPARAM)(LPCTSTR)m_temp_pathname;  //传给回调函数的参数,设置默认路径 
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
	//调用设置对话框
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
	//2. 调用函数得到这个目录下所有的文件名
	int fileNum = GetAllFileName(path, extenName, pathnamevector, filenamevector);
	if (fileNum == 0)
	{
		CString errorinfo;
		errorinfo.Format(_T("此目录下没有%s文件，请选择正确路径，在设置对话框里填入正确的后缀名"),extenName);
		MessageBox(errorinfo);
		return;
	}
	//创建文件夹存放新生成的供surfer调用的文件
	CString foldName = _T("\\Data文件");
	CString newpath = path+foldName;
	if (!PathIsDirectory(newpath))
	{
		::CreateDirectory(newpath, NULL);
	}


	//新建文件夹写入新的文件
	
	for (int i = 0; i < fileNum; i++)
	{
		
		CStdioFile fp;
		CString str_Line;
		if (fp.Open(pathnamevector[i], CFile::modeRead) == FALSE)
		{
			MessageBox(_T("读取打开文件失败： ") + pathnamevector[i]);
			return ;
		}
		vector<CString> datstrvector;
		while (fp.ReadString(str_Line))//读取文件中所有行，然后写入bln文件
		{
			datstrvector.push_back(str_Line);
		}fp.Close();
	
		//在同一文件中添加一个label列，label以文件名命名
		CString filename = newpath +_T("\\")+ filenamevector[i];
		ofstream blnoutf(filename); 
		if (!blnoutf)
		{
			MessageBox(_T("写入打开文件失败: ")+ filename);
			return;
		}
		for (int j = 0; j < datstrvector.size(); j++)
		{
			blnoutf << datstrvector[j] <<"\t"<<filenamevector[i].Left(filenamevector[i].GetLength()-1-extenName.GetLength()) << "\t" << j  << endl;
		}
		blnoutf.close();
	}
	//3. 写入surfer BAS文件
	CString basfilename = path + _T("\\批量生成PostMap.bas");
	
	ofstream surfBatOF(basfilename);
	if (!surfBatOF)
	{
		MessageBox(_T("打开文件错误: ")+ path + _T("\\批量生成PostMap.bas"));
		return;
	}
	//3. 循环写入BAS文件
	surfBatOF << "Sub Main" << endl;
		surfBatOF << "\tSet SurferApp=CreateObject(\"Surfer.Application\")" << endl;
		surfBatOF << "\tDim Doc As Object" << endl;
		//surfBatOF << "\tSurferApp.Visible=True" << endl;
		surfBatOF << "\tDim plot As Object" << endl;
		surfBatOF << "\tSet plot=SurferApp.Documents.Add(srfDocPlot)" << endl;
		surfBatOF << "\tSet MapFrame = plot.Shapes.AddBaseMap(\""<< baselayerdatafile <<"\")" << endl;
		surfBatOF << "\tSet BaseLayer= MapFrame.Overlays(1)" << endl;
		//surfBatOF << "\tBaseLayer.SymFrequency = 50" << endl;
		//循环添加剩余的map
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
	//关闭文件
	surfBatOF.close();

	//运行BAS文件
	ShellExecute(this->m_hWnd, "open", basfilename, "", "", SW_SHOW);//打开计算器
}


int C博士研究小助手View::GetAllFileName(CString path, CString extenName, vector<CString>& pathnamevector, vector<CString>& filenameVector)
{
	//计数器先清零
	int DataFileNum = 0;
	CFileFind finder;
	//路径
	CString strWildcard(path);
	strWildcard += _T("//*.*");
	// 开始查找第一个文件
	BOOL bWorking = finder.FindFile(strWildcard);
	filenameVector.clear();//使用向量前先清空
						   //判断循环，直到下一个不存在
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots()) //判断是否是文件
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
		//if (finder.IsDirectory()) //判断是否是文件夹 //路径
		//{
		//     CString str = finder.GetFilePath();
		//     //如果想获得子路径下的文件个数，可以继续调用该函数
		//     //GetFileCount(str);
		//}
	}
	finder.Close();
	return DataFileNum;
}
UINT Grd2netcdfThreadFunc(LPVOID ThreadParam)
{
	C博士研究小助手View * pView = (C博士研究小助手View *)ThreadParam;
	pView->Grd2netcdf();
	return 0;
}
void C博士研究小助手View::OnGrd2netcdf()
{
	// TODO:  在此添加命令处理程序代码
	if (IDOK == AfxMessageBox(_T("此功能Surfer13已经有了，另存为就可以\n还需要执行此功能吗"), MB_OK | MB_OKCANCEL))
	{
		AfxBeginThread((AFX_THREADPROC)Grd2netcdfThreadFunc, this);
	}
	
}

void C博士研究小助手View::OnGlobalmainmagnetic()
{
	if (m_CalGlobalMainMagDlg.DoModal() != IDOK)
	{
		return;
	}
	//保存文件
	CString outfilename;
	switch (m_CalGlobalMainMagDlg.m_ComponentType)
	{
	case COMPONENT_T:
		outfilename.Format(_T("地磁模型%d阶_%d阶_T.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_X:
		outfilename.Format(_T("地磁模型%d阶_%d阶_X.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_Y:
		outfilename.Format(_T("地磁模型%d阶_%d阶_Y.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_Z:
		outfilename.Format(_T("地磁模型%d阶_%d阶_Z.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_H:
		outfilename.Format(_T("地磁模型%d阶_%d阶_H.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_D:
		outfilename.Format(_T("地磁模型%d阶_%d阶_D.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
		break;
	case COMPONENT_I:
		outfilename.Format(_T("地磁模型%d阶_%d阶_I.grd"), m_CalGlobalMainMagDlg.m_StartOrder, m_CalGlobalMainMagDlg.m_EndOrder);
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
	//如果选择了GMT绘图则执行GMT绘图命令
	if (m_CalGlobalMainMagDlg.m_IsGMTplot)
	{
		CString outgrdfile = SaveFileDlg.GetPathName();
		CString extname = SaveFileDlg.GetFileExt();
		//1.1转换为nc格式
		WriteGrdTonetCDF(SaveFileDlg.GetFolderPath(), SaveFileDlg.GetFileName(), SaveFileDlg.GetFileExt());
		CString filename_nc = SaveFileDlg.GetFolderPath() + _T("\\") + SaveFileDlg.GetFileName().Left(SaveFileDlg.GetFileName().GetLength() - extname.GetLength()) + _T("nc");
		switch (m_CalGlobalMainMagDlg.m_GMTProjectionType)
		{
		case JN_PROJ:
		{
			//创建bat文件
			CString batfile = SaveFileDlg.GetFolderPath() + _T("\\JN投影.bat");
			ofstream ofbat(batfile);
			if (!ofbat)
			{
				MessageBox(_T("创建JN投影bat文件失败\n") + batfile);
				return;
			}
			//1.2 汇集作图命令
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
			//创建bat文件
			CString batfile = SaveFileDlg.GetFolderPath() + _T("\\JW投影.bat");
			ofstream ofbat(batfile);
			if (!ofbat)
			{
				MessageBox(_T("创建JW投影bat文件失败\n") + batfile);
				return;
			}
			//1.2 汇集作图命令
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
			ofbat << "REM 北极投影\n";
			ofbat << "psbasemap -R-180/180/45/90 -JG0/90/3i -Y10 -Ba30g30/a30g15 -O -K>> %psf%\n";
			ofbat << "grdimage %inf% -R -J -C%cptf% -O -K -E135 -V >> %psf%\n";
			ofbat << "gmt pscoast -R -J -A10000 -Wthinnest -Dc  -O -K >>%psf%\n";
			ofbat << "REM 南极投影\n";
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
		MessageBox(_T("计算完毕"));
	}
	
}

void C博士研究小助手View::Onnetcdf2dat()
{
	// TODO: 在此添加命令处理程序代码
	CString filter = _T("netCDF nc文件|*.nc|所有文件|*.*|");
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
	//1.1 写nc2dat的命令
	CString gmtcommandfile = pathname+_T("\\nc2dat.bat");
	CString datfilename = infilename.Left(infilename.GetLength()-extename.GetLength())+_T("dat");
	//CString grdfilename= infilename.Left(infilename.GetLength() - extename.GetLength()-1) + _T("_surf.grd");
	ofstream of_bat(gmtcommandfile);
	if (!of_bat)
	{
		MessageBox(_T("nc2bat时生成bat文件失败"));
		return;
	}
	else
	{
		of_bat << "gmt grd2xyz " << infilename << " > " << datfilename << "\n"<<"del "<< gmtcommandfile;
		of_bat.close();
	}
	//1.2 执行命令
	HINSTANCE hd;
	hd=ShellExecute(this->m_hWnd, "open", (_bstr_t)gmtcommandfile, "", "", SW_SHOW);//
	//1.3 将dat转换为grd
	
	
}


void C博士研究小助手View::OnGrdcut()
{
	AfxBeginThread((AFX_THREADPROC)GrdCutThreadFunc, this);
	
}


int C博士研究小助手View::Dat2Grd_Surfer()
{
	CString filter = _T("网格dat文件 dat|*.dat|所有文件|*.*|");
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
	//读取数据
	m_ProgressBar->SetPos(20);
	vector<vector<double>> grddata_vec;
	vector<double> rowvec;
	double x, y, z, xmin, xmax, ymin, ymax, zmin, zmax, x0, y0,x1,y1,x2,y2;
	FILE* fp;
	if ((fp = fopen((_bstr_t)datfile, "r")) == NULL)
	{
		AfxMessageBox(_T("dat2grd:打开") + datfile + _T("失败"));
		return 0;
	}
	fscanf(fp, "%lf%lf%lf", &x, &y, &z);
	x0 = x; y0 = y; x1 = x; y1 = y;
	xmin = x; xmax = x; ymin = y; ymax = y; zmin = z; zmax = z;
	fscanf(fp, "%lf%lf%lf", &x2, &y2, &z);
	UpdateData(true);
	//m_Progress.SetRange(0, m_rows);
	//其实对于一个网格的dat文件，转换为grd很简单，只需要获取列数和行数以，x，y最值以及第一个点的位置就行
	vector<int>colsvector;
	int cols = 0;
	rewind(fp);//回到文件头
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
	colsvector.push_back(cols);//最后一行
	grddata_vec.push_back(rowvec);
	fclose(fp);
	//赋值主要参数
	int rows = colsvector.size();
	cols = colsvector[0];
	int ArrangeType = 0;
	if (x1==x2)
	{
		//MessageBox(_T("以列为主"));
		if (x1==xmin)
		{
		//	MessageBox(_T("从左向右"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("从下往上"));
				ArrangeType = COL_LEFT2RIGHT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("从上往下"));
				ArrangeType = COL_LEFT2RIGHT_UP2DOWN;
			}
		}
		else
		{
		//	MessageBox(_T("从右向左"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("从下往上"));
				ArrangeType = COL_RIGHT2LEFT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("从上往下"));
				ArrangeType = COL_RIGHT2LEFT_UP2DOWN;
			}
		}
		
	}
	else if (y2==y1)
	{
	//	MessageBox(_T("以行为主"));
		if (x1==xmin)
		{
		//	MessageBox(_T("从左向右"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("从下往上"));
				ArrangeType = ROW_LEFT2RIGHT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("从上往下"));
				ArrangeType = ROW_LEFT2RIGHT_UP2DOWN;
			}
		}
		else
		{
		//	MessageBox(_T("从右向左"));
			if (y1 == ymin)
			{
		//		MessageBox(_T("从下往上"));
				ArrangeType = ROW_RIGHT2LEFT_DOWN2UP;
			}
			else
			{
		//		MessageBox(_T("从上往下"));
				ArrangeType = ROW_RIGHT2LEFT_UP2DOWN;
			}
		}
		
	}
	//CString str; str.Format(_T("%d  %d  \n%lf %lf\n%lf %lf\n%lf %lf\n"), colsvector[0], colsvector.size(), xmin, xmax, ymin, ymax, zmin, zmax); MessageBox(str);
	m_ProgressBar->SetPos(m_ProgressMax);
	//输出

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
		AfxMessageBox(_T("打开") + OutFileName + _T("失败"));
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
	MessageBox(_T("转换完毕"));
	
	return 0;
}

UINT Dat2GrdThreadFunc(LPVOID ThreadParam)
{
	C博士研究小助手View * pView = (C博士研究小助手View *)ThreadParam;
	pView->Dat2Grd_Surfer();
	return 0;
}
void C博士研究小助手View::OnDat2grd()
{
	AfxBeginThread((AFX_THREADPROC)Dat2GrdThreadFunc, this);
}


int C博士研究小助手View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码  AfxBeginThread((AFX_THREADPROC)UpdateFigureThrFunAfterRead,this);
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	m_ProgressBar= DYNAMIC_DOWNCAST(CMFCRibbonProgressBar, pMainFrm->m_wndRibbonBar.FindByID(ID_PROGRESS1));
	m_ProgressBar->SetRange(m_ProgressMin, m_ProgressMax);
	m_ProgressBar->SetPos(m_CurrentProgressPos);
	return 0;
}


void C博士研究小助手View::OnClr2cpt()
{
	// TODO: 在此添加命令处理程序代码
	if (m_Clr2CptDlg.DoModal()!=IDOK)
	{
		return;
	}
	double zmin = m_Clr2CptDlg.m_Zmin;
	double zmax = m_Clr2CptDlg.m_Zmax;
	//1.2 读取crl文件
	CString filter = _T("Surfer色标文件 clr|*.clr|所有文件|*.*|");
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


int C博士研究小助手View::Clr2Cpt(CString clrfile, CString cptfile, double zmin, double zmax)
{
	double zLength = fabs(zmin - zmax);
	struct CLRSTRUCT
	{
		double percent, value;
		int R, G, B, A;
	};
	//1.1 读取clr文件
	ifstream if_clr(clrfile);
	if (!if_clr)
	{
		MessageBox(_T("读取clr文件失败：")+clrfile);
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
	
	//1.2写入cpt文件
	ofstream of_cpt(cptfile);
	if (!of_cpt)
	{
		MessageBox(_T("写入cpt文件失败：") + cptfile);
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


void C博士研究小助手View::OnDatcut()
{
	// TODO: 在此添加命令处理程序代码
	AfxBeginThread((AFX_THREADPROC)DatCutThreadFunc, this);
}


int C博士研究小助手View::GmtCutnetCDF(CString infilename, CString outfilename, double xmin, double xmax, double ymin, double ymax)
{
	CString gmtcommandfile =  _T("grdcut.bat");
	
	ofstream of_bat(gmtcommandfile);
	if (!of_bat)
	{
		MessageBox(_T("nc2bat时生成bat文件失败"));
		return 0;
	}
	else
	{
		of_bat << "gmt grdcut " << infilename << " -G" << outfilename << " -R" << xmin << "/"<< xmax << "/" << ymin << "/" << ymax << " -V\n"
			<< "del *.history " << gmtcommandfile;
		of_bat.close();
	}
	//1.2 执行命令
	ShellExecute(this->m_hWnd, "open", (_bstr_t)gmtcommandfile, "", "", SW_SHOW);//
	return 0;
}


int C博士研究小助手View::GrdCut_Surfer(CString infilename, CString outfilename, double xmin, double xmax, double ymin, double ymax)
{
	//1.1 读取grd数据
	char dsaa[20];
	CString dsaaStr;
	int rows,cols;
	double bounds[6];
	ifstream fin_grd(infilename);
	if (!fin_grd)
	{
		MessageBox(_T("读取grd数据文件失败: ")+infilename);
		return 0;
	}
	fin_grd.getline(dsaa, 20);
	dsaaStr = (PCSTR)(_bstr_t)(PCTSTR)dsaa;
	if (dsaaStr.Left(4)!=_T("DSAA"))
	{
		MessageBox(_T("您选择的文件不是Surfer Text Grid文件\n")+infilename);
		fin_grd.close();
		return 0;
	}
	
	//读取头信息
	fin_grd >> cols >> rows;
	for (int i = 0; i < 6; i++)
	{
		fin_grd >> bounds[i];
	}
	//1.2 根据给定的范围确定输出的索引号
	double dx = (bounds[1] - bounds[0]) / (cols - 1);
	double dy = (bounds[3] - bounds[2]) / (rows - 1);
	int index_xmin, index_xmax, index_ymin, index_ymax;
	//1.2.1 x最小值
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
			strinfo.Format(_T("对不起，您截取的区域在数据区域之外\nxmax_data:%lf;xmin_cut:%lf"), bounds[1], xmin);
			MessageBox(strinfo);
			return 0;
		}
	}
	//1.2.2 x最大值
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
			strinfo.Format(_T("对不起，您截取的区域在数据区域之外\nxmin_data:%lf;xmax_cut:%lf"), bounds[0], xmax);
			MessageBox(strinfo);
			return 0;
		}
	}
	//1.2.3 y最小值
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
			strinfo.Format(_T("对不起，您截取的区域在数据区域之外\nymax_data:%lf;ymin_cut:%lf"), bounds[3], ymin);
			MessageBox(strinfo);
			return 0;
		}
	}
	//1.2.4 y最大值
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
			strinfo.Format(_T("对不起，您截取的区域在数据区域之外\nymin_data:%lf;ymax_cut:%lf"), bounds[2], ymax);
			MessageBox(strinfo);
			return 0;
		}
	}
	//CString str; str.Format(_T("%d %d\n%d  %d"), index_xmin, index_xmax, index_ymin, index_ymax); MessageBox(str);
	
	//1.3 根据给定范围申请二维数组
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
	//1.3 输出grd到文件
	//计算最大值最小值
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
		MessageBox(_T("输出grd数据文件失败: ") + infilename);
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
	//释放指针
	for (int i = 0; i < newrows; i++)
	{
		delete[] data[i];
	}delete[] data;

	MessageBox(_T("切取成功"));
	m_ProgressBar->SetPos(0);
	return 0;
}
UINT DatCutThreadFunc(LPVOID ThreadParam)
{
	C博士研究小助手View * pView = (C博士研究小助手View *)ThreadParam;
	pView->DatCut();
	return 0;
}

UINT GrdCutThreadFunc(LPVOID ThreadParam)
{
	C博士研究小助手View * pView = (C博士研究小助手View *)ThreadParam;
	pView->GrdCut();
	return 0;
}
int C博士研究小助手View::GrdCut()
{
	//1.1 选择待切去的文件
	CString filter = _T("netCDF nc文件|*.nc|GRD Surfer Txt Grid (*.grd)|*.grd|所有文件|*.*|");
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
	//1.2 选择区域大小
	if (m_AreaRangeSelDlg.DoModal() != IDOK)
	{
		return 0;
	}
	CString cuttedFilename;
	
	if (extename == _T("nc"))
	{
		//1.3 写grdcut的命令
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
		MessageBox(_T("格式错误，请选择nc或surfer 的grd Text格式"));
		return 0;
	}
}


int C博士研究小助手View::Grd2netcdf()
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
		if(WriteGrdTonetCDF(OpenFileDlg.GetFolderPath(), OpenFileDlg.GetFileName(), OpenFileDlg.GetFileExt()))MessageBox(_T("转换成功"));

	}
	return 0;
}


int C博士研究小助手View::DatCut()
{
	//1.1 选择待切去的文件
	CString filter = _T("Dat 文件|*.dat|Dat Txt文件 (*.txt)|*.txt||");
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
	//1.2 选择区域大小
	if (m_AreaRangeSelDlg.DoModal() != IDOK)
	{
		return 0;
	}
	double xmin = m_AreaRangeSelDlg.m_Xmin, xmax = m_AreaRangeSelDlg.m_Xmax, ymin = m_AreaRangeSelDlg.m_Ymin, ymax = m_AreaRangeSelDlg.m_Ymax;
	CString cuttedFilename;
	cuttedFilename.Format(_T("%s_%.2f_%.2f_%.2f_%.2f.%s"), infilename.Left(infilename.GetLength() - extename.GetLength() - 1),
		m_AreaRangeSelDlg.m_Xmin, m_AreaRangeSelDlg.m_Xmax, m_AreaRangeSelDlg.m_Ymin, m_AreaRangeSelDlg.m_Ymax,extename);

	//切去dat数据
	CStdioFile fin_dat;//输入文件
	CString str_Line;
	if (fin_dat.Open(infilename, CFile::modeRead) == FALSE)
	{
		MessageBox(_T("读取dat数据文件失败: ") + infilename);
		return 0;
	}
	ofstream fout_cutteddat(cuttedFilename);//输出文件
	if (!fout_cutteddat)
	{
		MessageBox(_T("写入dat数据文件失败: ") + cuttedFilename);
		return 0;
	}
	//循环判断读取并输出
	double x, y;
	while (fin_dat.ReadString(str_Line))
	{
		sscanf((_bstr_t)str_Line,"%lf%lf", &x, &y);

		if (x <= xmax && x >= xmin && y <= ymax && y >= ymin)
		{
			fout_cutteddat << str_Line << "\n";
		}
	}
	//关闭文件
	fin_dat.Close();
	fout_cutteddat.close();
	MessageBox(_T("切去成功"));
	return 0;
}


void C博士研究小助手View::OnClrtrans()
{
	// TODO: 在此添加命令处理程序代码
}
