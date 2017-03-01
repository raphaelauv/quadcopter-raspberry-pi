CC=g++

CFLAGS=-std=c++0x -Wno-write-strings

LDFLAGS= -lpthread 
LDFLAGS_WiringPi= -lwiringPi
LDFLAGS_RTIMULIB=  -L/usr/local/lib  -lRTIMULib 


ARCH := $(shell uname -m)

ifeq ($(ARCH),armv7l)
	LDFLAGS_Raspberry= $(LDFLAGS) $(LDFLAGS_WiringPi)  $(LDFLAGS_RTIMULIB)
else
	LDFLAGS_Raspberry= $(LDFLAGS) $(LDFLAGS_RTIMULIB)
endif

LDFLAGS_ClientRemote= $(LDFLAGS) -lSDL -lSDLmain

SRC=src/concurrent.c src/network.c src/log.c

#SRC_RTIMULib = $(wildcard src/RTIMULib/*.cpp) $(wildcard src/RTIMULib/IMUDrivers/*.cpp)

SRC_drone = $(SRC) src/serv.c src/Calibration/calibrate.c

SRC_drone_CPP= src/PID.cpp src/capteur.cpp 

SRC_client = $(SRC)  src/client.c src/Controller/controller.c src/Controller/manette.c 

OBJdroneMain= $(SRC_drone:.c=.o) $(SRC_drone_CPP:.cpp=.o)
#src/capteur.o src/PID.o 

OBJclientRemote= $(SRC_client:.c=.o) 

#OBJ_RTIMULib= $(SRC_RTIMULib:.cpp=.o)

EXEC = clientRemoteMain droneMain

all: 
	
	echo 'type : make drone   |or type : make client'

#all: $(EXEC)

drone:droneMain

client:clientRemoteMain

src/droneMain.o: src/droneMain.c
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS)

#$(OBJ_RTIMULib)
droneMain: src/motors.o $(OBJdroneMain)  src/droneMain.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_Raspberry)

src/clientRemoteMain.o: src/clientRemoteMain.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clientRemoteMain: $(OBJclientRemote) src/clientRemoteMain.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_ClientRemote)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf object/*.o
	find . -name '*.o' -delete

mrproper: clean
	rm -rf $(EXEC)
	rm -rf LOG_*
