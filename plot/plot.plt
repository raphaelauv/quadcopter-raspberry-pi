#! /usr/bin/gnuplot

name="FENETRE DESSIN"
set title name
plot for [col=2:8:1] 'plotData.dat' using 0:col with lines title columnheader
pause -1

