##	Source code for Bluetooth controlled massage motor project
### What 
* THE source code is programmed for a Embedded control board which directly connects to a MOBILE device£¨could be an Andriod phone, an Apple iphone or some other device£©through a serial-bluetooth converter, it accepts commands from MOBILE device Applications, then interprets the commands into motion of the motors(back and bottom).
* THE commands include `AUTHENTICATION`,`START` and `STOP` which are listed as below

### Application control protocol
* Currently only the following `2` types of commands that could be accepted and interpreted into corresponding motions.

|Index|Command|Format|Usage|Example|
|-|-|-|-|-|
|1|WISAA|"WAUTN,$String*00\r\n"|`$Sting` is a 32-bit ASCII encoded string which is encrypted with the unique ID of the MCU.<br>|**"WISAA,AF1C7042*00"** unlocks a device with a unique ID of `7E0E32E34403`
|2|WISAR|"WSTAT,$Time*00\r\n"|`$Time` is a parameter that indicates for how much time in seconds the device counld keep running.|**"WSTAT,6000*00"**<br> set the device available for 100 minutes. 
### How
* In our project, the main funcion of this product is a realization of a command receiver and a motion controller , the receiver waits for commands from Mobile Applications and interprets to specific actions. 

* In this project, the receiver, motion control and reception protocol are handled by an ARM-based MCU (STM32F1xx),  the Bluetooth BLE Protocol is processed by a SERIAL-Bluetooth chip `BLE102`, the motor is just controlled by a GPIO of the MCU .

* An ``BLE102`` is used for data receiver, and possibly data transmitter.

### Source Code
The source code directories has such trees:

	HARDWARE/
	SYSTEM/
	USER/
	CORE/
	CMSIS/
	OBJ/


The SYSTEM/, CORE/, and CMSIS/ directory is standard reference core/peripheral
source code for STM32F1xx series.

Please refer to the souce code for more details.

### Build
  The souce code is build under Keil uVersion4 (Keil v5), other comilers are
not tested.

  Please see the source code change log (using "git log" command)for revision
details.
