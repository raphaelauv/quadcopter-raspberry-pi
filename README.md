# PROJET DRONE M1 INFORMATIQUE


#########################################################################
#							PREPARATION Raspberry Pi 2 					#

Install RASPBIAN JESSIE Kernel version:4.4 on the Raspberry Pi 2

Patch the Raspberry witch the kernel_4.4.47_RT.tgz

	pi@raspberry ~$ tar xzf kernel.tgz
	pi@raspberry ~$ sudo rm -r /lib/firmware/
	pi@raspberry ~$ sudo rm -r /boot/overlays/
	pi@raspberry ~$ cd boot
	pi@raspberry ~$ sudo cp -rd * /boot/
	pi@raspberry ~$ cd ../lib
	pi@raspberry ~$ sudo cp -dr * /lib/

add to  /boot/cmdline.txt
dwc_otg.fiq_enable=0
dwc_otg.fiq_fsm_enable=0


Active SSH , I2C , Disable Garde-fou



#########################################################################
#							Compilation									#

#------------------------------LE CLIENT--------------------------------#

necessite :SDL 2 - la lib pour utiliser la manette XBOX360 (libSDL)

	make client

Pour calibrer la manette XBOX360   ->   jstest-gtk
https://apps.ubuntu.com/cat/applications/jstest-gtk/

Pour simuler manette XBOX360 from DualShocks
I) activé bluetooth 
II) bouton playstation + share -> lumiere qui clignote
III) taper "ds4drv --emulate-xboxdrv" dans le terminal


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

- choisir une action quand il n'y a plus de manette

- Corriger probleme avec la version actuel du kernel sur raspberry , ET PREEMP

- Filtrer valeur accelerometre verticale et altimetre pour le PID d'altitude

- Catcher Signaux System UNIX