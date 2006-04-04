# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE -O2
CXXFLAGS=$(CFLAGS)
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

ifeq ($(TARGET),i386-mingw32msvc)
 LDLIBS += -lws2_32
 CFLAGS += -mthreads
 LDFLAGS += -mthreads
 ifndef WIN32_COMPAT
  WIN32_COMPAT=winxp
 endif
 ifeq ($(WIN32_COMPAT),winnt)
  CFLAGS += -DWINVER=0x0400
 endif
 ifeq ($(WIN32_COMPAT),win2k)
  CFLAGS += -DWINVER=0x0500
 endif
 ifeq ($(WIN32_COMPAT),winxp)
  CFLAGS += -DWINVER=0x0501
 endif
endif

COMPONENTS=net http cookies str
OBJECTS=$(patsubst %,%.o,$(COMPONENTS))

.PHONY: all clean dep conf

all: libTomiTCP.a pok .depend

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)
	
-include .depend

MAKECONF=I=.conf-$$RANDOM.c; echo "main(){}" >$$I; \
	 echo -n >.config; O=.conf-$$RANDOM; \
	 if $(CC) -o $$O $$I -liconv 2>/dev/null; then \
	 echo ICONV=external >>.config; fi; $(RM) $$O $$I
conf:
	$(MAKECONF)
.config:
	$(MAKECONF)

-include .config

ifeq ($(ICONV),external)
 LDLIBS += -liconv
endif


clean:
	$(RM) *.{o,so,a} pok
libTomiTCP.a: $(OBJECTS)
	$(AR) rsv $@ $?

pok: pok.o libTomiTCP.a
