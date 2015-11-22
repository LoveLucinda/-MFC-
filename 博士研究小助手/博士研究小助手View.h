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

// 博士研究小助手View.h : C博士研究小助手View 类的接口
//

#pragma once
#include "vector"
using namespace std;
#include "netcdf.h"
#include "GRD_surfer.h"
#include "MultiSurfPostMapDlg.h"
#include "CalGlobalMainMagneticDlg.h"
#include "AreaRangeSelectDlg.h"
#include "Clr2CptDlg.h"

#define WM_PROGRESS_MESSAGE WM_USER+1		//更新进度条状态
#define WM_PROGRESS_END		WM_USER+2		//
#define WM_PROGRESS_Range		WM_USER+3	//设置进度条范围

//定义网格数据的dat排列方式
#define ROW_LEFT2RIGHT_UP2DOWN 1
#define ROW_LEFT2RIGHT_DOWN2UP 2
#define ROW_RIGHT2LEFT_UP2DOWN 3
#define ROW_RIGHT2LEFT_DOWN2UP 4
#define COL_LEFT2RIGHT_UP2DOWN 5
#define COL_LEFT2RIGHT_DOWN2UP 6
#define COL_RIGHT2LEFT_UP2DOWN 7
#define COL_RIGHT2LEFT_DOWN2UP 8

//定义grd结构体(别的地方使用可以复制)
//surfer的二进制grd文件读写
struct GRDDATA_SURFER_MFC
{
	int rows, columns;
	double bounds[6];
	float* data = NULL;
	bool Flag_bin = false;
	CMFCRibbonProgressBar* progress;

	friend ostream &operator<<(ostream &out, GRDDATA_SURFER_MFC value);			//操作符重载：输出
																					//friend istream &operator<<(istream &in, GRDDATA_SURFER value);			//操作符重载：输入
	friend ifstream &operator>>(ifstream &in, GRDDATA_SURFER_MFC& value);			//操作符重载：输入
};
//板块边界数据结构体
struct PlateBoundaryStructuct
{
	CString name;
	vector<double> lon, lat;
};

class C博士研究小助手View : public CView
{
protected: // 仅从序列化创建
	C博士研究小助手View();
	DECLARE_DYNCREATE(C博士研究小助手View)

// 特性
public:
	C博士研究小助手Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~C博士研究小助手View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPlantboundaryfilter();
	int ExtractGlobalPlateBoundary(CString infilename, CString pathname);
	afx_msg void OnCutlargefile();
	int CutLargeFile(CString filename, CString pathname);
	afx_msg void OnComputedx();
	int ComputeDx(CString filename, CString pathname, CString extname);
	afx_msg void OnDensyorrarefy();
	int DensyOrRarefy(CString filename, CString pathname, CString extname);
	int WriteGrdTonetCDF(CString path, CString grdfilename, CString extname);
	int ReadGrdFromnetCDF(CString path, CString grdfilename, CString extname);
	afx_msg void OnSurferMultipostmap();
	int GetAllFileName(CString path, CString extenName, vector<CString>& pathnamevector, vector<CString>& filenameVector);
	CString m_temp_pathname;
	CMultiSurfPostMapDlg m_MultiSurfPostMapDlg;
	afx_msg void OnGlobalmainmagnetic();
	afx_msg void OnGrd2netcdf();
	CCalGlobalMainMagneticDlg m_CalGlobalMainMagDlg;
	afx_msg void Onnetcdf2dat();
	afx_msg void OnGrdcut();
	CAreaRangeSelectDlg m_AreaRangeSelDlg;
	int Dat2Grd_Surfer();
	afx_msg void OnDat2grd();
	CMFCRibbonProgressBar* m_ProgressBar;//进度条
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//自定义消息映射函数
	//afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnSetProgressRange(WPARAM wParam, LPARAM lParam);
	int m_ProgressMin;
	int m_ProgressMax;
	int m_CurrentProgressPos;

	afx_msg void OnClr2cpt();
	CClr2CptDlg m_Clr2CptDlg;
	int Clr2Cpt(CString clrfile, CString cptfile, double zmin, double zmax);
	afx_msg void OnDatcut();
	int GmtCutnetCDF(CString infilename, CString outfilename, double xmin, double xmax, double ymin, double ymax);
	int GrdCut_Surfer(CString infilename, CString outfilename, double xmin, double xmax, double ymin, double ymax);
	int GrdCut();
	int Grd2netcdf();
	//CWinThread* m_CurrentThread;//用一个线程变量记录当前开启的用户线程，如果界面关掉，记得杀掉这个线程
	int DatCut();
	afx_msg void OnClrtrans();
	int Clr2Clr(CString clrfile1, CString clrfile2, double zmin, double zmax, double zmin2, double zmax2);
};

#ifndef _DEBUG  // 博士研究小助手View.cpp 中的调试版本
inline C博士研究小助手Doc* C博士研究小助手View::GetDocument() const
   { return reinterpret_cast<C博士研究小助手Doc*>(m_pDocument); }
#endif

