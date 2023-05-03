PROJECT := vextm-source
DLL := $(PROJECT).dll
INSTALLER := VEXTMOBSPluginInstaller.exe
MAKENSIS := ../vextm/buildtools/win32/nsis/makensis

SRCS = vextm-source.c \
	   vextm-thread.c \
	   colorbars.c

OBS_SOURCE := $(HOME)/git/obs-studio
OBS_INSTALL := "$(shell cygpath -u "$$PROGRAMFILES/obs-studio")"

CC = x86_64-w64-mingw32-gcc
STRIP = x86_64-w64-mingw32-strip

CFLAGS = -m64 -Wall
INCLUDES = -I$(OBS_SOURCE)/libobs
LIBS = -L$(OBS_INSTALL)/bin/64bit -static-libgcc -mwindows -Wl,-Bdynamic -lobs -Wl,-Bstatic -lpthread

OBJS += ${SRCS:.c=.o}

.PHONY: install clean all

all:: $(INSTALLER)

$(DLL): $(OBJS)
	@echo "Linking $@..."
	$(CC) -shared $(LIBS) -o $@ $(OBJS)

$(INSTALLER): setup.nsi $(DLL)
	@echo ""
	@echo "#####################################################"
	@echo "# CREATING $@ INSTALLER EXECUTABLE..."
	@$(STRIP) $(DLL)
	@$(MAKENSIS) "/XOutFile $@" $< > $@.out
	@echo "#####################################################"
	@echo ""

.c.o:
	@echo "Compiling $<..."
	@$(CC) -c $< -o $@ $(CFLAGS) $(CXXFLAGS) $(INCLUDES)

clean:
	rm -f $(DLL)
	rm -f $(INSTALLER)
	rm -f $(OBJS)
	rm -rf dist
	rm -f $(PROJECT).zip
	rm -f *.out
