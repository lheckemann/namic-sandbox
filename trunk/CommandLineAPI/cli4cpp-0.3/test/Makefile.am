if CPPUNIT

INCLUDES = -I$(top_srcdir)/include 


noinst_PROGRAMS = test

test_SOURCES = BasicParserTest.cpp OptionTest.cpp OptionsTest.cpp PosixParserTest.cpp Test.cpp
test_LDADD = $(top_builddir)/src/libcli.la

.PHONY: valgrind

valgrind: test
  LD_LIBRARY_PATH=$(top_builddir)/src/.libs valgrind .libs/test

endif
