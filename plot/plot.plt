#! /usr/bin/gnuplot

set terminal qt size 1800,1800

set datafile separator " "


set multiplot layout 6,1 rowsfirst

		name="MOTORS"
		set title name
		plot for [col=2:5:1] 'plotData.dat' using 0:col with lines title columnheader

		name="CLIENT (pitch roll yaw)"
		set title name
		plot for [col=8:10:1] 'plotData.dat' using 0:col with lines title columnheader

		name="INPUT (pitch roll yaw)"
		set title name
		plot for [col=11:13:1] 'plotData.dat' using 0:col with lines title columnheader

		name="OUTPUT (pitch roll yaw)"
		set title name
		plot for [col=14:15:1] 'plotData.dat' using 0:col with lines title columnheader

		name="BATTERY"
		set title name
		plot for [col=15:15:1] 'plotData.dat' using 0:col with lines title columnheader

		plot [][-1:1] NaN t"" # to force the minimal redraw of last plot of unset multiplot  ( weakness of GNUPLOT)


#unset multiplot  # Bug if uncomment

pause -1
