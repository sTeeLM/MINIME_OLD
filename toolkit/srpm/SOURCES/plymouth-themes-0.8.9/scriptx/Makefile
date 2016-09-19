CC      := gcc
LD      := ld

PLYMOUTH_LOGO_FILE=/usr/share/pixmaps/system-logo-white.png

PLY_SPLASH_CORE_INCLUDES=$(shell pkg-config --cflags-only-I ply-splash-core)
PLY_SPLASH_GRAPHICS_INCLUDES=$(shell pkg-config --cflags-only-I ply-splash-graphics)

PLY_SPLASH_CORE_LIBS=$(shell pkg-config --libs ply-splash-core)
PLY_SPLASH_GRAPHICS_LIBS=$(shell pkg-config --libs ply-splash-graphics)

TARGET_LIB := scriptx.so
CFLAGS_DEFINES = -D_GNU_SOURCE -DPLYMOUTH_LOGO_FILE=\"$(PLYMOUTH_LOGO_FILE)\" -fPIC
CFLAGS  := -I./ $(PLY_SPLASH_CORE_INCLUDES) $(PLY_SPLASH_GRAPHICS_INCLUDES) $(CFLAGS_DEFINES)
LDFLAGS := -shared -fpic -Wl,-soname,$(TARGET_LIB) $(PLY_SPLASH_CORE_LIBS) $(PLY_SPLASH_GRAPHICS_LIBS)
SOURCE  := $(wildcard *.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))

BUILT_SOURCES = script-lib-image.script.h                                     \
                script-lib-sprite.script.h                                    \
                script-lib-plymouth.script.h                                  \
                script-lib-math.script.h                                      \
                script-lib-string.script.h

all:$(BUILT_SOURCES) $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET_LIB) $(OBJS) 
 
%.o:%.c
	@echo Compiling $< ...
	$(CC) -c $(CFLAGS)  $< -o $*.o

%.script.h: %.script
	sed -e 's/\\/\\\\/g'                                                  \
	    -e 's/"/\\"/g'                                                    \
	    -e 's/\(.*\)/  "\1\\n"/g'                                         \
	    -e '1istatic const char* $(subst -,_,$(@:.script.h=_string)) ='   \
	    -e '$$a;'                                                         \
	    $< > $@

 
.PHONY: clean
 
clean:
	rm *.so *.o *.script.h -rf
