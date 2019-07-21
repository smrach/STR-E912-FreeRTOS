# File: Makefile
#
# Project: STR91x Eclipse demo
#
# Description: this makefile build the demo with the lwIP tasks.
#
# Created: 27/08/2008
#
# Author: Stefano Oliveri
# E-mail: software@stf12.net
# Modified by :Stefano Mrach 2019

# Project name
PROJECT_NAME=RTOSDemo

GCC_BASE = D:/Compilers/em_armgcc/
GCC_BIN = $(GCC_BASE)bin/
GCC_LIB = $(GCC_BASE)arm-none-eabi/lib/
GCC_INC = $(GCC_BASE)arm-none-eabi/include/

BASE_PATH=D:/str912/FreeRTOS
DEMO_PATH=$(BASE_PATH)/LcdDemo

# Project specific PATH
RTOS_SOURCE_DIR=$(BASE_PATH)/Source
#DEMO_COMMON_DIR=$(DEMO_PATH)/Common/Minimal
#DEMO_INCLUDE_DIR=$(DEMO_PATH)/Common/include
LWIP_DIR=$(DEMO_PATH)/Common/ethernet/lwIP/HEAD/src
LWIP_PORT_DIR=$(DEMO_PATH)/Common/ethernet/lwIP/HEAD/contrib/port
ST_LIB_DIR=$(DEMO_PATH)/STR91xFWLib/v2.0
USER_CODE_DIR=$(DEMO_PATH)/LCD
#USE_THUMB_MODE = YES
USE_THUMB_MODE = NO

AS = $(GCC_BIN)arm-none-eabi-as
CC = $(GCC_BIN)arm-none-eabi-gcc
CPP = $(GCC_BIN)arm-none-eabi-g++
LD = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-size
NM = $(GCC_BIN)arm-none-eabi-nm
LDSCRIPT=STR91x-ROM.ld

ifeq ($(USE_THUMB_MODE),YES)
THUMB    = -mthumb
THUMB_IW = -mthumb-interwork
THUMB_D = THUMB_INTERWORK
THUMB_CLEAN = 
else 
THUMB    = 
THUMB_IW = 
THUMB_D = NO_THUMB_INTERWORK
THUMB_CLEAN = 
endif

# should use --gc-sections but the debugger does not seem to be able to cope with the option.
LINKER_FLAGS=  $(THUMB) -nostartfiles -Xlinker -o$(PROJECT_NAME).axf -Xlinker -M -Xlinker -Map=$(PROJECT_NAME).map -Xlinker --no-gc-sections

DEBUG=-g
OPTIM=-O0

CFLAGS= $(DEBUG) \
		$(OPTIM) \
		-T$(LDSCRIPT) \
		-I . \
		-I $(LWIP_DIR)/include \
		-I $(LWIP_DIR)/include/ipv4 \
		-I $(LWIP_DIR)/include/lwip \
		-I $(LWIP_DIR)/include/netif \
		-I $(LWIP_DIR)/include/ipv4/lwip \
		-I $(LWIP_PORT_DIR)/FreeRTOS/STR91x \
		-I $(ST_LIB_DIR)/inc \
		-I $(USER_CODE_DIR)/ \
		-I $(RTOS_SOURCE_DIR)/include \
		-I $(RTOS_SOURCE_DIR)/portable/GCC/STR9x \
		-D STR91X_GCC \
		-D STACK_LWIP_130 \
		-D $(THUMB_D)
		-mcpu=arm966e-s \
		-D PACK_STRUCT_END=__attribute\(\(packed\)\) \
		-D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
		-fomit-frame-pointer \
		-ffunction-sections \
		$(THUMB_IW)

										
THUMB_SOURCE= \

