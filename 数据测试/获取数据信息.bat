set infile=大区域-布格重力异常.nc
set infofile=网格数据信息.txt

gmt grdinfo -L1 -L2 -M %infile% >%infofile%