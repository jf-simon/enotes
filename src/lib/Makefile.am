MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_CFLAGS@ \
-DEFL_EFL_BUILD

lib_LTLIBRARIES = libenotes.la

includes_HEADERS = Enotes.h
includesdir = $(includedir)/enotes-@VMAJ@

libenotes_la_SOURCES = enotes.c
libenotes_la_LIBADD = @EFL_LIBS@ -lm
libenotes_la_LDFLAGS = @EFL_LTLIBRARY_FLAGS@
