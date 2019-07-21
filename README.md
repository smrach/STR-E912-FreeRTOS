# STR-E912-FreeRTOS
Here are some my experiments on GCC builds of FreeRTOS on specific board Olimex STR-E912

Some time ago, I bought from ebay a development board made by Olimex, the STR-E912.
Starting from a good article I've found here http://developers.stf12.net/just-another-eclipse-demo-str91x#TOC-STR912-specific-info  made by Stefano Oliveri I slowly started to manage and setup my environment to be able to learning this board.

![alt text](https://github.com/smrach/STR-E912-FreeRTOS/blob/master/IMG_20190720_185050_small.jpg)

 After some trials I got the fast way to upload firmware on it. Since I dont want to buy nor install on my own pc others tools, I searched for a hand build tool. For compiling I use em_armgcc that I've downloaded somewhere...;)
From there I had to find how to put the firmware inside it, so I managed to setup a Raspberry PI with openocd.

![alt text](https://github.com/smrach/STR-E912-FreeRTOS/blob/master/IMG_20190721_165533_small.jpg)


