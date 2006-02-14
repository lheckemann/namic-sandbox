DISTCLEANFILES = config-auto.h

libcliincludedir = $(includedir)/cli
libcliinclude_HEADERS =  \
  BasicParser.h CLI.h CommandLine.h HelpFormatter.h Option.h \
  Options.h ParseException.h Parser.h PosixParser.h Types.h

dist-hook:
  rm -f $(distdir)/config-auto.h
