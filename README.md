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

Passer en UDP -> 65 %

Faire protocol d'arret totale et de "pause"

Calibrer valeur manette

corriger Code C appeler en C++