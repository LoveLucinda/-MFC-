// 根据球谐系数解算磁场分布
#include "MagCalc_Version_1_0.h"
#include <math.h>
#include <stdio.h>
#include <memory.h>

// #prRgma comment (lib,"..\\LegendreSrc\\debug\\LegendreSrc.lib")
// #prRgma comment (lib, "dfor.lib")
// #prRgma comment (lib, "Legendre.lib")


// 申明外部函数
extern "C"
{
	void _stdcall PlmSchmidt_d1(double *p, double *dp, int lmax, double z, int csphase, int cnorm);
}

// 定义常数圆周率
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// 弧度转换为角度
#define RAD2DEG(rRd)    ((rRd)*(180.0L/M_PI))
// 角度转换为弧度
#define DEG2RAD(deg)    ((deg)*(M_PI/180.0L))

// 定义Ellipsoid结构体
typedef struct _Ellipsoid
{
	/*semi-major axis of the ellipsoid*/
	double a;
	/*semi-minor axis of the ellipsoid*/
	double b;
	/* flattening */
	double fla;
	/*first eccentricity squared */
	double epssq;
	/* first eccentricity */
	double eps;
	/* mean radius of  ellipsoid*/
	double re;
}Ellipsoid;
// The Schmidt-semi normalized associated Legendre functions
bool Legendre (double *Pcup, double *dPcup, double x, int nMax)
/*	This function evaluates all of the Schmidt-semi normalized associated Legendre
functions up to degree nMax. The functions are initially scaled by
10^280 sin^m in order to minimize the effects of underflow at large m
near the poles (see Holmes and Featherstone 2002, J. Geodesy, 76, 279-299).
Note that this function performs the same operRtion as WMM_PcupLow.
However this function also can be used for high degree (large nMax) models.

Calling ParRmeters:
	INPUT
		nMax:	 Maximum spherical harmonic degree to compute.
		x:		 cos(colatitude) or sin(latitude).

	OUTPUT
		Pcup:	A vector of all associated Legendgre polynomials evaluated at
				x up to nMax. The lenght must by greater or equal to (nMax+1)*(nMax+2)/2.
	    dPcup:  Derivative of Pcup(x) with respect to latitude

	CALLS : none
Notes:

Adopted from the FORTrRN code written by Mark Wieczorek September 25, 2005.

Manoj Nair, Nov, 2009 Manoj.C.Nair@Noaa.Gov

Change from the previous version
The prevous version computes the derivatives as
dP(n,m)(x)/dx, where x = sin(latitude) (or cos(colatitude) ).
However, the WMM Geomagnetic routines requires dP(n,m)(x)/dlatitude.
Hence the derivatives are multiplied by sin(latitude).
Removed the options for CS phase and normalizations.

Note: In geomagnetism, the derivatives of ALF are usually found with
respect to the colatitudes. Here the derivatives are found with respect
to the latitude. The difference is a sign reversal for the derivative of
the Associated Legendre Functions.

The derivates can't be computed for latitude = |90| degrees.
*/
{
	double  pm2, pm1, pmm, plm, rescalem, z, scalef;
	double *f1, *f2, *PreSqr;
	f1=f2=PreSqr=NULL;
	int k, kstart, m, n, NumTerms;
//	int astat[3];

	NumTerms = ( ( nMax + 1 ) * ( nMax + 2 ) / 2 );

	if ( fabs ( x ) == 1.0 )
	{
		printf ( "Error in PcupHigh: derivative cannot be calculated at poles\n" );
		return false;
	}


	f1 = new double[  NumTerms+1 ]; //(double *) 	malloc	( (NumTerms +1) * sizeof ( double ) );
	if ( f1 == NULL )
	{
		//printf("error allocating in WMM_PcupHigh\n");
		return false;
	}


	PreSqr = new double[NumTerms+1 ]; //(double *) 	malloc	( (NumTerms +1) * sizeof ( double ) );

	if ( PreSqr == NULL )
	{
		//printf("error allocating in WMM_PcupHigh\n");
		return false;
	}

	f2 = new double[NumTerms+1 ]; //(double *) 	malloc	( (NumTerms +1) * sizeof ( double ) );

	if ( f2 == NULL )
	{
		//printf("error allocating in WMM_PcupHigh\n");
		return false;
	}

	scalef = 1.0e-280;

	for ( n = 0 ; n <= 2*nMax+1 ; ++n )
	{
		PreSqr[n] = sqrt ( ( double ) ( n ) );
	}

	k = 2;

	for ( n=2 ; n<=nMax ; n++ )
	{
		k = k + 1;
		f1[k] = ( double ) ( 2*n-1 ) / ( double ) ( n );
		f2[k] = ( double ) ( n-1 ) / ( double ) ( n );
		for ( m=1 ; m<=n-2 ; m++ )
		{
			k = k+1;
			f1[k] = ( double ) ( 2*n-1 ) / PreSqr[n+m] / PreSqr[n-m];
			f2[k] = PreSqr[n-m-1] * PreSqr[n+m-1] / PreSqr[n+m] / PreSqr[n-m];
		}
		k = k + 2;
	}

	/*z = sin (geocentric latitude) */
	z = sqrt ( ( 1.0-x ) * ( 1.0+x ) );
	pm2  = 1.0;
	Pcup[0] = 1.0;
	dPcup[0] = 0.0;
	if ( nMax == 0 )
		return false;
	pm1  		= x;
	Pcup[1] 	= pm1;
	dPcup[1] 	= z;
	k = 1;

	for ( n = 2; n <= nMax; n++ )
	{
		k = k+n;
		plm = f1[k]*x*pm1-f2[k]*pm2;
		Pcup[k] = plm;
		dPcup[k] = ( double ) ( n ) * ( pm1 - x * plm ) / z;
		pm2  = pm1;
		pm1  = plm;
	}

	pmm = PreSqr[2]*scalef;
	rescalem = 1.0/scalef;
	kstart = 0;

	for ( m = 1; m <= nMax - 1; ++m )
	{
		rescalem = rescalem*z;

		/* Calculate Pcup(m,m)*/
		kstart = kstart+m+1;
		pmm =  pmm * PreSqr[2*m+1] / PreSqr[2*m];
		Pcup[kstart] = pmm*rescalem / PreSqr[2*m+1];
		dPcup[kstart] = - ( ( double ) ( m ) * x * Pcup[kstart] / z );
		pm2 = pmm/PreSqr[2*m+1];
		/* Calculate Pcup(m+1,m)*/
		k = kstart+m+1 ;
		pm1 = x * PreSqr[2*m+1] * pm2;
		Pcup[k] = pm1*rescalem;
		dPcup[k] =   ( ( pm2*rescalem ) * PreSqr[2*m+1] - x * ( double ) ( m+1 ) * Pcup[k] ) / z;
		/* Calculate Pcup(n,m)*/
		for ( n = m+2; n <= nMax; ++n )
		{
			k = k+n;
			plm  = x*f1[k]*pm1-f2[k]*pm2;
			Pcup[k] = plm*rescalem;
			dPcup[k] = ( PreSqr[n+m] * PreSqr[n-m] * ( pm1 * rescalem ) - ( double ) ( n ) * x * Pcup[k] ) / z;
			pm2  = pm1;
			pm1  = plm;
		}
	}

	/* Calculate Pcup(nMax,nMax)*/
	rescalem = rescalem*z;
	kstart = kstart+m+1;
	pmm =  pmm  / PreSqr[2*nMax];
	Pcup[kstart] = pmm * rescalem;
	dPcup[kstart] = - ( double ) ( nMax ) * x * Pcup[kstart] / z;
	delete[]f1; //free(f1);
	delete[]PreSqr; //free(PreSqr);
	delete[]f2; //free(f2);

	return true ;
} /* WMM_PcupHigh */

