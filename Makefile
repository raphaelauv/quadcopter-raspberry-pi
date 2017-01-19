CC=g++

CFLAGS=-std=c++0x
CFLAGS_Raspberry= $(CFLAGS) -lwiringPi

LDFLAGS=-lpthread 
LDFLAGS_Raspberry= $(LDFLAGS) -lwiringPi 
LDFLAGS_ClientRemote= $(LDFLAGS) -lSDL -lSDLmain

EXEC=  clientRemote

SRC=src/concurrent.c

SRC_drone = $(SRC) src/controldeVol.c src/motors.c src/serv.c

SRC_client = $(SRC) src/Manette/controller.c src/Manette/manette.c

OBJdroneMain= $(SRC_drone:.c=.o)  $(SRC_drone:.cpp=.o) 

OBJclientRemote= $(SRC_client:.c=.o)  $(SRC_client:.cpp=.o)


all: $(EXEC)

object/droneMain.o: src/droneMain.c
	$(CC) -o $@ -c $< $(CFLAGS_Raspberry)

droneMain: object/droneMain.o $(OBJdroneMain)
	$(CC) -o $@ $^ $(LDFLAGS_Raspberry)

object/client.o: src/client.c
	$(CC) -o $@ -c $< $(CFLAGS)

clientRemote: object/client.o $(OBJclientRemote)
	$(CC) -o $@ $^ $(LDFLAGS_ClientRemote)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf object/*.o
	find . -name '*.o' -delete

mrproper: clean
	rm -rf $(EXEC)