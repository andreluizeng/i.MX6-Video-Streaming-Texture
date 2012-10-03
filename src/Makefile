APPNAME			= simple_gpu_player
DESTDIR			= ../bin

BSP_DIR			= /home/andre/bsps/mx6x/1204/ltib/rootfs/home/gst-api
ROOTFS_DIR		= /home/andre/bsps/mx6x/1204/ltib/rootfs

TOOLCHAIN		= /opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain
CROSS_COMPILE		= $(TOOLCHAIN)/bin/arm-none-linux-gnueabi-

LD			= $(CROSS_COMPILE)g++
CC			= $(CROSS_COMPILE)gcc
DEL_FILE		= rm -rf
CP_FILE			= cp -rf      

TARGET_PATH_LIB 	= $(ROOTFS_DIR)/usr/lib
TARGET_PATH_INCLUDE 	= $(ROOTFS_DIR)/usr/include

CFLAGS			= -DLINUX -DUSE_SOC_MX6 -Wall -O2 -fsigned-char -march=armv7-a -mfpu=neon -mfloat-abi=softfp \
			  -DEGL_API_FB -DGPU_TYPE_VIV -DGL_GLEXT_PROTOTYPES -DENABLE_GPU_RENDER_20 \
			  -I. -I$(TARGET_PATH_INCLUDE) \
			  -I$(TARGET_PATH_INCLUDE)/gstreamer-0.10 \
                          -I$(TARGET_PATH_INCLUDE)/glib-2.0 -I$(TARGET_PATH_LIB)/glib-2.0/include \
                          -I$(TARGET_PATH_INCLUDE)/libxml2 \
			  -I$(TARGET_PATH_INCLUDE)/opencv 

LFLAGS			= -Wl,--library-path=$(TARGET_PATH_LIB),-rpath-link=$(TARGET_PATH_LIB) -lm \
			  -lEGL -lCLC -lGLSLC -lVDK -lGLESv2 -lpthread \
			  -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 

#			  -lcxcore -lcv -lcvaux -lhighgui -lml \
			  

OBJECTS			= main.o gstcontrol.o  glutils.o glplane.o


first: all

all: $(APPNAME)

$(APPNAME): $(OBJECTS) 
	$(LD) $(LFLAGS) -o $(DESTDIR)/$(APPNAME) $(OBJECTS)  

main.o: main.cpp
	$(LD) $(CFLAGS) -c -o main.o main.cpp

gstcontrol.o: gstcontrol.cpp
	$(LD) $(CFLAGS) -c -o gstcontrol.o gstcontrol.cpp

glutils.o: glutils.cpp
	$(LD) $(CFLAGS) -c -o glutils.o glutils.cpp

glplane.o: glplane.cpp
	$(LD) $(CFLAGS) -lvpu -c -o  glplane.o glplane.cpp

clean:
	$(DEL_FILE) $(OBJECTS)
	$(DEL_FILE) *~ core *.core

distclean: clean
	$(DEL_FILE) $(DESTDIR)/$(APPNAME)


install: all

