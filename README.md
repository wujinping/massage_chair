##	Source code for Bluetooth controlled massage motor project
### What 
* THE source code is programmed for a Embedded control board which directly connects to a MOBILE device£¨could be an Andriod phone, an Apple iphone or some other device£©through a serial-bluetooth converter, it accepts commands from MOBILE device Applications, then interprets the commands into motion of the motors(back and bottom).
* THE commands include `AUTHENTICATION REQUEST`,`AUTHENTICATION RESPONSE` , `DEVICE START` and `DEVICE START RESPONSE` which are listed as below

### Application control protocol
* Currently only the following `2` types of commands that could be accepted and interpreted into corresponding motions, and `2` corresponding responses.

|Index|Command|Direction|Format|Usage|Example|
|-|-|-|-|-|-|
|1|AUTHENTICATION REQUEST|App-Device|"$R,$Seed*00"|Sending this command initiates a authentication procedure <br>`$Seed`is a 32-bits ASCII string(little endian, by the way) password seed for the purpose of encryption <br>`00` is reserved for a CRC-16 check of this message <br>|**"$R,AF1C7042*00"**<br> sets a password seed of `0xaf1c7042`
|2|AUTHENTICATION RESPONSE|Device-App|"$R,$ID*00\r\n"|This response message is sent when an `AUTHENTICATON REQUEST` command is received.<br>`$ID` is a 32-bits ASCII string of the device unique ID.|**"$R,12345678,*00"**<br> reponses the device ID of `0x12345678`. 
|3|DEVICE START|App-Device|"$S,$Password,$Time*00"|`$Password` is a 32-bits ASCII string password encoded with the `$ID` and `$Seed`(unfortunately this parameter is ignored for now)<br>`$Time` is a parameter that indicates for how much time in seconds the device counld keep running.|**"$S,3ed2c2ff,6000*00"**<br> set the device available for 100 minutes if `0x3ed2c2ff` is a valid password.
|4|DEVICE START RESPONSE|Device-App|"$S,$Result*00"|`$Result` represents the result of the authentication, `OK` represents a successful authentication and `FAILED` otherwise.|**"$S,OK*00"**<br> Authentication passed and the device unlocked to work mode.
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
