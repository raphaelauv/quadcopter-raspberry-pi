CC=g++

CFLAGS=-std=c++0x -Wno-write-strings

LDFLAGS= -lpthread 
LDFLAGS_RTIMULIB=  -L/usr/local/lib  -lRTIMULib 
LDFLAGS_WiringPi= -lwiringPi

ARCH := $(shell uname -m)
ifeq ($(ARCH),armv7l)
	LDFLAGS_Raspberry= $(LDFLAGS)  $(LDFLAGS_RTIMULIB) $(LDFLAGS_WiringPi)
else
	LDFLAGS_Raspberry= $(LDFLAGS) $(LDFLAGS_RTIMULIB)
endif

LDFLAGS_ClientRemote= $(LDFLAGS) -lSDL -lSDLmain

SRC_basic = src/concurrent.c src/network.c src/log.c

#SRC_RTIMULib = $(wildcard src/RTIMULib/*.cpp) $(wildcard src/RTIMULib/IMUDrivers/*.cpp)

SRC_drone = $(SRC_basic) src/serv.c src/Calibration/calibrate.c src/ADC/MCP3008.c src/motors.c src/PWM/I2C_custom.c src/PWM/PCA9685.c

SRC_drone_CPP = src/PID.cpp src/sensor.cpp 

SRC_client = $(SRC_basic)  src/client.c src/Controller/controller.c src/Controller/manette.c 

OBJdroneMain= $(SRC_drone:.c=.o) $(SRC_drone_CPP:.cpp=.o)
#src/sensor.o src/PID.o 

OBJclientRemote= $(SRC_client:.c=.o) 

#OBJ_RTIMULib= $(SRC_RTIMULib:.cpp=.o)

EXEC = clientRemoteMain droneMain

all: 
	echo 'type : make drone   |or type : make client'

#all: $(EXEC)

drone:droneMain

client:clientRemoteMain

#$(OBJ_RTIMULib)
droneMain: $(OBJdroneMain)  src/droneMain.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_Raspberry)

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
	rm -rf DATA_*
