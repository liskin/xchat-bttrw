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

%.so: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -fPIC -DPIC -shared $(TARGET_ARCH) $< $(LOADLIBES) -o $@

.PHONY: all clean

all: pok irc $(patsubst %.cc,%.so,$(wildcard m_*.cc))
clean:
	$(RM) pok irc *.{o,so}
pok: pok.o net.o
irc: irc.o net.o
pok.o: pok.cc net.h
net.o: net.cc net.h
irc.o: irc.cc net.h irc.h
m_pokus.so: m_pokus.cc net.h irc.h
m_nomi.so: m_nomi.cc net.h irc.h
