#include "iostream"
#include "fstream"
using namespace std;

#ifndef GRDSURFER
#define GRDSURFER
struct GRDDATA_SURFER
{
	int rows, columns;
	double bounds[6];
	double* data=NULL;
	bool Flag = true;

	friend ostream &operator<<(ostream &out, GRDDATA_SURFER value);			//操作符重载：输出
	//friend istream &operator<<(istream &in, GRDDATA_SURFER value);			//操作符重载：输入
	friend ifstream &operator>>(ifstream &in, GRDDATA_SURFER& value);			//操作符重载：输入
};


#endif
