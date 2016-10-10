INCLUDES=-I/etc/X11 -I$(pvr_PATH)/Include/ -I. -I./glm -I$(pvr_PATH)/Tools -I$(pvr_PATH)/Tools/OGLES2
CPP=g++
CC=gcc
CFLAGS=${INCLUDES} -DSTANDALONE -DSRV_USE_JOYSTICK -g
GLTOOLS_SRC_PATH=../GLTools/src
LIBDIRS = -L$(pvr_PATH)/Library/Linux_x86_64 -L$(pvr_PATH)/Tools/OGLES2/Build/Linux_x86_64/ReleaseX11
#/usr/X11R6/lib -L/usr/X11R6/lib64 -L/usr/local/lib
LDFLAGS= ${LIBDIRS} -lEGL -lGLESv2 -lX11  -logles2tools

.PHONY: all srv 

all: srv
 
srv:
	g++ ${CFLAGS} standalone.cxx \
		sgxRender3DSRV.cpp \
		textures.cpp \
		car.cpp \
		single_view.c \
		${LDFLAGS} \
		-o srv
