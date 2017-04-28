#! /usr/bin/gnuplot

set multiplot layout 6,1 rowsfirst


name="MOTORS"
set title name
plot for [col=2:5:1] 'plotData.dat' using 0:col with lines title columnheader

name="ANGLE"
set title name
plot for [col=6:6:1] 'plotData.dat' using 0:col with lines title columnheader

name="CLIENT (pitch roll yaw)"
set title name
plot for [col=7:9:1] 'plotData.dat' using 0:col with lines title columnheader

name="INPUT (pitch roll yaw)"
set title name
plot for [col=10:12:1] 'plotData.dat' using 0:col with lines title columnheader

name="OUTPUT (pitch roll yaw)"
set title name
plot for [col=13:15:1] 'plotData.dat' using 0:col with lines title columnheader

name="BATTERY"
set title name
plot for [col=16:16:1] 'plotData.dat' using 0:col with lines title columnheader


unset multiplot

pause -1