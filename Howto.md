What you need:

TEXAS INSTRUMENTS - CC1110DK-MINI-868 - CC1110, WIRELESS, MINI DEV KIT

<img src='http://ztsunami.googlecode.com/svn/wiki/images/CC1110DK-MINI_Devkit.jpg' alt='mDk' />

Costs around £55 including debugger (£30 on its own), but you could well split it in 2 and sell the second part



DO NOT GET THE FOLLOWING: 

TEXAS INSTRUMENTS - CC1110EMK868-915 - CC1110 RF TRANSCEIVER, EVAL MODULE

It might work, but it will be very difficult.

It's also more expensive, and doesn't come with the flasher/debugger

I know, I made this mistake at first, and had to return it.



---


Flash the firmware (Source Tab, Browse, Trunk/ztsunamiFW/zTsunami.hex) to the chip using the provided USB flasher tool and "TI SmartRF Flash Programmer Software"


<img src='http://ztsunami.googlecode.com/svn/wiki/images/GetFirmware.png' alt='GFw' />



<img src='http://ztsunami.googlecode.com/svn/wiki/images/Cabling.jpg' alt='Cbl' />

1 Orientation of the flashing cable

2 jumper config for battery power



<img src='http://ztsunami.googlecode.com/svn/wiki/images/Flashing.png' alt='Fls' />


---


Solder 3 wires as such:

Black is Ground (GND)

Blue is   Devkit Rx, PC Tx (P0\_2)

Yellow is Devkit Tx, PC Rx (P0\_3)


<img src='http://ztsunami.googlecode.com/svn/wiki/images/BoardWires.jpg' alt='BoW' />


Attach the wires to a serial port (like an FTDI USB to serial cable), or scope them with a logic analyser, like the Saleae ( https://www.saleae.com/ ) tool.

Launch a serial terminal (Tera Term ( http://ttssh2.sourceforge.jp/index.html.en ) for example)

Select correct port, Set bitrate to 115200


<img src='http://ztsunami.googlecode.com/svn/wiki/images/SerialPortConfig.png' alt='SPC' />



---


As of v0.1 the firmware is being developed using Keil C51 v9.5.2
( http://www.keil.com/c51/c51.asp )

With the demo license it is limited to 2KB of code.

v0.1 uses: "Program Size: data=180.2 xdata=0 code=1521"


So it is likely the toolchain change to SDCC ( http://sdcc.sourceforge.net/ ) in the future.

You don't need these tools if you plan to use the unmodified ztsunami default firmware: just flash the .hex file provided