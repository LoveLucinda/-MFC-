
#include "stdafx.h"
//标志磁场分量的ID
#define COMPONENT_T	0
#define COMPONENT_X	1
#define COMPONENT_Y	2
#define COMPONENT_Z	3
#define COMPONENT_H	4
#define COMPONENT_D	5
#define COMPONENT_I	6

int CalMag(CString cofFileName, int ComponentType, int n_start, int n_end, double lamda0, double lamda1, double dlamda, 
	double theta0, double theta1, double dtheta, CString outfilename);
