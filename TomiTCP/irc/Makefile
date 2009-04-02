# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS)
LDLIBS=-ldl
LDFLAGS=-rdynamic
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

%.so: %.cc irc.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -fPIC -DPIC -shared $(TARGET_ARCH) $< $(LOADLIBES) -o $@

.PHONY: all clean dep

all: irc $(patsubst %.cc,%.so,$(wildcard m_*.cc))

MAKEDEP=gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)
	
-include .depend

clean:
	$(RM) irc *.{o,so}
irc: irc.o ../libTomiTCP.a

m_xchatsux.so: m_xchatsux.cc irc.h smiles.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -fPIC -DPIC -shared $(TARGET_ARCH) $< smiles.o $(LOADLIBES) -o $@
