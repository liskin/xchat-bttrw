# vim:set sw=8 nosta:

ifeq ($(TARGET),i386-mingw32msvc)
 DEBUG=no
else
 DEBUG=yes
endif

WINDRES=windres
CFLAGS=-Wall -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS)
LDLIBS=
LDFLAGS=
REVISIONS=-DREVISION=\"$(shell svn info | perl -ne 'if(/Last Changed Rev: (\d+)/){print $$1;}')\" \
	-DTOMITCP_REV=\"$(shell svn info TomiTCP | perl -ne 'if(/Last Changed Rev: (\d+)/){print $$1;}')\"
RSRC=

LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

ifeq ($(TARGET),i386-mingw32msvc)
 LDLIBS += -lws2_32
 ICONV=external
 RSRC=rsrc.o
endif

ifdef WIN32_COMPAT
 GATE_WIN32_COMPAT=-DWIN32_COMPAT=\"$(WIN32_COMPAT)\"
endif

ifeq ($(ICONV),external)
 LDLIBS += -liconv
endif

.PHONY: all clean dep dummy buildw32

all: libxchat-bttrw.a gate README

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)

-include .depend

libxchat-bttrw.a: xchat.o roomtext.o login.o room.o irc.o idle.o smiles.o \
                  charset.o list.o ison.o userinfo.o
	$(AR) rsv $@ $?

gate: gate.o md5.o setproctitle.o libxchat-bttrw.a TomiTCP/libTomiTCP.a $(RSRC)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
ifeq ($(TARGET),i386-mingw32msvc)
	strip $@
endif

gate.o:
	$(COMPILE.cc) $(REVISIONS) $(GATE_WIN32_COMPAT) $(OUTPUT_OPTION) $<

clean:
	$(RM) libxchat-bttrw.a gate *.o
#	$(MAKE) -C TomiTCP clean

TomiTCP/libTomiTCP.a: dummy
	$(MAKE) -C TomiTCP DEBUG=$(DEBUG)

%.o: %.rc
	$(WINDRES) -o $@ $^

# docs
README: gate.cc
	./mkreadme

# Win32 building
DESTDIR=xchat-bttrw-win32
buildw32:
	$(RM) $(filter-out $(DESTDIR)/build.log, $(wildcard $(DESTDIR)/*))
	$(MAKE) clean dep
	$(MAKE) -C TomiTCP clean dep
	@echo
	@echo -e '\033[1m' Building for WinXP and up... '\033[m'
	@echo
	$(MAKE) WIN32_COMPAT=winxp
	cp -L gate $(DESTDIR)/gate.exe
	@echo
	
	touch gate.cc
	$(MAKE) -C TomiTCP clean dep
	@echo
	@echo -e '\033[1m' Building for Win2000 and up... '\033[m'
	@echo
	$(MAKE) WIN32_COMPAT=win2k
	cp -L gate $(DESTDIR)/gate-win2k.exe
	@echo
	
	touch gate.cc
	$(MAKE) -C TomiTCP clean dep
	@echo
	@echo -e '\033[1m' Building for Win98, WinNT4 and up... '\033[m'
	@echo
	$(MAKE) WIN32_COMPAT=winnt
	cp -L gate $(DESTDIR)/gate-win98.exe
	@echo
	
	cp -L COPYRIGHT $(DESTDIR)/
	cp -L COPYING $(DESTDIR)/
	cp -L README $(DESTDIR)/README.txt
	cp -L $(shell which libiconv-2.dll) $(DESTDIR)/
