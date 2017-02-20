CC=g++

CFLAGS=-std=c++0x
CFLAGS_Raspberry= $(CFLAGS) -lwiringPi

LDFLAGS=-lpthread 

#--------------------------SUR RASBPERRY---------------------------#

#rajouté  -lwiringPi   a la suite de  LDFLAGS_Raspberry= $(LDFLAGS)
LDFLAGS_Raspberry= $(LDFLAGS)
#-lwiringPi 

#------------------------------------------------------------------#

LDFLAGS_ClientRemote= $(LDFLAGS) -lSDL -lSDLmain

SRC=src/concurrent.c src/network.c src/log.c

SRC_RTIMULib = $(wildcard src/RTIMULib/*.cpp) $(wildcard src/RTIMULib/IMUDrivers/*.cpp)

SRC_drone = $(SRC) src/PID.c src/serv.c src/capteur.cpp

SRC_client = $(SRC)  src/client.c src/Controller/controller.c src/Controller/manette.c 

OBJdroneMain= $(SRC_drone:.c=.o)

OBJclientRemote= $(SRC_client:.c=.o) 

OBJ_RTIMULib= $(SRC_RTIMULib:.cpp=.o)

EXEC = clientRemoteMain droneMain

all: 
	
	echo 'type : make drone   |or type : make client'

#all: $(EXEC)

drone:droneMain

client:clientRemoteMain


src/motors.o: src/motors.c
	$(CC) $(CFLAGS_Raspberry) -c $< -o $@

src/droneMain.o: src/droneMain.c
	$(CC) $(CFLAGS_Raspberry) -o $@ -c $< 

droneMain: src/motors.o $(OBJdroneMain) $(OBJ_RTIMULib) src/droneMain.o
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