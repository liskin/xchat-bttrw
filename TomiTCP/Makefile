# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS)
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

COMPONENTS=net http cookies str
OBJECTS=$(patsubst %,%.o,$(COMPONENTS))

.PHONY: all clean dep

all: libTomiTCP.a pok .depend

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)
	
-include .depend

clean:
	$(RM) *.{o,so,a} pok
libTomiTCP.a: $(OBJECTS)
	$(AR) rsv $@ $?

pok: pok.o libTomiTCP.a
