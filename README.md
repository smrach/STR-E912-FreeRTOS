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
- VS Code for visual source code inspect e Notepad++ for fast source code edit.
- Raspberry Pi model 2011.12 connected to lan ( saved from trash and given as gift from a work colleague)
- Openocd installed on Raspberry Pi and some wires to connected the STR-E912 target board
- Putty SSH on windows opening openocd
- WinSCP on windows for put binary on Raspberry folder.
- Telnet on windows pc to ruspberry for board flashing.

