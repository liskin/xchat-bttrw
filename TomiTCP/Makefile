# vim:set sw=8 nosta:

DEBUG=no

CFLAGS=-Wall -D_GNU_SOURCE -fPIC -DPIC
CXXFLAGS=$(CFLAGS)
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

.PHONY: all clean

all: libTomiTCP.a pok
clean:
	$(RM) *.{o,so,a}
libTomiTCP.a: net.o http.o cookies.o
	$(AR) rsv $@ $?
net.o: net.cc net.h
http.o: http.cc http.h net.h str.h
cookies.o: cookies.cc cookies.h
pok: pok.o libTomiTCP.a
