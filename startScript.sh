#!/bin/bash

# who run the script:
# pi@raspberry ~$ sh drone/startScript.sh

#Set sound volume to maximum
sudo amixer sset 'PCM' 100%
file="/home/pi/drone/droneMain2"
if [ -f "$file" ]
	then
	while true; do
        	result=$( sudo  /home/pi/drone/droneMain --verb  --log )
		#wait
		echo $result
		if ["$result" -ne 0]
			then
				echo "PROGRAM FAIL";
				break;
		fi
        	echo "\n---------------------\n"
		echo "\n relunch PROGRAM \n"
	done


else
	echo $file "FILE not found"
fi
