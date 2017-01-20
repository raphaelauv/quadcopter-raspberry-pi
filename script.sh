#!/bin/bash

#  pour drone

gcc -lwiringPi -O0 -g3 -Wall -c -o "object/concurrent.o" "src/concurrent.c"

gcc -lwiringPi -O0 -g3 -Wall -c -o "object/controldeVol.o" "src/controldeVol.c"

gcc -lwiringPi -O0 -g3 -Wall -c -o "object/droneMain.o" "src/droneMain.c"

gcc -lwiringPi -O0 -g3 -Wall -c -o "object/motors.o" "src/motors.c"

gcc -lwiringPi -O0 -g3 -Wall -c -o "object/serv.o" "src/serv.c"

g++ -o "droneMain"  object/concurrent.o object/controldeVol.o object/droneMain.o object/motors.o object/serv.o  -lpthread -lwiringPi


#   pour CLIENT

gcc -O0 -g3 -Wall -c -o "object/controller.o" "src/Manette/controller.c"

gcc -O0 -g3 -Wall -c -o "object/manette.o" "src/Manette/manette.c"

gcc -O0 -g3 -Wall -c -o "object/client.o" "src/client.c"

gcc -O0 -g3 -Wall -c -o "object/concurrent.o" "src/concurrent.c"

g++  -o "droneCLIENT"  object/controller.o object/manette.o object/client.o object/concurrent.o   -lSDL -lpthread -lSDLmain