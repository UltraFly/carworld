
#generic opengl application Makefile, Marcus Hewat 1999
#you can probably use this makefile for your opengl app by changing the APPNAME variable
#send patches to hewat@club-internet.fr

APPNAME=carworld
VERSION=0.223
RELEASE=1
SRC_COMMON=src
DEST=obj
BIN=bin
DEP=dep
DOC=doc

LIBS_X11=-lm -lX11 -lXext -lGL

#switch on the unix name
#to set compiler, OS and API specific variables
#if uname is CYGWIN I figure we are under Windows
#should find something better...

ifeq ($(shell uname),CYGWIN_98-4.10)
	EXECUTABLE=win_$(APPNAME).exe
	API=win32
	CPLUSPLUS=cl.exe
	LINK=link.exe
	CPPFLAGS=-nologo -ML -W3 -GR -GX -D WIN32 -D _WINDOWS -FD -Fo
	CPPFLAGS_RELEASE=-D NDEBUG -Ox
	CPPFLAGS_DEBUG=-D DEBUG -FR"$(DEST)\\" #-YX /Fd"$(DEST)\\" -GZ
	LDFLAGS=opengl32.lib glu32.lib dxguid.lib dinput.lib kernel32.lib \
		user32.lib gdi32.lib ws2_32.lib -nologo -subsystem:windows -incremental:no \
		-pdb:"$(DEST)\win_carw.pdb" -machine:I386 -out:
endif
ifeq ($(shell uname),BeOS)
	EXECUTABLE=be_$(APPNAME)
	API=beos
	CPLUSPLUS=g++
	LINK=$(CPLUSPLUS)
	CPPFLAGS=-Wall -ansi -Wno-multichar -D USE_POSIX_SOCKETS -o 
	CPPFLAGS_RELEASE=-O3
	CPPFLAGS_DEBUG=-g
	LDFLAGS=-lbe -lGL -lgame -ldevice -o
endif
ifeq ($(shell uname),Linux)
	EXECUTABLE=linux_$(APPNAME)
	API=x11
	CPLUSPLUS=g++
	LINK=$(CPLUSPLUS)
	CPPFLAGS=-Wall -ansi -I/usr/X11R6/include -I/usr/include/X11 -D USE_POSIX_SOCKETS -D USE_LIN_JOY -o
	CPPFLAGS_RELEASE=-O3
	CPPFLAGS_DEBUG=-g
	LDFLAGS=-L/usr/X11R6/lib $(LIBS_X11) -o
endif
ifeq ($(shell uname),SunOS)
	EXECUTABLE=sun_$(APPNAME)
	API=x11
	CPLUSPLUS=g++
	LINK=$(CPLUSPLUS)
	CPPFLAGS=-Wall -ansi -I/usr/include/X11 -D USE_POSIX_SOCKETS -o 
	CPPFLAGS_RELEASE=-O3
	CPPFLAGS_DEBUG=-g
	LDFLAGS=-L/usr/openwin/lib -R/usr/openwin/lib $(LIBS_X11) -lm -lsocket -lnsl -o 
endif
#the following are OSes are untested and might need tweeking
ifeq ($(shell uname),HP-UX)
	EXECUTABLE=hp_$(APPNAME)
	API=x11
	CPLUSPLUS=g++
	LINK=$(CPLUSPLUS)
	CPPFLAGS=-Wall -D USE_POSIX_SOCKETS -o
	CPPFLAGS_RELEASE=-O9
	CPPFLAGS_DEBUG=-g
	LDFLAGS=$(LIBS_X11) -lm -o
endif
ifeq ($(shell uname),IRIX)
	EXECUTABLE=sgi_$(APPNAME)
	API=x11
	CPLUSPLUS=CC
	LINK=$(CPLUSPLUS)
	CPPFLAGS=-D USE_POSIX_SOCKETS -o
	CPPFLAGS_RELEASE=-O
	CPPFLAGS_DEBUG=-g
	#no -lXext?
	LDFLAGS=$(LIBS_X11) -lm -o
endif

#add API specific source directory to the source directories
SRC=$(SRC_COMMON):src/$(API)

DEP_FILE=$(DEP)/$(API).dep

#tell make where to find the files
vpath %.h $(SRC)
vpath %.cpp $(SRC)
vpath %.o $(DEST)