// typedef struct _CoordGeodetic
// {
// 	double lamda;// longitude
// 	double theta; // geodetic latitude
// 	double HeightAboveEllipsoid; // height above the ellipsoid (HaE)
// }CoordGeodetic;
// 	
// typedef struct _CoordSpherical
// {
// 	double lamda;/* longitude*/
// 	double theta;/* geocentric latitude*/
// 	double r;  /* distance from the center of the ellipsoid*/
// }CoordSpherical;

// 设置默认Ellipsoid结构体的参数
void SetDefaultEllipsoid(Ellipsoid &Ellip)
{
	// 地球的参数
	Ellip.a=6378.136460;
	Ellip.b=6356.7523142;
	Ellip.fla=1.0/298.257223563;
	Ellip.eps=sqrt(1- (Ellip.b * Ellip.b) / (Ellip.a * Ellip.a));
	Ellip.epssq=Ellip.eps * Ellip.eps; //WGS84
	Ellip.re=6371.2;/* Earth's radius */
}

// 大地坐标转换为地心坐标
void GeodeticToSpherical(double theta, double HeightAboveEllipsoid, double &theta1, double &r, Ellipsoid Ellip)
/* Converts Geodetic coordinates to Spherical coordinates
INPUT   Ellip  data  structure:
				double a; semi-major axis of the ellipsoid
				double b; semi-minor axis of the ellipsoid
				double fla;  flattening
				double epssq; first eccentricity squared

		CoordGeodetic:  
				double theta; ( geodetic latitude )
				double HeightAboveEllipsoid; ( height above the WGS84 ellipsoid (HaE) )

OUTPUT	CoordSpherical:
				double theta1; ( geocentric latitude )
				double r;  	  ( distance from the center of the ellipsoid)*/
{
	double CosLat, SinLat, rc, xp, zp; /*all local variables */

/*
** Convert geodetic coordinates, (defined by the WGS-84
** reference ellipsoid), to Earth Centered Earth Fixed Cartesian
** coordinates, and then to spherical coordinates.
*/

	CosLat = cos(DEG2RAD(theta));
	SinLat = sin(DEG2RAD(theta));

/* compute the local rRdius of curvature on the WGS-84 reference ellipsoid */

	rc = Ellip.a / sqrt(1.0 - Ellip.epssq * SinLat * SinLat);

/* compute ECEF Cartesian coordinates of specified point (for longitude=0) */

	xp = (rc + HeightAboveEllipsoid) * CosLat;
	zp = (rc*(1.0 - Ellip.epssq) + HeightAboveEllipsoid) * SinLat;

/* compute spherical rRdius and angle lambda and phi of specified point */

	r = sqrt(xp * xp + zp * zp);
	theta1 = RAD2DEG(asin(zp / r));     /* geocentric latitude */

}

