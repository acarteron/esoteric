#### Common variables for Makefiles
TARGET=RG350VGA

## Directories
ROOT=$(shell pwd)

DIR_SRC=src
DIR_ASM=asm
DIR_HDR=include

DIR_BIN=bin
DIR_OBJ=obj
DIR_LIB=lib
DIR_DOC=doc
DIR_DST=dist
DIR_OPK=opk
DIR_LK=

CROSS_COMPILE=/mnt/storage/rg350/RG350_buildroot/output/host/usr/bin/mipsel-linux-

## Compilation
CC = $(CROSS_COMPILE)gcc
GXX = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip

SYSROOT     := $(shell $(CC) --print-sysroot)
SDL_CONFIG  := $(SYSROOT)/usr/bin/sdl-config
SDL_CFLAGS  := $(shell $(SDL_CONFIG) --cflags)
SDL_LIBS    := $(shell $(SDL_CONFIG) --libs)


DEBUG ?= 2
CFLAGS = \
	-ggdb \
    -std=gnu++11 \
	-shared-libgcc \
	-DTARGET_$(TARGET) \
    -DLOG_LEVEL=$(DEBUG) \
    -DHAVE_LIBOPK \
	-DENABLE_INOTIFY \
    -Wno-narrowing \
	$(SDL_CFLAGS) \
    -O2 \
    -finline-functions \
    -mips32r2 \
    -fPIC \
	-flto \
	-fexceptions \
    -mplt \
    -msym32 \
	-funroll-loops \
	-fomit-frame-pointer

CXXFLAGS = $(CFLAGS)
LDFLAGS = $(SDL_LIBS) -lfreetype -lSDL_image -lSDL_ttf -lSDL_gfx -lSDL_mixer -lopk -lpng -lasound # -L$(DIR_LK)/$(DIR_LIB)
CFLAGS_OBJ=-I $(ROOT)/$(DIR_HDR) # -I $(DIR_LK)/$(DIR_HDR) # -Wall -Wextra 

