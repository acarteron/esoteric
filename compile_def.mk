SYSROOT     := $(shell $(CC) --print-sysroot)
SDL_CONFIG  := $(SYSROOT)/usr/bin/sdl-config
SDL_CFLAGS  := $(shell $(SDL_CONFIG) --cflags)
SDL_LIBS    := $(shell $(SDL_CONFIG) --libs)
