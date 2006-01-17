# vim:set sw=8 nosta:

ifeq ($(TARGET),i386-mingw32msvc)
 DEBUG=no
else
 DEBUG=yes
endif

WINDRES=windres
CFLAGS=-Wall -D_GNU_SOURCE -O2
CXXFLAGS=$(CFLAGS)
LDLIBS=
LDFLAGS=
REVISIONS=-DREVISION=\"$(shell svn info | perl -ne 'if(/Last Changed Rev: (\d+)/){print $$1;}')\" \
	-DTOMITCP_REV=\"$(shell svn info TomiTCP | perl -ne 'if(/Last Changed Rev: (\d+)/){print $$1;}')\"
RSRC=

LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifeq ($(DEBUG),yes)
 CFLAGS += -g -DDEBUG
else
 CFLAGS += -DNDEBUG
endif

ifeq ($(TARGET),i386-mingw32msvc)
 LDLIBS += -lws2_32
 RSRC=rsrc.o
endif

ifdef WIN32_COMPAT
 GATE_WIN32_COMPAT=-DWIN32_COMPAT=\"$(WIN32_COMPAT)\"
endif

.PHONY: all clean dep conf dummy buildw32 docs

all: libxchat-bttrw.a gate README

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


libxchat-bttrw.a: xchat.o roomtext.o login.o room.o irc.o idle.o smiles.o \
                  charset.o list.o ison.o userinfo.o
	$(AR) rsv $@ $?

gate: gate.o md5.o setproctitle.o libxchat-bttrw.a TomiTCP/libTomiTCP.a $(RSRC)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
ifeq ($(TARGET),i386-mingw32msvc)
	strip $@
endif

gate.o:
	$(COMPILE.cc) $(REVISIONS) $(GATE_WIN32_COMPAT) $(OUTPUT_OPTION) $<

clean:
	$(RM) libxchat-bttrw.a gate *.o
#	$(MAKE) -C TomiTCP clean

TomiTCP/libTomiTCP.a: dummy
	$(MAKE) -C TomiTCP DEBUG=$(DEBUG)

%.o: %.rc
	$(WINDRES) -o $@ $^

# docs
README: gate.cc
	./mkreadme

# Win32 building
DESTDIR=xchat-bttrw-win32
buildw32:
	$(RM) $(filter-out $(DESTDIR)/build.log, $(wildcard $(DESTDIR)/*))
	$(MAKE) clean dep conf
	$(MAKE) -C TomiTCP clean dep conf
	@echo
	@echo -e '\033[1m' Building for WinXP and up... '\033[m'
	@echo
	$(MAKE) WIN32_COMPAT=winxp
	cp -L gate $(DESTDIR)/gate.exe
	@echo
	
	touch gate.cc
	$(MAKE) -C TomiTCP clean dep
	@echo
	@echo -e '\033[1m' Building for Win2000 and up... '\033[m'
	@echo
	$(MAKE) WIN32_COMPAT=win2k
	cp -L gate $(DESTDIR)/gate-win2k.exe
	@echo
	
	touch gate.cc
	$(MAKE) -C TomiTCP clean dep
	@echo
	@echo -e '\033[1m' Building for Win98, WinNT4 and up... '\033[m'
	@echo
	$(MAKE) WIN32_COMPAT=winnt
	cp -L gate $(DESTDIR)/gate-win98.exe
	@echo
	
	cp -L COPYRIGHT $(DESTDIR)/
	cp -L COPYING $(DESTDIR)/
	cp -L README $(DESTDIR)/README.txt
	cp -L $(shell which libiconv-2.dll) $(DESTDIR)/

# Documentation
docs_zmp:
	$(RM) -r docs/
	doxygen docs_zmp.cfg
	perl -i -pe ' \
		s/^\\usepackage(\[.*?\])?\{(textcomp|alltt)\}//g; \
		s/\\setcounter\{tocdepth\}\{1\}/\\setcounter{tocdepth}{2}/; \
		s/(\\section)/\\clearemptydoublepage$$1/g; \
		s/(\\tableofcontents)/$$1\\clearemptydoublepage/; \
		s/(\\end\{titlepage\})/$$1\\clearemptydoublepage/; \
		s/(\\printindex)/\\clearemptydoublepage\\pagestyle{plain}$$1/; \
		s/(\\documentclass\[a4paper)(\]\{article\})/$$1,12pt,titlepage,twoside$$2/; \
	' docs/latex/refman.tex
	perl -i -e ' \
		open(my $$f, "docs_zmp_header.tex") or die $$!; \
		my $$titlepage = join("", <$$f>); \
		close($$f); \
		while (<>) { \
			if (/\\begin\{titlepage\}/) { \
				print; \
				print $$titlepage; \
				while ($$_ = <> and not /\\end\{titlepage\}/) {} \
				print; \
			} else { print; } \
		} \
	' docs/latex/refman.tex
	perl -i -pe ' \
		s/^\\usepackage(\[.*?\])?\{(fontenc)\}//g; \
		s/\\leftmark/\\rightmark/g; \
	' docs/latex/doxygen.sty
	$(MAKE) -C docs/latex
