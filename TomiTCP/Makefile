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

all: libTomiTCP.a
clean:
	$(RM) *.{o,so,a}
libTomiTCP.a: net.o
	$(AR) rsv $@ $<
net.o: net.cc net.h
