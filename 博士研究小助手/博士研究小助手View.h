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

// ��ʿ�о�С����View.h : C��ʿ�о�С����View ��Ľӿ�
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

#define WM_PROGRESS_MESSAGE WM_USER+1		//���½�����״̬
#define WM_PROGRESS_END		WM_USER+2		//
#define WM_PROGRESS_Range		WM_USER+3	//���ý�������Χ

//�����������ݵ�dat���з�ʽ
#define ROW_LEFT2RIGHT_UP2DOWN 1
#define ROW_LEFT2RIGHT_DOWN2UP 2
#define ROW_RIGHT2LEFT_UP2DOWN 3
#define ROW_RIGHT2LEFT_DOWN2UP 4
#define COL_LEFT2RIGHT_UP2DOWN 5
#define COL_LEFT2RIGHT_DOWN2UP 6
#define COL_RIGHT2LEFT_UP2DOWN 7
#define COL_RIGHT2LEFT_DOWN2UP 8

//����grd�ṹ��(��ĵط�ʹ�ÿ��Ը���)
//surfer�Ķ�����grd�ļ���д
struct GRDDATA_SURFER_MFC
{
	int rows, columns;
	double bounds[6];
	float* data = NULL;
	bool Flag_bin = false;
	CMFCRibbonProgressBar* progress;

	friend ostream &operator<<(ostream &out, GRDDATA_SURFER_MFC value);			//���������أ����
																					//friend istream &operator<<(istream &in, GRDDATA_SURFER value);			//���������أ�����
	friend ifstream &operator>>(ifstream &in, GRDDATA_SURFER_MFC& value);			//���������أ�����
};
//���߽����ݽṹ��
struct PlateBoundaryStructuct
{
	CString name;
	vector<double> lon, lat;
};

class C��ʿ�о�С����View : public CView
{
protected: // �������л�����
	C��ʿ�о�С����View();
	DECLARE_DYNCREATE(C��ʿ�о�С����View)

// ����
public:
	C��ʿ�о�С����Doc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~C��ʿ�о�С����View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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
	CMFCRibbonProgressBar* m_ProgressBar;//������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�Զ�����Ϣӳ�亯��
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
	//CWinThread* m_CurrentThread;//��һ���̱߳�����¼��ǰ�������û��̣߳��������ص����ǵ�ɱ������߳�
	int DatCut();
	afx_msg void OnClrtrans();
	int Clr2Clr(CString clrfile1, CString clrfile2, double zmin, double zmax, double zmin2, double zmax2);
};

#ifndef _DEBUG  // ��ʿ�о�С����View.cpp �еĵ��԰汾
inline C��ʿ�о�С����Doc* C��ʿ�о�С����View::GetDocument() const
   { return reinterpret_cast<C��ʿ�о�С����Doc*>(m_pDocument); }
#endif

