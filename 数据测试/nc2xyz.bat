set ncfile=pres_temp.nc
set xyzfileout=pres_temp.dat

gmt grd2xyz %ncfile% > %xyzfileout%