# vim:set sw=8 nosta:

ifeq ($(TARGET),i386-mingw32msvc)
 DEBUG=no
else
 DEBUG=yes
endif

CFLAGS=-Wall -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS)
LDLIBS=
LDFLAGS=
REVISIONS=-DREVISION=\"$(shell svn info | perl -ne 'if(/Revision: (\d+)/){print $$1;}')\" \
	-DTOMITCP_REV=\"$(shell svn info TomiTCP | perl -ne 'if(/Revision: (\d+)/){print $$1;}')\" \

LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

ifeq ($(TARGET),i386-mingw32msvc)
 LDLIBS += -liconv -lws2_32
 LDFLAGS += -Wl,--wrap -Wl,strerror
endif

.PHONY: all clean dep dummy

all: libxchat-bttrw.a gate

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)

-include .depend

libxchat-bttrw.a: xchat.o roomtext.o login.o room.o irc.o idle.o smiles.o \
                  charset.o list.o
	$(AR) rsv $@ $?

gate: gate.o md5.o libxchat-bttrw.a TomiTCP/libTomiTCP.a
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
ifeq ($(TARGET),i386-mingw32msvc)
	strip $@
endif

gate.o: gate.cc
	$(COMPILE.cc) $(REVISIONS) $(OUTPUT_OPTION) $<

clean:
	$(RM) libxchat-bttrw.a gate *.o
#	$(MAKE) -C TomiTCP clean

TomiTCP/libTomiTCP.a: dummy
	$(MAKE) -C TomiTCP DEBUG=$(DEBUG)
