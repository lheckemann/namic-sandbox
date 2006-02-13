// -*- C++ -*-

 //  NOTE: One leading blank has been added to each line of this file
 //        to cope with the demands of the Wiki.  This blank should
 //        be deleted before trying to compile.

 /*
 ## File: TclChannelStreambuf.h
 ##
 ## Synopsis:
 ##      Class to support redirection of a C++ stream to a Tcl channel
 ##      (particularly, routing cin/cout/cerr to the console).
 #@CDef
 */

 #ifndef __TclChannelStreambuf_h
 #define __TclChannelStreambuf_h 1

 extern "C" {
 #include <tcl.h>
 }

 #include <iostream.h>
 #ifdef _MSVC_VER
 #include <streamb.h>
 #else
 #include <streambuf.h>
 #endif

 class TclChannelStreambuf : public streambuf {
 private:
     Tcl_Channel channel_;
     Tcl_DString iBuffer_;
     int iOffset_;
 public:
     TclChannelStreambuf (Tcl_Channel);
     ~TclChannelStreambuf ();
     virtual int overflow (int);
     virtual int underflow ();
     virtual int sync ();
 };

 extern "C" void TclConsoleStreambufSetup _ANSI_ARGS_(( void ));

 #endif /* __TclChannelStreambuf_h */

