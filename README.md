# STR-E912-FreeRTOS
Here are some my experiments on GCC builds of FreeRTOS on specific board Olimex STR-E912

Some time ago, I bought from ebay a development board made by Olimex, the STR-E912.
Starting from a good article I've found here http://developers.stf12.net/just-another-eclipse-demo-str91x#TOC-STR912-specific-info  made by Stefano Oliveri with him informations I slowly started to manage and setup my environment to be able to learning this board.

![alt text](https://github.com/smrach/STR-E912-FreeRTOS/blob/master/IMG_20190720_185050_small.jpg)

 After some trials I got the fast way to upload firmware on it. Since I dont want to buy nor install on my own pc others tools, I searched for a hand build tool. For compiling I use em_armgcc that I've downloaded somewhere...;)
From there I had to find how to put the firmware inside it, so I managed to setup a Raspberry PI with openocd.

![alt text](https://github.com/smrach/STR-E912-FreeRTOS/blob/master/IMG_20190721_165533_small.jpg)

My setup is the following:
- Pc with Windows 10 LTSB 2016 (with custom blinded privacy options set).
- Make for windows
- Folder on disk with em_armgcc toolchain (maybe from here) https://launchpad.net/gcc-arm-embedded/+download
- VS Code for visual source code inspect and Notepad++ for fast source code edit.
- Raspberry Pi Model B Rev 2(2011.12) running Raspbian 8 (jessie) 4.4.34+ connected to the lan ( saved from trash bin and given as gift from a work colleague)
- Openocd installed on Raspberry Pi and some wires connected to the JTAG port of STR-E912 target board.
- Putty SSH on windows for launch Raspberry Openocd daemons.
- WinSCP on windows for update the compiled binaries on Raspberry folder.
- Telnet on windows pc to Raspberry Openocd daemon for board flashing/update.

How to setup and test the build:
- Grab the sources.
- Make sure you have Make installed.
- Make sure you have arm-none-eaby gcc toolchain installed.
- If you dont have or want have the toolchain on path, simply edit the makefile on LcdDemo folder and set the GCC_BASE variable to the path of your toolchain.
- Launch 'make' from LcdDemo folder and if all go right you should have the RTOSDemo.bin file ready to be flashed on board.

That's all!
