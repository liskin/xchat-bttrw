# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE -I..
CXXFLAGS=$(CFLAGS)
LDLIBS=-ldl -lTomiTCP
LDFLAGS=-rdynamic -L..
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

%.so: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -fPIC -DPIC -shared $(TARGET_ARCH) $< $(LOADLIBES) -o $@

.PHONY: all clean

all: irc $(patsubst %.cc,%.so,$(wildcard m_*.cc))
clean:
	$(RM) irc *.{o,so}
irc: irc.o
irc.o: irc.cc irc.h ../net.h
m_pokus.so: m_pokus.cc irc.h ../net.h
m_nomi.so: m_nomi.cc irc.h ../net.h
m_rejoin.so: m_rejoin.cc irc.h ../net.h
m_channel.so: m_channel.cc irc.h ../net.h
m_take.so: m_take.cc irc.h ../net.h
