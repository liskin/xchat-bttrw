# vim:set sw=8 nosta:

TARGET=$(shell $(CC) -dumpmachine)

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE -O2
CXXFLAGS=$(CFLAGS)
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

OBJECTS=net.o http.o cookies.o str.o

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

ifneq (,$(findstring mingw32,$(TARGET)))
 LDLIBS += -lws2_32
 CFLAGS += -mthreads
 LDFLAGS += -mthreads
 OBJECTS += win32.o
endif

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
