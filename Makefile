
#generic opengl/SDL application Makefile, Marcus Hewat 1999
#you can probably use this makefile for your opengl/SDL app by changing the APPNAME variable
#send patches to hewat@users.sourceforge.net

APPNAME=carworld
VERSION=0.245
RELEASE=1
SRC=src
DEST=obj
DOC=doc

EXECUTABLE=$(APPNAME)
CPLUSPLUS=g++
LINK=$(CPLUSPLUS)
DEFAULT_CPPFLAGS=-Wall -ansi `sdl-config --cflags` -DUSE_POSIX_SOCKETS -o
CPPFLAGS=$(DEFAULT_CPPFLAGS)
CPPFLAGS_RELEASE=-O3
CPPFLAGS_DEBUG=-g
DEFAULT_LD_FLAGS=-lm -lGL -lGLU `sdl-config --libs` -lSDL_image -o
LDFLAGS=$(DEFAULT_LD_FLAGS)

#switch on the unix name
#to set compiler and OS specific variables
#if uname is CYGWIN I figure we are under Windows
#should find something better...

ifeq ($(shell uname),Linux)
	LDFLAGS=-L/usr/X11R6/lib -lpthread $(DEFAULT_LD_FLAGS)
endif

ifeq ($(shell uname),CYGWIN_98-4.10)
	EXECUTABLE=$(APPNAME).exe
	CPLUSPLUS=cl.exe
	LINK=link.exe
	CPPFLAGS=-nologo -ML -W3 -GR -GX -D WIN32 -D _WINDOWS -FD -Fo
	CPPFLAGS_RELEASE=-D NDEBUG -Ox
	CPPFLAGS_DEBUG=-D _DEBUG -FR"$(DEST)\\" #-YX /Fd"$(DEST)\\" -GZ
	LDFLAGS=opengl32.lib dxguid.lib dinput.lib SDL.lib SDLmain.lib ws2_32.lib -nologo -subsystem:windows -incremental:no \
		-pdb:"$(DEST)\$(APPNAME).pdb" -machine:I386 -out:
endif

ifeq ($(shell uname),Darwin)
	CPPFLAGS=`sdl-config --libs` -framework OpenGL -Wall -ansi -Wno-multichar -D USE_POSIX_SOCKETS -o
	LDFLAGS=`sdl-config --cflags --libs` -lSDL_image -framework OpenGL -Wall -ansi -Wno-multichar -o 
endif


ifeq ($(shell uname),SunOS)
	CPPFLAGS=-I/usr/include/X11 $(DEFAULT_CPPFLAGS)
	LDFLAGS=-L/usr/openwin/lib -R/usr/openwin/lib $(DEFAULT_LD_FLAGS) -lsocket -lnsl -o 
endif

ifeq ($(shell uname),MorphOS)
	LINK=$(DEFAULT_CPPFLAGS) -noixemul
endif

DEP_FILE=$(APPNAME).dep

#tell make where to find the files
vpath %.h $(SRC)
vpath %.cpp $(SRC)
vpath %.o $(DEST)

#create object file names from the source file names
src_dirs=$(subst :, ,$(SRC))
source_files=$(foreach dir,$(src_dirs),$(wildcard $(dir)/*.cpp))
header_files=$(foreach dir,src src/win32 src/x11 src/beos,$(wildcard $(dir)/*.h))
OBJECTS=$(notdir $(patsubst %.cpp,%.o,$(source_files)))

release:
	$(MAKE) $(EXECUTABLE) CPPFLAGS="$(CPPFLAGS_RELEASE) $(CPPFLAGS)"
	strip $(EXECUTABLE)

debug:
	$(MAKE) $(EXECUTABLE) CPPFLAGS="$(CPPFLAGS_DEBUG) $(CPPFLAGS)"

install:
	-rm -rf $(RPM_BUILD_ROOT)
	mkdir -p $(RPM_BUILD_ROOT)/usr/bin
	mkdir -p $(RPM_BUILD_ROOT)/usr/share
	install -m 755 -o 0 -g 0 $(EXECUTABLE) $(RPM_BUILD_ROOT)/usr/bin/$(EXECUTABLE)
	install -d -m 755 -o 0 -g 0 $(RPM_BUILD_ROOT)/usr/share/$(APPNAME)
	cp -r data /usr/share/$(APPNAME)

uninstall:
	-rm -rf $(RPM_BUILD_ROOT)/usr/bin/$(APPNAME) $(RPM_BUILD_ROOT)/usr/share/$(APPNAME)

clean:
	-rm -rf $(DEST) $(EXECUTABLE)

realclean: clean
	-rm -rf $(DOC) *.spec log.txt

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
$(EXECUTABLE): $(DEST) $(OBJECTS)
	cd $(DEST) ; $(LINK) $(OBJECTS) $(LDFLAGS)../$@

$(DEST):
	-mkdir $(DEST)

$(OBJECTS): %.o: %.cpp
	$(CPLUSPLUS) $(CPPFLAGS)$(DEST)/$@ -c $<

#this is the text used to generate the spec file for the RPM package
SPEC_TEXT="\
\nSummary: car simulation using OpenGL for rendering\
\nName: $(APPNAME)\
\nVersion: $(VERSION)\
\nRelease: $(RELEASE)\
\nCopyright: GPL\
\nGroup: Amusements/Games\
\nSource: http://superb-west.dl.sourceforge.net/sourceforge/$(APPNAME)/$(APPNAME)-$(VERSION).tar.gz\
\nBuildRoot: /var/tmp/%{name}-buildroot\
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
\n%doc readme.txt\
\n/usr/bin/$(APPNAME)\
\n/usr/share/$(APPNAME)"

spec:
	echo -e $(SPEC_TEXT) > $(APPNAME)-$(VERSION)-$(RELEASE).spec

#include $(DEP_FILE)
