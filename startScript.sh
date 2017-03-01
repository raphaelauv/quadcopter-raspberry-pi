#!/bin/bash

# who run the script:
# pi@raspberry ~$ sh ./startScript.sh

while true; do
	sudo ./droneMain --log
	wait
	echo "\n---------------------\n"
done