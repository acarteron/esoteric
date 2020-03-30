TARGET=LINUX

CC = gcc
GXX = g++
STRIP = strip
include ./compile_def.mk


DEBUG ?= 2
CFLAGS = -ggdb \
	$(SDL_CFLAGS) \
    -I/usr/local/include/ \
	-shared-libgcc \
    -std=gnu++11 \
	-DTARGET_$(TARGET) \
    -DLOG_LEVEL=$(DEBUG) \
#    -DHAVE_LIBOPK \
	-DENABLE_INOTIFY \
	-fexceptions \
    -Wno-narrowing  

CXXFLAGS := $(CFLAGS)
LDFLAGS := $(SDL_LIBS) -lSDL_image -lSDL_ttf -lSDL_gfx -lSDL_mixer -lfreetype -lpthread -lpng -lasound #-lopk
CFLAGS_OBJ=-I $(ROOT)/$(DIR_HDR) # -I $(DIR_LK)/$(DIR_HDR) # -Wall -Wextra 

SRC_IGNORE=$(ROOT)/$(DIR_SRC)/imageio.cpp $(ROOT)/$(DIR_SRC)/linkscannerdialog.cpp
