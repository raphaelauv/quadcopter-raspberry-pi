#!/bin/bash

# who run the script:
# pi@raspberry ~$ sh drone/startScript.sh

#Set sound volume to maximum
sudo amixer sset 'PCM' 100%

file="/home/pi/drone/droneMain"
if [ -f "$file" ]

then
	#while true; do
        	$var = sudo /home/pi/drone/droneMain --verb
		echo $var
		if ["$var" -ne 0];then break
		fi
        	wait;
        	echo "\n---------------------\n"
	#done


else
	echo "$file not found"
fi
