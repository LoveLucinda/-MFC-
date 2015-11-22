del *.ps

set infile=������-���������쳣.nc
set outpsfile=����GMT_nc.ps
set tempcpt=tempcpt.cpt
set gradientf=tempgradient

gmt psbasemap -JM10c -R45/70/-45/-25 -B5 -K -P > %outpsfile%
gmt grd2cpt %infile% -Crainbow -Z > %tempcpt%
gmt grdgradient %infile% -A0/270 -Ne0.2 -G%gradientf%
gmt grdimage %infile% -J -R -B -C%tempcpt% -I%gradientf% -O  -K -E150 -V >> %outpsfile%
rem gmt grdcontour %infile% -R -J -C50 -Wc0.25pta -Wa0.75pt2_2:0 -O -K -P >> %outpsfile%

pause