#create object file names from the source file names
src_dirs=$(subst :, ,$(SRC))
source_files=$(foreach dir,$(src_dirs),$(wildcard $(dir)/*.cpp))
header_files=$(foreach dir,src src/win32 src/x11 src/beos,$(wildcard $(dir)/*.h))
OBJECTS=$(notdir $(patsubst %.cpp,%.o,$(source_files)))

#user targets
#warning: default build is debug
debug: $(BIN)
	$(MAKE) $(BIN)/$(EXECUTABLE) CPPFLAGS="$(CPPFLAGS_DEBUG) $(CPPFLAGS)"

release: $(BIN)
	$(MAKE) $(BIN)/$(EXECUTABLE) CPPFLAGS="$(CPPFLAGS_RELEASE) $(CPPFLAGS)"
	strip $(BIN)/$(EXECUTABLE)

install: release
	-rm -rf /usr/bin/$(APPNAME) /usr/share/$(APPNAME)
	install -m 755 -o 0 -g 0 $(BIN)/$(EXECUTABLE) /usr/bin/$(APPNAME)
	install -d -m 755 -o 0 -g 0 /usr/share/$(APPNAME)
	cp -r data /usr/share/$(APPNAME)

uninstall:
	-rm -rf /usr/bin/$(APPNAME) /usr/share/$(APPNAME)

clean:
	-rm -rf $(DEST) $(BIN)/$(EXECUTABLE)

realclean:
	-rm -rf $(DEST) $(BIN) $(DOC) $(APPNAME)-$(VERSION)-$(RELEASE).spec log.txt

#BUG the dependency file needs to have its directory names removed manualy...
#makedepend -f- -Y -Isrc:src:win32 src/*.cpp src/win32/*.cpp > dep/win32.dep
#makedepend -f- -Y -Isrc:src:beos src/*.cpp src/beos/*.cpp > dep/beos.dep
depend:
	-makedepend -f- -Y -I$(SRC) $(source_files) > $(DEP_FILE)

doc: $(header_files)
	-rm -rf $@
	doc++ -A -c -v -d $(DOC) -F $(header_files)

rpm: realclean spec
	-rm -rf ../$(APPNAME)-$(VERSION)
	cp -r ../$(APPNAME) ../$(APPNAME)-$(VERSION)
	cd .. ; tar -cf /usr/src/RPM/SOURCES/$(APPNAME)-$(VERSION).tar $(APPNAME)-$(VERSION)
	rm -f /usr/src/RPM/SOURCES/$(APPNAME)-$(VERSION).tar.gz
	gzip /usr/src/RPM/SOURCES/$(APPNAME)-$(VERSION).tar
	rpm -bb --target i386 $(APPNAME)-$(VERSION)-$(RELEASE).spec


#internal targets
$(BIN)/$(EXECUTABLE): $(BIN) $(DEST) $(OBJECTS)
	cd $(DEST) ; $(LINK) $(OBJECTS) $(LDFLAGS)../$@

$(DEST):
	-mkdir $(DEST)

$(BIN):
	-mkdir $(BIN)

$(OBJECTS): %.o: %.cpp
	$(CPLUSPLUS) $(CPPFLAGS)$(DEST)/$@ -c $<

#this is the text used to generate the spec file for the RPM package
SPEC_TEXT="\
\nSummary: car simulation useing OpenGL for rendering\
\nName: $(APPNAME)\
\nVersion: $(VERSION)\
\nRelease: $(RELEASE)\
\nCopyright: GPL\
\nGroup: Amusements/Games\
\nSource: perso.club-internet.fr/hewat/$(APPNAME)/$(APPNAME)-$(VERSION).tar.gz\
\n\
\n%description\
\nCar simulaton with an emphasis on dynamics useing OpenGL for rendering,\
\nwith support for networked mode (up to 8 drivers).\
\n\
\n%prep\
\n%setup\
\n\
\n%build\
\nmake release\
\n\
\n%install\
\nmake install\
\n\
\n%files\
\n%doc README\
\n/usr/bin/$(APPNAME)\
\n/usr/share/$(APPNAME)"

spec:
	echo -e $(SPEC_TEXT) > $(APPNAME)-$(VERSION)-$(RELEASE).spec

include $(DEP_FILE)
