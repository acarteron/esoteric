#### Common variables for Makefiles
TARGET=RG350VGA

CROSS_COMPILE=/opt/gcw0-toolchains/5.4/usr/bin/mipsel-linux-

## Compilation
CC = $(CROSS_COMPILE)gcc
GXX = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip
include ./compile_def.mk


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
LDFLAGS = -L$(SYSROOT)/usr/lib/ $(SDL_LIBS)  -lfreetype -lSDL_image -lSDL_ttf -lSDL_gfx -lSDL_mixer -lopk -lpng -lasound # -L$(DIR_LK)/$(DIR_LIB)
CFLAGS_OBJ=-I $(ROOT)/$(DIR_HDR) # -I $(DIR_LK)/$(DIR_HDR) # -Wall -Wextra 
SRC_IGNORE=false

