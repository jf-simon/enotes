MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = enotes

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-I$(top_builddir)/src/bin/ \
-I$(top_srcdir)/src/bin/ \
-I$(top_builddir)/src/lib/ \
-I$(top_srcdir)/src/lib/ \
@EFL_CFLAGS@

enotes_SOURCES = enotes_main.c
# enotes_SOURCES = enotes_main.c enotes_caldav.c
enotes_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libenotes.la

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = enotes_private.h
