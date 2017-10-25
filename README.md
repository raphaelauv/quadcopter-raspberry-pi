# DRONE quadcopter on a raspberry-pi

https://vimeo.com/219726521



<a href="https://vimeo.com/219726521" target="_blank"><img src="https://user-images.githubusercontent.com/10202690/32027519-ce650de6-b9e9-11e7-86b3-66e38aafdbc4.gif" alt="alt text" width="500" height="whatever"></a>

<img src="https://user-images.githubusercontent.com/10202690/32025639-592c9e8e-b9e1-11e7-8fee-897b930bd3d0.jpg" alt="alt text" width="500" height="whatever">
<img src="https://user-images.githubusercontent.com/10202690/32025949-622249f2-b9e2-11e7-953c-f4a228931363.jpg" alt="alt text" width="500" height="whatever">


### PREPARATION Raspberry Pi 2

Install RASPBIAN JESSIE Kernel version : 4.4 on the Raspberry Pi 2

```
Clone this git with this specific command ( without sudo !!)

	pi@raspberry ~$ git clone git@moule.informatique.univ-paris-diderot.fr:thibaud/drone.git ~/drone
```


Active SSH and I2C , Install RTIMULib2 inside Lib folder ( follow the Readme instructions of RTIMULib2)



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
Disable USB RT PREEMPT LIMITATION -> pi@raspberry ~$ sudo nano /boot/cmdline.txt

	add those lines : 

	dwc_otg.fiq_enable=0
	dwc_otg.fiq_fsm_enable=0
```

```
Disable Garde-fou -> pi@raspberry ~$ sudo nano .profile

	add this line

	sudo sysctl kernel.sched_rt_runtime_us=-1
```

```
Frequence I2C -> pi@raspberry ~$ sudo nano /etc/modprobe.d/i2c.conf
	
	add this line

	options i2c_bcm2708 baudrate=400000
```

```
Edit the boot file of raspberrypi ->  pi@raspberry ~$ sudo nano /etc/rc.local
	
	add this line at the end

	sh /home/pi/drone/startScript.sh &
```

```
Edit the config file of raspberrypi -> pi@raspberry ~$ sudo nano /boot/config.txt

	Make sure it contains the settings of the file present in the git
```

### Compilation


#### CLIENT/CONTROLLER

dependencies : SDL (for XBOX360  and similar Controller) libsdl1.2-dev

	make client


XBOX360 controller calibration ->   jstest-gtk
https://apps.ubuntu.com/cat/applications/jstest-gtk/
Emulate controller XBOX360 from DualShocks
active bluetooth 
bouton playstation + share -> Flashing light
"ds4drv --emulate-xboxdrv"



#### DRONE

	make drone

## Execution

for the Drone on the Raspberry Pi 2 :

	sudo ./droneMain --help

for the Client/Controller :

	./client 192.168.***.***



## For calibration

Testing vibrations

	sudo ./droneMain --verb --testpower --data --vibration
