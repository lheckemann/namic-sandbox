INCLUDES = -I$(top_srcdir)/include 

lib_LTLIBRARIES = libcli.la
libcli_la_SOURCES = BasicParser.cpp CommandLine.cpp HelpFormatter.cpp Option.cpp Options.cpp Parser.cpp PosixParser.cpp

#
# The top-level configure script computes libraries
# for all subdirectories. Currently, these are
# the cppunit and dynamic-shared library libraries.
#
# Since these are only required by the test directory,
# override default libs.
#
# In the future, it may be necessary to set the
# variable after filtering the result of configure's
# LIB macro.
#
LIB =
