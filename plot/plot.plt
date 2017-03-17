#! /usr/bin/gnuplot


### Start multiplot (2x2 layout)
set multiplot layout 3,1 rowsfirst


name="FENETRE DESSIN"
set title name
plot for [col=2:5:1] 'plotData.dat' using 0:col with lines title columnheader

plot for [col=6:7:1] 'plotData.dat' using 0:col with lines title columnheader

plot for [col=8:8:1] 'plotData.dat' using 0:col with lines title columnheader


unset multiplot

pause -1