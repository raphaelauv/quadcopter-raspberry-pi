#!/bin/bash

# who run the script:
# pi@raspberry ~$ sh ./startScript.sh

#Set sound volume to maximum
sudo amixer sset 'PCM' 100%

while true; do
	sudo ./droneMain --log
	wait
	echo "\n---------------------\n"
done