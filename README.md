# PROJET DRONE M1 INFORMATIQUE

#########################################################################
#							Compilation									#

#------------------------------LE CLIENT--------------------------------#

necessite :SDL 2 - la lib pour utiliser la manette XBOX360 (libSDL)

	make client

Pour claibrer la manette XBOX360   ->   jstest-gtk
https://apps.ubuntu.com/cat/applications/jstest-gtk/


#------------------------------LE DRONE--------------------------------#

	make drone

#########################################################################


#########################################################################
#							Execution									#

pour executer le code du drone :

	./droneMain

pour executer le code du client/controlleur :

	./client 127.0.0.1


--------------
A faire

-Passer en UDP -> 65 %

-Faire protocol d'arret totale et de "pause"

-Volatile , variable bool_arret_motor

-Calibrer valeur manette

-Interfacer correctement Code C appeler en C++

-faire quelque chose quand il n'y a pas de manette branché.