##	Source code for remote cradle control of 'Linz' project
### What is 'Linz'
* We do not konw yet..., but the jobs I did in this project include `remote cradle control`, `pc control protocol` and part of the windows souce code for the system written in `LUA` language.

### What is remote cradle control
* That is a control station that connects to PC, which accept commands from PC Applications, then interprets the commands into cradle turning angles(horizontal and vertical) and transmit to a remote cradle salve device, which accepts the angles and do the turning job.

### PC control protocol
* Currently only the following 2 types of commands that could interprets into corresponding turning angles.

|Index|Command|Format|Usage|Example|
|-|-|-|-|-|
|1|WISAA|"WISAA,$Index,$H,$V*00\r\n"|`$Index` is a parameter that indicates which remote cradle this command is issued to.<br>	`$H` stands for the absolute horizontal angle that you want set for this cradle.<br>	`$V` standsfor the absolute vertical angle.|**"WISAA,2,120,90*00"** Sets the 2nd cradle angles to 120 degrees horizontal and 90 degrees vertical.  
|2|WISAR|"WISAR,$Index,$H_R,$V_R*00\r\n"|`$Index` is a parameter that indicates which remote cradle this command is issued to.<br>	`$H_R` stands for the relative horizontal(relative to current angle) angle that you want set for this cradle.<br>	`$V_R` stands for the relative vertical angle.|**"WISAR,2,10,-10*00"**<br> Makes the 2nd cradle turns 10 degrees counter clock-wise horizontal and 10 degress clock-wise vertical. 
### What We Do
* In our project, the main funcion of this product is a realization of a command transmitter and a command receiver, the transmitter waits for commands from PC Applications and transmit the command through a 2.4G transceiver chip `nRF24L01`
, the receiver however, controls the cradle to turning into specific angles when commands were received from the transmitter.

* In this project, the transmission and reception protocol are handled by an ARM
based MCU (STM32F1xx),  the interface connects PC and the transmitter is through a USB device (which simulates our transmitter into a USB-SERIAL port).

* An ``nRF24L01`` is used for RF transceiver, both on transmitter side and receiver side.

* An `DS3115` and `DS3118` is used for the cradle, each serves as vertical digital servo and horizontal digital servo.

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