ARM_SOURCE= \
		91x_it.c \
		stf_stdlib.c \
		printf.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/STR9x/port.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/STR9x/portISR.c \
		main.c \
		net_launch_srv.c \
		./lwipWebServer/httpd.c \
		./lwipWebServer/fs.c \
		./lwipNetCalc/NetCalcApp.c \
		$(USER_CODE_DIR)/lcd.c \
		$(ST_LIB_DIR)/src/91x_lib.c \
		$(ST_LIB_DIR)/src/91x_enet.c \
		$(ST_LIB_DIR)/src/91x_scu.c \
		$(ST_LIB_DIR)/src/91x_gpio.c \
		$(ST_LIB_DIR)/src/91x_fmi.c \
		$(ST_LIB_DIR)/src/91x_tim.c \
		$(ST_LIB_DIR)/src/91x_uart.c \
		$(ST_LIB_DIR)/src/91x_wdg.c \
		$(ST_LIB_DIR)/src/91x_vic.c \
		$(ST_LIB_DIR)/src/91x_ssp.c \
		$(RTOS_SOURCE_DIR)/list.c \
		$(RTOS_SOURCE_DIR)/queue.c \
		$(RTOS_SOURCE_DIR)/tasks.c \
		$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c \
		$(LWIP_DIR)/api/api_lib.c \
		$(LWIP_DIR)/api/api_msg.c \
		$(LWIP_DIR)/api/err.c \
		$(LWIP_DIR)/api/netbuf.c \
		$(LWIP_DIR)/api/netifapi.c \
		$(LWIP_DIR)/api/tcpip.c \
		$(LWIP_DIR)/core/ipv4/icmp.c \
		$(LWIP_DIR)/core/ipv4/inet.c \
		$(LWIP_DIR)/core/ipv4/inet_chksum.c \
		$(LWIP_DIR)/core/ipv4/ip.c \
		$(LWIP_DIR)/core/ipv4/ip_addr.c \
		$(LWIP_DIR)/core/ipv4/ip_frag.c \
		$(LWIP_DIR)/core/dhcp.c \
		$(LWIP_DIR)/core/dns.c \
		$(LWIP_DIR)/core/init.c \
		$(LWIP_DIR)/core/mem.c \
		$(LWIP_DIR)/core/memp.c \
		$(LWIP_DIR)/core/netif.c \
		$(LWIP_DIR)/core/pbuf.c \
		$(LWIP_DIR)/core/raw.c \
		$(LWIP_DIR)/core/stats.c \
		$(LWIP_DIR)/core/sys.c \
		$(LWIP_DIR)/core/tcp.c \
		$(LWIP_DIR)/core/tcp_in.c \
		$(LWIP_DIR)/core/tcp_out.c \
		$(LWIP_DIR)/core/udp.c \
		$(LWIP_DIR)/core/snmp/asn1_dec.c \
		$(LWIP_DIR)/core/snmp/asn1_enc.c \
		$(LWIP_DIR)/core/snmp/mib_structs.c \
		$(LWIP_DIR)/core/snmp/mib2.c \
		$(LWIP_DIR)/core/snmp/msg_in.c \
		$(LWIP_DIR)/core/snmp/msg_out.c \
		$(LWIP_DIR)/netif/etharp.c \
		$(LWIP_DIR)/netif/stf91x_ethernetif.c \
		$(LWIP_PORT_DIR)/FreeRTOS/STR91x/sys_arch.c \
		
		
THUMB_OBJS = $(THUMB_SOURCE:.c=.o)
ARM_OBJS = $(ARM_SOURCE:.c=.o)

LIBS= 

all: $(PROJECT_NAME).bin

$(PROJECT_NAME).bin : $(PROJECT_NAME).axf
	$(OBJCOPY) $(PROJECT_NAME).axf -O binary $(PROJECT_NAME).bin
	 

$(PROJECT_NAME).axf : $(THUMB_OBJS) $(ARM_OBJS) startup.s vector.s Makefile
	$(CC) $(CFLAGS) $(ARM_OBJS) $(THUMB_OBJS) $(LIBS) startup.s vector.s $(LINKER_FLAGS) 


$(THUMB_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) -mthumb $< -o $@

$(ARM_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

	
clean :
	$(THUMB_CLEAN)
	del *.o -r
 
log : $(PROJECT_NAME).axf
	$(NM) -n $(PROJECT_NAME).axf > $(PROJECT_NAME)SymbolTable.txt
	$(OBJDUMP) $(PROJECT_NAME).axf > $(PROJECT_NAME)MemoryListing.txt