void AverageFilter(double Fai0, long n,double* W);

// 主函数
int CalMag(CString cofFileName,int ComponentType, int n_start, int n_end,double lamda0, double lamda1, double dlamda, double theta0, double theta1, double dtheta,CString outfilename)
{
	FILE *fp;
	int i,j,k;

	int csphase=1;
	int cnorm=0;

	//最大阶次
	int lmax=13;
	// 起始阶数
	//int n_start=1;
	//// 终止阶数
	//int n_end=13;

	//球谐系数个数
	int num=(lmax+1)*(lmax+2)/2-1; 
	int nn,mm;
	double *g=new double[num];
	double *h=new double[num];

	double *p =new double[num+1];
	double *dp=new double[num+1];	
	
	// 球谐系数文件名
	fp=fopen((_bstr_t)cofFileName, "r");
	if (!fp)
	{
		MessageBox(NULL, _T("打开系数文件:") + cofFileName + _T("失败"), _T("全球主磁场计算"), MB_OK);
		return 0;
	}

	for (i=0; i<num; ++i)
	{
		fscanf(fp, "%d %d %lf %lf", &nn, &mm, &g[i], &h[i]);
	}
	fclose(fp);

	////起始经度
	//double lamda0=78; 
	////终止经度
	//double lamda1=92; 
	////经度间隔
	//double dlamda=0.05; 

	int nlamda=(int)((lamda1-lamda0)/dlamda+1);

	////起始纬度
	//double theta0=38; 
	////终止纬度
	//double theta1=52; 
	////纬度间隔
	//double dtheta=0.05; 

	int ntheta=(int)((theta1-theta0)/dtheta+1);

	int itheta, ilamda, itl;

	int numCoord=nlamda*ntheta;

	double *X=new double[numCoord];
	double *Y=new double[numCoord];
	double *Z=new double[numCoord];
	double *H=new double[numCoord];
	double *F=new double[numCoord];
	double *I=new double[numCoord];
	double *D=new double[numCoord];
	double *theta=new double[ntheta];
	double *lamda=new double[nlamda];
	double *outComponent = new double[numCoord];

	memset(X,0, numCoord*sizeof(double));
	memset(Y,0, numCoord*sizeof(double));
	memset(Z,0, numCoord*sizeof(double));
	memset(H,0, numCoord*sizeof(double));
	memset(F,0, numCoord*sizeof(double));

	double xtmp, ytmp, ztmp;
	//余纬度
	double ctheta; 
	double thetatmp;
	double sinctheta, cosctheta;
	double sinmlamda, cosmlamda;
	//高程km
	double HeightAboveEllipsoid=0.0; 
	double r, rR;
	double rpow;
	Ellipsoid Ellip;
	bool bDefaultEllipsoid=true;
	bool bGeodeticToSpherical=false;

	if (bDefaultEllipsoid)
	{
		SetDefaultEllipsoid(Ellip);
	}

	// 20111010修改
	// 计算滤波因子(20111008)
	double Fai,Fai0;
	Fai=10.0;
	Fai0=Fai*M_PI/180.0;
	printf("\nFai= %.6lf\n",Fai);

	double *W;
    W   = new double[lmax+1];	
	W[0]=1;

	for(i=1; i<=lmax; ++i)
	{
		W[i]=1;
	}

	//计算高斯滤波因子
	if(Fai>0)
	{
		AverageFilter(Fai0,lmax,W);
	}

	// 显示计算进程
   // printf("\nCalculating process : 1234");

	long nProcess = 0;

	for (itheta=0; itheta<ntheta; ++itheta)
	{
		nProcess = (long)((itheta+1)*100/ntheta);
       // printf("\b\b\b\b%2ld%% ", nProcess);

		theta[itheta]=theta0+itheta*dtheta;

		if (bGeodeticToSpherical)
		{
			GeodeticToSpherical(theta[itheta], HeightAboveEllipsoid, ctheta, r, Ellip);		
			ctheta=90-ctheta;
		}
		else
		{	
			//r=Ellip.re+HeightAboveEllipsoid;
			// 20111010修改
			double a0=Ellip.a+HeightAboveEllipsoid;
			double b0=a0-a0*Ellip.fla;
			r=sqrt(a0*a0*cos(theta[itheta]*M_PI/180.0)*cos(theta[itheta]*M_PI/180.0)+b0*b0*sin(theta[itheta]*M_PI/180.0)*sin(theta[itheta]*M_PI/180.0));
			ctheta=90-theta[itheta];
		}

		rR=Ellip.re/r;		
		sinctheta=sin(DEG2RAD(ctheta));
		cosctheta=cos(DEG2RAD(ctheta)); 
//		PlmSchmidt_d1(p, dp, lmax, cosctheta, csphase, cnorm);
		Legendre ( p, dp, cosctheta, lmax );

		for (ilamda=0; ilamda<nlamda; ++ilamda)
		{
			lamda[ilamda]=lamda0+ilamda*dlamda;
			itl=itheta*nlamda+ilamda;
			xtmp=ytmp=ztmp=0;

			for (i=n_start; i<=n_end; ++i)
			{
				rpow=pow(rR, i+2);
				for (j=0; j<=i; ++j)
				{
					k=i*(i+1)/2+j-1;					
					sinmlamda=sin(j*DEG2RAD(lamda[ilamda]));
					cosmlamda=cos(j*DEG2RAD(lamda[ilamda]));
					// 无滤波
					xtmp+=-rpow*(g[k]*cosmlamda+h[k]*sinmlamda)*dp[k+1];//*(-sinctheta);
					ytmp+=rpow*(g[k]*sinmlamda-h[k]*cosmlamda)*p[k+1]*j/sinctheta;
					ztmp+=-rpow*(g[k]*cosmlamda+h[k]*sinmlamda)*p[k+1]*(i+1);

					// 滤波
					//xtmp+=-rpow*(g[k]*cosmlamda+h[k]*sinmlamda)*W[i]*dp[k+1];//*(-sinctheta);
					//ytmp+=rpow*(g[k]*sinmlamda-h[k]*cosmlamda)*W[i]*p[k+1]*j/sinctheta;
					//ztmp+=-rpow*(g[k]*cosmlamda+h[k]*sinmlamda)*W[i]*p[k+1]*(i+1);
				}
			}

			thetatmp=DEG2RAD(90-theta[itheta]-ctheta);
			X[itl]=xtmp*cos(thetatmp)-ztmp*sin(thetatmp);
			Y[itl]=ytmp;
			Z[itl]=xtmp*sin(thetatmp)+ztmp*cos(thetatmp);
			H[itl]=sqrt(X[itl]*X[itl]+Y[itl]*Y[itl]);
			F[itl]=sqrt(X[itl]*X[itl]+Y[itl]*Y[itl]+Z[itl]*Z[itl]);
			D[itl]=RAD2DEG(atan2(Y[itl], X[itl]));
			I[itl]=RAD2DEG(atan2(Z[itl], H[itl]));
		}
	}
	//根据选择输出分量
	double Fmin, Fmax;
	switch (ComponentType)
	{
	case COMPONENT_T:
	{
		Fmin = F[0], Fmax = F[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				//fprintf(fp, "%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.3lf\t%.3lf\n", lamda[ilamda], theta[itheta], 
				//X[itl], Y[itl], Z[itl], H[itl], F[itl], D[itl], I[itl]);
				//fprintf(fp, "%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.5lf\t%.5lf\n", lamda[ilamda], theta[itheta], 
				//X[itl], Y[itl], Z[itl], H[itl], F[itl]);
				if (F[itl]>Fmax)
				{
					Fmax = F[itl];
				}
				if (F[itl]<Fmin)
				{
					Fmin = F[itl];
				}
				outComponent[itl] = F[itl];
			}

		}
	}
		break;
	case COMPONENT_X:
	{
		Fmin = X[0], Fmax = X[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				if (X[itl]>Fmax)
				{
					Fmax = X[itl];
				}
				if (X[itl]<Fmin)
				{
					Fmin = X[itl];
				}
				outComponent[itl] =X[itl];
			}

		}
	}
		break;
	case COMPONENT_Y:
	{
		Fmin = Y[0], Fmax = Y[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				if (Y[itl]>Fmax)
				{
					Fmax = Y[itl];
				}
				if (Y[itl]<Fmin)
				{
					Fmin = Y[itl];
				}
				outComponent[itl] = Y[itl];
			}

		}
	}
		break;
	case COMPONENT_Z:
	{
		Fmin = Z[0], Fmax = Z[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				if (Z[itl]>Fmax)
				{
					Fmax = Z[itl];
				}
				if (Z[itl]<Fmin)
				{
					Fmin = Z[itl];
				}
				outComponent[itl] = Z[itl];
			}

		}
	}
		break;
	case COMPONENT_H:
	{
		Fmin = H[0], Fmax = H[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				if (H[itl]>Fmax)
				{
					Fmax = H[itl];
				}
				if (H[itl]<Fmin)
				{
					Fmin = H[itl];
				}
				outComponent[itl] = H[itl];
			}

		}
	}
		break;
	case COMPONENT_D:
	{
		Fmin = D[0], Fmax = D[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				if (D[itl]>Fmax)
				{
					Fmax = D[itl];
				}
				if (D[itl]<Fmin)
				{
					Fmin = D[itl];
				}
				outComponent[itl] = D[itl];
			}

		}
	}
		break;
	case COMPONENT_I:
	{
		Fmin = I[0], Fmax = I[0];
		for (itheta = 0; itheta<ntheta; ++itheta)
		{
			for (ilamda = 0; ilamda<nlamda; ++ilamda)
			{
				itl = itheta*nlamda + ilamda;
				if (I[itl]>Fmax)
				{
					Fmax = I[itl];
				}
				if (I[itl]<Fmin)
				{
					Fmin = I[itl];
				}
				outComponent[itl] = I[itl];
			}

		}
	}
		break;
	}

	//输出GRD格式
	FILE* fpgrdout;
	if ((fpgrdout = fopen((_bstr_t)outfilename, "w")) == NULL)
	{
		MessageBox(NULL, _T("保存结算结果: :") + outfilename + _T("失败"), _T("全球主磁场计算"), MB_OK);
		return 0;
	}
	fprintf(fpgrdout, "DSAA\n");
	fprintf(fpgrdout, "%d\t%d\n", nlamda, ntheta);
	fprintf(fpgrdout, "%.5f\t%.5f\n", lamda0, lamda1);
	fprintf(fpgrdout, "%.5f\t%.5f\n", theta0, theta1);
	fprintf(fpgrdout, "%.5f\t%.5f\n", Fmin, Fmax);
	for (itheta = 0; itheta<ntheta; ++itheta)
	{
		for (ilamda = 0; ilamda < nlamda; ++ilamda)
		{
			itl = itheta*nlamda + ilamda;
			fprintf(fpgrdout, "%.5lf  ", outComponent[itl]);
		}
		fprintf(fpgrdout, "\n");
	}
	fclose(fpgrdout);
	// 释放内存
	delete[] W;
	delete[] p; 
	delete[] dp;
	delete[] g; 
	delete[] h; 
	delete[] X;
	delete[] Y;
	delete[] Z;
	delete[] H;
	delete[] F;
	delete[] I;
	delete[] D;
	delete[] theta;
	delete[] lamda;
	delete[] outComponent;

	return 1;
}

//平均滤波器的计算
void AverageFilter(double Fai0, long n,double* W)
{
	double temp1,temp2,temp3;
	temp1=1;
	temp2=1/2.0*(1+cos(Fai0));

    W[0]=temp1;
	W[1]=temp2;

    for(long i=2;i<=n;i++)
    {
        temp3  = (2*i-1)/(i+1)*cos(Fai0)*temp2-(i-2)/(i+1)*temp1;
        W[i]   = temp3;
        temp1  = temp2;
        temp2  = temp3;
    }
}