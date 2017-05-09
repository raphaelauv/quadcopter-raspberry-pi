#! /usr/bin/gnuplot

fileName=ARG1 ; #else  fileName="plotData.dat"


set terminal qt size 1800,1800

set datafile separator " "



set multiplot layout 7,1 rowsfirst
		
		

		name="MOTORS (Signal microsecond)"
		set title name
		plot for [col=2:5:1] fileName using 0:col with lines title columnheader

		name="ANGLE (pitch roll) (angle degree)"
		set title name
		plot for [col=6:7:1] fileName using 0:col with lines title columnheader

		name="CLIENT (pitch roll yaw)  (SCALE *1000)"
		set title name
		plot for [col=8:10:1] fileName using 0:col with lines title columnheader

		name="INPUT (pitch roll yaw)  (SCALE *1000)"
		set title name
		plot for [col=11:13:1] fileName using 0:col with lines title columnheader

		name="OUTPUT (pitch roll yaw)  (SCALE *1000)"
		set title name
		plot for [col=14:16:1] fileName using 0:col with lines title columnheader

		name="BATTERY (voltage SCALE *1000)"
		set title name
		plot for [col=17:17:1] fileName using 0:col with lines title columnheader

		name="NOTHING"
		set title name
		plot [][-1:1] NaN t"" # to force the minimal redraw of last plot of unset multiplot  ( weakness of GNUPLOT)


#unset multiplot  # Bug if uncomment

pause -1
