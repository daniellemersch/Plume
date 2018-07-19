CXX = g++
#CXX = $(shell wx-config --cxx)
LIBS := -lants

# Sous Windows, l'executable finit en .exe (pour strip)
ifeq ($(OS),Windows_NT)
PROGRAM = Plume.exe
else
PROGRAM = Plume
endif

ifndef OSTYPE
OSTYPE = $(shell uname)
endif

# Sets some variables under MacOS
ifneq (,$(findstring ${OSTYPE},Darwin))
WXCONFIG = /usr/local/bin/wx-config
#ARCH_FLAGS = -arch i386
ARCH_FLAGS =

#execute the script testlibpng.sh to test whether the libraries png and tiff are installed, 
# if so they are used for compilation
LIBS := $(LIBS) $(shell bash testlibpng.sh)
else
WXCONFIG = wx-config
ARCH_FLAGS =
endif

OBJECTS = mainwin.o main.o wx_plume.o wx_colorcode.o colorwin.o wx_frames.o frameswin.o

.SUFFIXES:      .o .cpp

.cpp.o :
	$(CXX) -c $(ARCH_FLAGS) -O3 `$(WXCONFIG) --cxxflags` -o $@ $(INCLUDE) $<

# Sous MacOS on a besoin d'une application...

ifneq (,$(findstring ${OSTYPE},Darwin))

#CPPFLAGS = -Wall -O3 -arch i386

all:    $(PROGRAM).app

$(PROGRAM).app:   $(PROGRAM)
#	`$(WXCONFIG) --rescomp` $(PROGRAM)
	mkdir -p $(PROGRAM).app
	mkdir -p $(PROGRAM).app/Contents
	mkdir -p $(PROGRAM).app/Contents/MacOS
	mkdir -p $(PROGRAM).app/Contents/Resources
	echo -n "APPL????" > $(PROGRAM).app/Contents/PkgInfo
	cp $(PROGRAM) $(PROGRAM).app/Contents/MacOS
	cp Info.plist $(PROGRAM).app/Contents
	cp app.icns $(PROGRAM).app/Contents/Resources
	rm -f $(PROGRAM)

else

all:  $(PROGRAM)

endif

$(PROGRAM):     $(OBJECTS)
	$(CXX)  $(ARCH_FLAGS) -o $(PROGRAM) $(OBJECTS) `$(WXCONFIG) --libs` $(LIBS)
	strip $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM) ../common/*.o
	rm -rf $(PROGRAM).app

.PHONY: all clean 
