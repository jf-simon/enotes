
if EFL_HAVE_TESTS

check_PROGRAMS = enotes_tests

enotes_tests_SOURCES = test_enotes.c
enotes_tests_CPPFLAGS = -I$(top_builddir)/src/lib/ \
-DPACKAGE_TESTS_DIR=\"$(top_srcdir)/src/tests/\" \
-DPACKAGE_BUILD_DIR=\"`pwd`/$(top_builddir)/src/tests/\" \
@EFL_CFLAGS@ \
@CHECK_CFLAGS@
enotes_tests_LDADD = @CHECK_LIBS@ $(top_builddir)/src/lib/libenotes.la
enotes_tests_DEPENDENCIES = $(top_builddir)/src/lib/libenotes.la

endif

EXTRA_DIST = test_enotes.c
