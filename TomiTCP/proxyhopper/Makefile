# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS)
LDLIBS=-lTomiTCP
LDFLAGS=-L..
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

.PHONY: all clean dep

all: proxyhopper

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)
	
-include .depend

clean:
	$(RM) proxyhopper *.o
proxyhopper: proxyhopper.o
