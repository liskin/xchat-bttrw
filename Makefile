# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE -I./TomiTCP
CXXFLAGS=$(CFLAGS)
LDLIBS=
LDFLAGS=
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

.PHONY: all clean dep dummy

all: xchat-bttrw

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)

-include .depend

clean:
	$(RM) xchat-bttrw *.o
	$(MAKE) -C TomiTCP clean
xchat-bttrw: main.o TomiTCP/libTomiTCP.a
TomiTCP/libTomiTCP.a: dummy
	$(MAKE) -C TomiTCP
