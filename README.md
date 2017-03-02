# PROJET DRONE M1 INFORMATIQUE



### PREPARATION Raspberry Pi 2

Install RASPBIAN JESSIE Kernel version : 4.4 on the Raspberry Pi 2

```
Clone this git with this specific command ( without sudo !!)

	pi@raspberry ~$ git clone git@moule.informatique.univ-paris-diderot.fr:thibaud/drone.git ~/drone
```

Active SSH and I2C , Install RTIMULib2 inside Lib folder ( follow Readme instructions )

```
Patch the Raspberry witch the kernel_4.4.47_RT.tgz :

	pi@raspberry ~$ tar xzf kernel_4.4.47_RT.tgz
	pi@raspberry ~$ sudo rm -r /lib/firmware/
	pi@raspberry ~$ sudo rm -r /boot/overlays/
	pi@raspberry ~$ cd boot
	pi@raspberry ~$ sudo cp -rd * /boot/
	pi@raspberry ~$ cd ../lib
	pi@raspberry ~$ sudo cp -dr * /lib/
```

```
Disable USB RT PREEMPT LIMITATION -> add to /boot/cmdline.txt : 

	dwc_otg.fiq_enable=0
	dwc_otg.fiq_fsm_enable=0
```

```
Disable Garde-fou -> add to .profile:

	sudo sysctl kernel.sched_rt_runtime_us=-1
```

```
Frequence I2C -> sudo nano /etc/modprobe.d/i2c.conf :

	options i2c_bcm2708 baudrate=400000
```



### Compilation


##### * CLIENT/CONTROLLER

dependencies : SDL (for XBOX360 Controller) libsdl1.2-dev

	make client

XBOX360 controller calibration ->   jstest-gtk
	https://apps.ubuntu.com/cat/applications/jstest-gtk/

Emulate controller XBOX360 from DualShocks
 * active bluetooth 
	* bouton playstation + share -> Flashing light
	* "ds4drv --emulate-xboxdrv"



##### * DRONE

	make drone

### Execution

for the Drone on the Raspberry Pi 2 :

	sudo ./droneMain

for the Client/Controller :

	./client 192.168.***.***
