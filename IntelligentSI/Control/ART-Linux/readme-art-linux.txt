;;; to-emacs: -*- outline -*-



* Intro 
  Brief information about the current state of the ARTLinux version.

* Environment


** ARTLinux

  Used VineLinux 3.2 as a starting point.

  Added ARTLinux to kernel 2.4.36 following the prof. Takesue's
  helpful instructions at

  http://www.toyota.nitech.ac.jp/~takesue/rt_ctrl/art00.html

  
  $ uname -a
  Linux localhost.localdomain 2.4.36-ART #3 Tue Feb 5 17:37:13 JST 2008 i686 unknown

** I/O board

  Tested with I/O board - HRP Interface Board 07-0003-1
  http://www.zuko.jp

  The kernel module and the library are installed in 
  /usr/local/src/hrpifb/I7sDriver/hrpib7.o
  /usr/local/src/hrpifb/lib/I7sIo.o

** CMake

  Using CMake to build.  The current CMakeLists.txt file works only on
  Linux for now.

  To build:
  - edit the CMakeLists.txt (e.g., the path to I7sIo.o, etc.)
  - $ cd build
  - $ ccmake ..
  - $ make


* Source comments

  - The watchdog used in the VxWorks version is not implemented yet in
    the Linux version.
  - Need to be run as root to set the thread scheduling, otherwise
    runs with the default settings.
  - Many //TODO: comments.
  - The bugs are mine.


* Miscellaneous

  Any questions, comments, and critiques are welcome.
  Vesko, vesko@vier.mech.nitech.ac.jp
