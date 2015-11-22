#include "GRD_surfer.h"

ostream& operator<<(ostream &out, GRDDATA_SURFER value)
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

ifstream &operator>>(ifstream &fin, GRDDATA_SURFER& value)
{
	char dsaa[10];
	fin >> dsaa;
	if (dsaa[3]!='A')
	{
		value.Flag = false;
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
	}value.data = new double[value.rows*value.columns];
	for (int i = 0; i < value.rows; i++)
	{
		for (int j = 0; j < value.columns; j++)
		{
			fin >> value.data[j + i*value.columns];
		}
	}
	return fin;
}
