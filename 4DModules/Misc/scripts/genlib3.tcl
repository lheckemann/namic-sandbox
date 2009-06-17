#!/bin/sh
# the next line restarts using tclsh \
    exec tclsh "$0" "$@"

################################################################################
#
# genlib.tcl
#
# generate the Lib directory with the needed components for slicer
# to build
#
# Steps:
# - pull code from anonymous cvs
# - configure (or cmake) with needed options
# - build for this platform
#
# Packages: cmake, tcl, itcl, iwidgets, blt, ITK, VTK, teem
# 
# Usage:
#   genlib [options] [target]
#
# run genlib from the Slicer3 directory next to where you want the packages to be built
# E.g. if you run /home/pieper/Slicer3/Scripts/genlib.tcl it will create
# /home/pieper/Slicer3-lib
#
# - sp - 2004-06-20
#


if {[info exists ::env(CVS)]} {
    set ::CVS "{$::env(CVS)}"
} else {
    set ::CVS cvs
}

# for subversion repositories (Sandbox)
if {[info exists ::env(SVN)]} {
    set ::SVN $::env(SVN)
} else {
    set ::SVN svn
}

################################################################################
#
# check to see if need to build a package
# returns 1 if need to build, 0 else
# if { [BuildThis  ""] == 1 } {
proc BuildThis { testFile packageName } {
    if {![file exists $testFile] || $::GENLIB(update) || [lsearch $::GENLIB(buildList) $packageName] != -1} {
        # puts "Building $packageName (testFile = $testFile, update = $::GENLIB(update), buildlist = $::GENLIB(buildList) )"
        return 1
    } else {
        # puts "Skipping $packageName"
        return 0
    }
}
#

################################################################################
#
# simple command line argument parsing
#

proc Usage { {msg ""} } {
    global SLICER
    
    set msg "$msg\nusage: genlib \[options\] \[target\]"
    set msg "$msg\n  \[target\] is the the Slicer3_LIB directory"
    set msg "$msg\n             and is determined automatically if not specified"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   --help : prints this message and exits"
    set msg "$msg\n   --clean : delete the target first"
    set msg "$msg\n   --update : do a cvs update even if there's an existing build"
    set msg "$msg\n   --release : compile with optimization flags"
    set msg "$msg\n   --relwithdebinfo : compile with optimization flags and debugging symbols"
    set msg "$msg\n   --nobuild : only download and/or update, but don't build"
    set msg "$msg\n   --test-type : type of ctest to run (for enabled packages)"
    set msg "$msg\n   optional space separated list of packages to build (lower case)"
    set msg "$msg\n   -32 : does a 32 bit build of Slicer and all the libs (Default: isainfo -b on Solaris, 32 bit on other OS'es)"
    set msg "$msg\n   -64 : does a 64 bit build of Slicer and all the libs"
    set msg "$msg\n   --suncc : builds Slicer with Sun's compilers (The default is gcc)"
    set msg "$msg\n   --gcc : builds Slicer with GNU compilers"
    puts stderr $msg
}

set GENLIB(clean) "false"
set GENLIB(update) "false"
set GENLIB(buildit) "true"
set GENLIB(test-type) ""
set ::GENLIB(buildList) ""
if {$tcl_platform(os) == "SunOS"} {
  set isainfo [exec isainfo -b]
  set ::GENLIB(bitness) "-$isainfo"
} else {
  set GENLIB(bitness) "32"
}
set GENLIB(compiler) "gcc"

set isRelease 0
set isRelWithDebInfo 0
set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--clean" -
        "-f" {
            set GENLIB(clean) "true"
        }
        "--update" -
        "-u" {
            set GENLIB(update) "true"
        }
        "--release" {
            set isRelease 1
            set ::VTK_BUILD_TYPE "Release"
        }
        "--relwithdebinfo" {
            set isRelWithDebInfo 1
            set ::VTK_BUILD_TYPE "ReleaseWithDebInfo"
        }
        "--nobuild" {
            set ::GENLIB(buildit) "false"
        }
        "-t" -
        "--test-type" {
            incr i
            if { $i == $argc } {
                Usage "Missing test-type argument"
            } else {
                set ::GENLIB(test-type) [lindex $argv $i]
            }
        }
        "-64" {
            set ::GENLIB(bitness) "64"
        }
        "-32" {
            set ::GENLIB(bitness) "32"
        }
        "--suncc" {
            set ::GENLIB(compiler) "suncc"
        }
        "--gcc" {
            set ::GENLIB(compiler) "gcc"
        }
        "--help" -
        "-h" {
            Usage
            exit 1
        }
        "-*" {
            Usage "unknown option $a\n"
            exit 1
        }
        default {
            lappend strippedargs $a 
        }
    }
}

set argv $strippedargs
set argc [llength $argv]

set ::Slicer3_LIB ""
if {$argc > 1 } {
    # the stripped args list now has the Slicer3_LIB first and then the list of packages to build
    set ::GENLIB(buildList) [lrange $strippedargs 1 end]
    set strippedargs [lindex $strippedargs 0]
# puts "Got the list of package to build: '$::GENLIB(buildList)' , stripped args = $strippedargs"
} 
set ::Slicer3_LIB $strippedargs


################################################################################
#
# Utilities:

proc runcmd {args} {
    global isWindows
    puts "running: $args"

    # print the results line by line to provide feedback during long builds
    # interleaves the results of stdout and stderr, except on Windows
    if { $isWindows } {
        # Windows does not provide native support for cat
        set fp [open "| $args" "r"]
    } else {
        set fp [open "| $args |& cat" "r"]
    }
    while { ![eof $fp] } {
        gets $fp line
        puts $line
    }
    set ret [catch "close $fp" res] 
    if { $ret } {
        puts stderr $res
        if { $isWindows } {
            # Does not work on Windows
        } else {
            error $ret
        }
    } 
}

# helper proc to edit a file in place (cross platform)
proc replaceStringInFile {fileName oldString newString} {
  set fp [open $fileName "r"]
  set contents [read $fp]
  close $fp
  regsub -all $oldString $contents $newString contents
  set fp [open $fileName "w"]
  puts $fp $contents
  close $fp
}


################################################################################
# First, set up the directory
# - determine the location
# - determine the build
# 

# hack to work around lack of normalize option in older tcl
# set Slicer3_HOME [file dirname [file dirname [file normalize [info script]]]]
set cwd [pwd]
cd [file dirname [info script]]
cd ..
set Slicer3_HOME [pwd]
cd $cwd

if { $::Slicer3_LIB == "" } {
  set ::Slicer3_LIB [file dirname $::Slicer3_HOME]/Slicer3-lib
  puts "Slicer3_LIB is $::Slicer3_LIB"
}

#######
#
# Note: the local vars file, slicer2/slicer_variables.tcl, overrides the default values in this script
# - use it to set your local environment and then your change won't 
#   be overwritten when this file is updated
#
set localvarsfile $Slicer3_HOME/slicer_variables3.tcl
catch {set localvarsfile [file normalize $localvarsfile]}
if { [file exists $localvarsfile] } {
    puts "Sourcing $localvarsfile"
    source $localvarsfile
} else {
    puts "stderr: $localvarsfile not found - use this file to set up your build"
    exit 1
}

#initialize platform variables
switch $tcl_platform(os) {
    "SunOS" {
        set isSolaris 1
        set isWindows 0
        set isDarwin 0
        set isLinux 0
    }
    "Linux" { 
        set isSolaris 0
        set isWindows 0
        set isDarwin 0
        set isLinux 1
    }
    "Darwin" { 
        set isSolaris 0
        set isWindows 0
        set isDarwin 1
        set isLinux 0
    }
    default { 
        set isSolaris 0
        set isWindows 1
        set isDarwin 0
        set isLinux 0
    }
}

set ::VTK_DEBUG_LEAKS "ON"
if ($isRelease) {
    set ::VTK_BUILD_TYPE "Release"
    set ::env(VTK_BUILD_TYPE) $::VTK_BUILD_TYPE
    if ($isWindows) {
        set ::VTK_BUILD_SUBDIR "Release"
    } else {
        set ::VTK_BUILD_SUBDIR ""
    }
    puts "Overriding slicer_variables.tcl; VTK_BUILD_TYPE is '$::env(VTK_BUILD_TYPE)', VTK_BUILD_SUBDIR is '$::VTK_BUILD_SUBDIR'"
    set ::VTK_DEBUG_LEAKS "OFF"
}

if ($isRelWithDebInfo) {
    set ::VTK_BUILD_TYPE "RelWithDebInfo"
    set ::env(VTK_BUILD_TYPE) $::VTK_BUILD_TYPE
    if ($isWindows) {
        set ::VTK_BUILD_SUBDIR "RelWithDebInfo"
    } else {
        set ::VTK_BUILD_SUBDIR ""
    }
    puts "Overriding slicer_variables.tcl; VTK_BUILD_TYPE is '$::env(VTK_BUILD_TYPE)', VTK_BUILD_SUBDIR is '$::VTK_BUILD_SUBDIR'"
    set ::VTK_DEBUG_LEAKS "OFF"
}

# tcl file delete is broken on Darwin, so use rm -rf instead
if { $::GENLIB(clean) } {
    puts "Deleting slicer lib files..."
    if { $isDarwin } {
        runcmd rm -rf $Slicer3_LIB
        if { [file exists $Slicer3_LIB/tcl/isPatched] } {
            runcmd rm $Slicer3_LIB/tcl/isPatched
        }

    } else {
        file delete -force $Slicer3_LIB
    }
}

if { ![file exists $Slicer3_LIB] } {
    file mkdir $Slicer3_LIB
}


################################################################################
# If is Darwin, don't use cvs compression to get around bug in cvs 1.12.13
#

if {$isDarwin} {
    set CVS_CO_FLAGS "-q"  
} else {
    set CVS_CO_FLAGS "-q -z3"    
}


################################################################################
# Get and build CMake
#

# set in slicer_vars
if { [BuildThis $::CMAKE "cmake"] == 1 } {
    file mkdir $::CMAKE_PATH
    cd $Slicer3_LIB


    if {$isWindows} {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/Binaries/Windows/CMake-build CMake-build
    } else {
        runcmd $::CVS -d :pserver:anonymous:cmake@www.cmake.org:/cvsroot/CMake login
        eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anonymous@www.cmake.org:/cvsroot/CMake checkout -r $::CMAKE_TAG CMake"

        if {$::GENLIB(buildit)} {
          cd $::CMAKE_PATH
          runcmd $Slicer3_LIB/CMake/bootstrap
          eval runcmd $::MAKE
       }
    }
}


################################################################################
# Get and build tcl, tk, itcl, widgets
#
#

# on windows, tcl won't build right, as can't configure, so save commands have to run
if { [BuildThis $::TCL_TEST_FILE "tcl"] == 1 } {

    if {$isWindows} {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/Binaries/Windows/$::TCL_VERSION-build tcl-build
    }

    file mkdir $Slicer3_LIB/tcl
    cd $Slicer3_LIB/tcl
    if { $::TCL_VERSION == "tcl85" } {
      runcmd $::CVS -d:pserver:anonymous:@tcl.cvs.sourceforge.net:/cvsroot/tcl login 
      runcmd $::CVS -z3 -d:pserver:anonymous@tcl.cvs.sourceforge.net:/cvsroot/tcl co -r core-8-5-6 tcl
    } else {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/$::TCL_VERSION/tcl tcl
    }

    if {$::GENLIB(buildit)} {
      if {$isWindows} {
          # can't do windows
      } else {
        cd $Slicer3_LIB/tcl/tcl/unix

        if {$::GENLIB(bitness) == "64"} {
          runcmd ./configure --enable-64bit --prefix=$Slicer3_LIB/tcl-build
        } else {
          runcmd ./configure --prefix=$Slicer3_LIB/tcl-build
        }

        eval runcmd $::MAKE
        eval runcmd $::MAKE install
      }
    }
}

if { [BuildThis $::TK_TEST_FILE "tk"] == 1 } {
    cd $Slicer3_LIB/tcl

    if { $::TCL_VERSION == "tcl85" } {
      runcmd $::CVS -d:pserver:anonymous:@tktoolkit.cvs.sourceforge.net:/cvsroot/tktoolkit login 
      runcmd $::CVS -z3 -d:pserver:anonymous@tktoolkit.cvs.sourceforge.net:/cvsroot/tktoolkit co -r core-8-5-6 tk
    } else {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/$::TCL_VERSION/tk tk
    }

    if {$::GENLIB(buildit)} {
      if {$isWindows} {
         # ignore, already downloaded with tcl
      } else {
        cd $Slicer3_LIB/tcl/tk/unix
        if { $isDarwin } {
          runcmd ./configure --with-tcl=$Slicer3_LIB/tcl-build/lib --prefix=$Slicer3_LIB/tcl-build --disable-corefoundation --x-libraries=/usr/X11R6/lib --x-includes=/usr/X11R6/include --with-x
        } else {
          if {$::GENLIB(bitness) =="64"} {
            runcmd ./configure --with-tcl=$Slicer3_LIB/tcl-build/lib --enable-64bit --prefix=$Slicer3_LIB/tcl-build
          } else {
            runcmd ./configure --with-tcl=$Slicer3_LIB/tcl-build/lib --prefix=$Slicer3_LIB/tcl-build
          }
        } 
        eval runcmd $::MAKE
        eval runcmd $::MAKE install
         
        file copy -force $Slicer3_LIB/tcl/tk/generic/default.h $Slicer3_LIB/tcl-build/include
        file copy -force $Slicer3_LIB/tcl/tk/unix/tkUnixDefault.h $Slicer3_LIB/tcl-build/include
      }
   }
}

if { [BuildThis $::ITCL_TEST_FILE "itcl"] == 1 } {

    cd $Slicer3_LIB/tcl

    if { $::TCL_VERSION == "tcl85" } {
      runcmd $::CVS -d:pserver:anonymous:@incrtcl.cvs.sourceforge.net:/cvsroot/incrtcl login
      runcmd $::CVS -z3 -d:pserver:anonymous@incrtcl.cvs.sourceforge.net:/cvsroot/incrtcl co -r HEAD incrTcl
    } else {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/$::TCL_VERSION/incrTcl incrTcl
    }

    cd $Slicer3_LIB/tcl/incrTcl

    exec chmod +x ../incrTcl/configure 
    if {$::GENLIB(buildit)} {
      if {$isWindows} {
         # ignore, already downloaded with tcl
      } else {
        if { $isDarwin } {
          exec cp ../incrTcl/itcl/configure ../incrTcl/itcl/configure.orig
          exec sed -e "s/\\*\\.c | \\*\\.o | \\*\\.obj) ;;/\\*\\.c | \\*\\.o | \\*\\.obj | \\*\\.dSYM | \\*\\.gnoc ) ;;/" ../incrTcl/itcl/configure.orig > ../incrTcl/itcl/configure 
      }
      if {$::GENLIB(bitness) == "64"} {
        set ::env(CC) "$::GENLIB(compiler) -m64"
        puts "genlib incrTcl 64 bit branch: $::env(CC)"
        runcmd ../incrTcl/configure --with-tcl=$Slicer3_LIB/tcl-build/lib --with-tk=$Slicer3_LIB/tcl-build/lib --prefix=$Slicer3_LIB/tcl-build
      } else {
        set ::env(CC) "$::GENLIB(compiler)"
        puts "genlib incrTcl 32 bit branch: $::env(CC)"
        runcmd ../incrTcl/configure --with-tcl=$Slicer3_LIB/tcl-build/lib --with-tk=$Slicer3_LIB/tcl-build/lib --prefix=$Slicer3_LIB/tcl-build
      }

      if { $isDarwin } {
        # need to run ranlib separately on lib for Darwin
        # file is created and ranlib is needed inside make all
        catch "eval runcmd $::MAKE all"
        if { [file exists ../incrTcl/itcl/libitclstub3.2.a] } {
          runcmd ranlib ../incrTcl/itcl/libitclstub3.2.a
        }
      }

      eval runcmd $::MAKE all
      eval runcmd $::SERIAL_MAKE install
    }
  }
}

################################################################################
# Get and build iwidgets
#

if { [BuildThis $::IWIDGETS_TEST_FILE "iwidgets"] == 1 } {
    cd $Slicer3_LIB/tcl

    if { $::TCL_VERSION == "tcl85" } {
      runcmd $::CVS -d:pserver:anonymous:@incrtcl.cvs.sourceforge.net:/cvsroot/incrtcl login
      runcmd $::CVS -z3 -d:pserver:anonymous@incrtcl.cvs.sourceforge.net:/cvsroot/incrtcl co -r HEAD iwidgets
    } else {
      runcmd  $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/$::TCL_VERSION/iwidgets iwidgets
    }

    if {$::GENLIB(buildit)} {
        if {$isWindows} {
            # is present in the windows binary download
        } else {
            cd $Slicer3_LIB/tcl/iwidgets
            runcmd ../iwidgets/configure --with-tcl=$Slicer3_LIB/tcl-build/lib --with-tk=$Slicer3_LIB/tcl-build/lib --with-itcl=$Slicer3_LIB/tcl/incrTcl --prefix=$Slicer3_LIB/tcl-build
            # make all doesn't do anything...
            # iwidgets won't compile in parallel (with -j flag)
            eval runcmd $::SERIAL_MAKE all
            eval runcmd $::SERIAL_MAKE install
        }
    }
}

################################################################################
# Get and build blt
#

if { [BuildThis $::BLT_TEST_FILE "blt"] == 1 } {
    cd $Slicer3_LIB/tcl


    if { $::TCL_VERSION == "tcl85" } {
      runcmd $::CVS -d:pserver:anonymous:@blt.cvs.sourceforge.net:/cvsroot/blt login
      runcmd $::CVS -z3 -d:pserver:anonymous@blt.cvs.sourceforge.net:/cvsroot/blt co -r HEAD blt
    } else {
      runcmd  $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/$::TCL_VERSION/blt blt
      runcmd  $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/tcl/blt blt
    }

    if {$::GENLIB(buildit)} {
        if { $isWindows } { 
            # is present in the windows binary download
        } elseif { $isDarwin } {
            if { ![file exists $Slicer3_LIB/tcl/isPatchedBLT] } { 
              puts "Patching..." 
              runcmd curl -k -O https://share.spl.harvard.edu/share/birn/public/software/External/Patches/bltpatch 
              cd $Slicer3_LIB/tcl/blt 
              runcmd patch -p2 < ../bltpatch 

              # create a file to make sure BLT isn't patched twice 
              runcmd touch $Slicer3_LIB/tcl/isPatchedBLT 
              file delete $Slicer3_LIB/tcl/bltpatch 
            } else { 
              puts "BLT already patched." 
            }
            cd $Slicer3_LIB/tcl/blt
            runcmd ./configure --with-tcl=$Slicer3_LIB/tcl/tcl/unix --with-tk=$Slicer3_LIB/tcl-build --prefix=$Slicer3_LIB/tcl-build --enable-shared --x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib --with-cflags=-fno-common
            eval runcmd $::MAKE
            eval runcmd $::MAKE install
        } elseif { $isSolaris } {
            cd $Slicer3_LIB/tcl/blt
            runcmd ./configure --with-tcl=$Slicer3_LIB/tcl/tcl/unix --with-tk=$Slicer3_LIB/tcl-build --prefix=$Slicer3_LIB/tcl-build --enable-shared
            eval runcmd $::SERIAL_MAKE
            eval runcmd $::SERIAL_MAKE install
        } else {
            cd $Slicer3_LIB/tcl/blt
            runcmd ./configure --with-tcl=$Slicer3_LIB/tcl/tcl/unix --with-tk=$Slicer3_LIB/tcl-build --prefix=$Slicer3_LIB/tcl-build
            eval runcmd $::SERIAL_MAKE
            eval runcmd $::SERIAL_MAKE install
        }
    }
}

################################################################################
# Get and build python
#

if {  [BuildThis $::PYTHON_TEST_FILE "python"] && !$::USE_SYSTEM_PYTHON && [string tolower $::USE_PYTHON] == "on" } {
    if { $isWindows } {

      file mkdir $::Slicer3_LIB/python
      cd $::Slicer3_LIB

      runcmd $::SVN co $::PYTHON_TAG python-build
      cd $Slicer3_LIB/python-build

      # patch the socket module to accomodate newer visual studios
      if { $::GENERATOR != "Visual Studio 7 .NET 2003" } {
        file copy -force $::Slicer3_HOME/Base/GUI/Python/patched-socketmodule.c Modules/socketmodule.c
      }

      # point the tkinter build file to the slicer tcl-build 
      replaceStringInFile "PCbuild/_tkinter.vcproj" "tcltk" "tcl-build"

      if { $::GENERATOR != "Visual Studio 7 .NET 2003" } {
         if {$::MAKE != "c:/Program Files/Microsoft Visual Studio 9.0/Common7/IDE/VCExpress.exe"} {
           runcmd $::MAKE PCbuild/pcbuild.sln /Upgrade
         } else {
           runcmd $::MAKE PCbuild/pcbuild.sln
         }
      }
      runcmd $::MAKE PCbuild/pcbuild.sln /out buildlog.txt /build Release

      # fix distutils to ignore it's hardcoded python version info
      replaceStringInFile Lib/distutils/msvccompiler.py "raise DistutilsPlatformError," "print"

      
      # copy the lib so that numpy and slicer can find it easily
      # copy the socket shared library so python can find it
      # TODO: perhaps we need an installer step here
      set ret [catch "file copy -force $::Slicer3_LIB/python-build/PCbuild/python25.lib $::Slicer3_LIB/python-build/Lib/python25.lib "]
      if {$ret == 1} {
          puts "ERROR: couldn't copy $::Slicer3_LIB/python-build/PCbuild/python25.lib to $::Slicer3_LIB/python-build/Lib/"
          exit 1
      }
      set ret [catch "file copy -force $::Slicer3_LIB/python-build/PCbuild/_socket.pyd $::Slicer3_LIB/python-build/Lib/_socket.pyd"]
      if {$ret == 1} {
         puts "ERROR: failed to copy $::Slicer3_LIB/python-build/PCbuild/_socket.pyd $::Slicer3_LIB/python-build/Lib/_socket.pyd"
         exit 1
       }
      set ret [catch "file copy -force $::Slicer3_LIB/python-build/PCbuild/_ctypes.pyd to $::Slicer3_LIB/python-build/Lib/_ctypes.pyd"]
      if {$ret == 1} {
        puts "ERORR: failed to copy $::Slicer3_LIB/python-build/PCbuild/_ctypes.pyd to $::Slicer3_LIB/python-build/Lib/_ctypes.pyd"
        exit 1
      }

    } else {

      file mkdir $::Slicer3_LIB/python
      file mkdir $::Slicer3_LIB/python-build
      cd $::Slicer3_LIB

      cd $Slicer3_LIB/python
      runcmd $::SVN co $::PYTHON_TAG
      cd $Slicer3_LIB/python/release25-maint

      foreach flag {LD_LIBRARY_PATH LDFLAGS CPPFLAGS} {
        if { ![info exists ::env($flag)] } { set ::env($flag) "" }
      }
      set ::env(LDFLAGS) "$::env(LDFLAGS) -L$Slicer3_LIB/tcl-build/lib"
      set ::env(CPPFLAGS) "$::env(CPPFLAGS) -I$Slicer3_LIB/tcl-build/include"
      set ::env(LD_LIBRARY_PATH) $Slicer3_LIB/tcl-build/lib:$Slicer3_LIB/python-build/lib:$::env(LD_LIBRARY_PATH)

      runcmd ./configure --prefix=$Slicer3_LIB/python-build --with-tcl=$Slicer3_LIB/tcl-build --enable-shared
      eval runcmd $::MAKE
      eval runcmd $::SERIAL_MAKE install
        
      if { $isDarwin } {
            # Special Slicer hack to build and install the .dylib
            file mkdir $::Slicer3_LIB/python-build/lib/
            file delete -force $::Slicer3_LIB/python-build/lib/libpython2.5.dylib
            set fid [open environhack.c w]
            puts $fid "char **environ=0;"
            close $fid
            runcmd gcc -c -o environhack.o environhack.c
            runcmd libtool -o $::Slicer3_LIB/python-build/lib/libpython2.5.dylib -dynamic  \
                -all_load libpython2.5.a environhack.o -single_module \
                -install_name $::Slicer3_LIB/python-build/lib/libpython2.5.dylib \
                -compatibility_version 2.5 \
                -current_version 2.5 -lSystem -lSystemStubs

        }
    }
}

################################################################################
# Get and build netlib (blas and lapack)
#

if { [BuildThis $::NETLIB_TEST_FILE "netlib"] && !$::USE_SYSTEM_PYTHON && $::USE_SCIPY } {

    # This part follows the instraction available at http://www.scipy.org/Installing_SciPy/Linux

    file mkdir $::Slicer3_LIB/netlib
    file mkdir $::Slicer3_LIB/netlib-build

    if { $isWindows } {
        # windows binary already checked out
    } else {

        # generate paltform tag  
        if { $isDarwin } {
          set platform DARWIN
        } elseif { $isLinux && $::GENLIB(bitness) == "64" } {
          set platform LINUX64
        } else {
          set platform LINUX
        }
       
        # Build lapack

        cd $::Slicer3_LIB/netlib
        runcmd $::SVN co $::LAPACK_TAG lapack
        runcmd $::SVN co $::NETLIB_INC_TAG netlib_make_inc

        file copy -force $::Slicer3_LIB/netlib/netlib_make_inc/lapack_make.inc.$::FORTRAN_COMPILER.$platform $::Slicer3_LIB/netlib/lapack/make.inc
        cd lapack/SRC
        runcmd make

        # Build ATLAS
        cd $::Slicer3_LIB/netlib        
        runcmd $::SVN co $::ATLAS_TAG ATLAS
        file mkdir $::Slicer3_LIB/netlib-build/ATLAS-build
        cd $::Slicer3_LIB/netlib-build/ATLAS-build

        # NOTE: For ATLAS, we force the configure to continue regardless
        # of the results of the CPU throttling probe (see ATLAS/INSTALL.txt
        # for detail)
        if {$::GENLIB(bitness) == "64"} {
          runcmd $::Slicer3_LIB/netlib/ATLAS/configure -b 64 -Fa alg -fPIC --with-netlib-lapack=$::Slicer3_LIB/netlib/lapack/lapack_$platform.a -Si cputhrchk 0
        } else {
          runcmd $::Slicer3_LIB/netlib/ATLAS/configure -Fa alg -fPIC --with-netlib-lapack=$::Slicer3_LIB/netlib/lapack/lapack_$platform.a -Si cputhrchk 0
        }
        runcmd make
    }
}

################################################################################
# Get and build numpy and scipy
#

if {  [BuildThis $::NUMPY_TEST_FILE "python"] && !$::USE_SYSTEM_PYTHON && [string tolower $::USE_PYTHON] == "on" } {

    set ::env(PYTHONHOME) $::Slicer3_LIB/python-build
    cd $::Slicer3_LIB/python

    # do numpy

    runcmd $::SVN co $::NUMPY_TAG numpy

    if { $isLinux } {
        # Add MAIN__ stub symbol to work around link error with gnu fortran
        # (protect the definition in case the patch is applied more than once)
        set fp [open numpy/numpy/linalg/python_xerbla.c "a"]
        puts $fp "#ifndef MAIN__DEFINED"
        puts $fp "int MAIN__() {return (0);}"
        puts $fp "#endif"
        puts $fp "#define MAIN__DEFINED"
        close $fp
    }

    if { $isWindows } {

        # prepare the environment for numpy build script
        regsub -all ":" [file dirname $::MAKE] "" devenvdir
        regsub -all ":" $::COMPILER_PATH "" vcbindir
        set ::env(PATH) /cygdrive/$devenvdir:/cygdrive/$vcbindir:$::env(PATH)
        set ::env(PATH) $::Slicer3_LIB/python-build/PCbuild:$::env(PATH)
        set ::env(INCLUDE) [file dirname $::COMPILER_PATH]/include
        set ::env(INCLUDE) $::MSSDK_PATH/Include\;$::env(INCLUDE)
        set ::env(LIB) $::MSSDK_PATH/Lib\;[file dirname $::COMPILER_PATH]/lib

        cd $::Slicer3_LIB/python/numpy
        runcmd $::Slicer3_LIB/python-build/PCbuild/python.exe ./setup.py --verbose install

        # numpy dlls do not have embeded manifests by default, need to add them here
        # - only required for newer versions of visual studio.  Assume that
        #   if the mt command doesn't exists, there are no manifests
        set mt $::MSSDK_PATH/Bin/mt.exe
        if { [file exists $mt] } {
          set libprefix $::Slicer3_LIB/python-build/Lib/site-packages/numpy
          foreach lib {core fft lib linalg numarray random} {
            set pyds [glob -nocomplain $libprefix/$lib/*.pyd]
            foreach pyd $pyds {
              runcmd $mt -manifest $pyd.manifest -outputresource:$pyd\;2
            }
          }
        }

    } else {

        if { $isDarwin } {
            if { ![info exists ::env(DYLD_LIBRARY_PATH)] } { set ::env(DYLD_LIBRARY_PATH) "" }
            set ::env(DYLD_LIBRARY_PATH) $::Slicer3_LIB/python-build/lib:$::env(DYLD_LIBRARY_PATH)
        } else {
            if { ![info exists ::env(LD_LIBRARY_PATH)] } { set ::env(LD_LIBRARY_PATH) "" }
            set ::env(LD_LIBRARY_PATH) $::Slicer3_LIB/python-build/lib:$::env(LD_LIBRARY_PATH)
        }

        if {$::GENLIB(bitness) == "64"} {
          set ::env(CC) "$::GENLIB(compiler) -m64"
        } else {
          set ::env(CC) "$::GENLIB(compiler)"
        }

        cd $::Slicer3_LIB/python/numpy

        # generate site.cfg
        set fp [open "site.cfg" "w"]
        puts $fp
        puts $fp "\[DEFAULT\]"
        puts $fp "library_dirs = $::Slicer3_LIB/netlib-build/ATLAS-build/lib"
        puts $fp "include_dirs = $::Slicer3_LIB/netlib-build/ATLAS-build/include"
        puts $fp "\[blas_opt\]"
        puts $fp "libraries = ptf77blas, ptcblas, atlas"
        puts $fp "\[lapack_opt\]"
        puts $fp "libraries = lapack, ptf77blas, ptcblas, atlas"
        close $fp

       # build and install
        runcmd $::Slicer3_LIB/python-build/bin/python ./setup.py install

        if { $::USE_SCIPY } {
          # TODO: need to have a way to build the blas library...
          cd $::Slicer3_LIB/python
          runcmd $::SVN co $::SCIPY_TAG scipy
          
          cd $::Slicer3_LIB/python/scipy

          # generate site.cfg
          set fp [open "site.cfg" "w"]
          puts $fp
          puts $fp "\[DEFAULT\]"
          puts $fp "library_dirs = $::Slicer3_LIB/netlib-build/ATLAS-build/lib"
          puts $fp "include_dirs = $::Slicer3_LIB/netlib-build/ATLAS-build/include"
          puts $fp "\[blas_opt\]"
          puts $fp "libraries = ptf77blas, ptcblas, atlas"
          puts $fp "\[lapack_opt\]"
          puts $fp "libraries = lapack, ptf77blas, ptcblas, atlas"
          close $fp

          runcmd $::Slicer3_LIB/python-build/bin/python ./setup.py install
        }
    }
}


################################################################################
# Get and build vtk
#

if { [BuildThis $::VTK_TEST_FILE "vtk"] == 1 } {
    cd $Slicer3_LIB

    runcmd $::CVS -d :pserver:anonymous:vtk@public.kitware.com:/cvsroot/VTK login
    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK checkout -r $::VTK_TAG VTK"

    # Andy's temporary hack to get around wrong permissions in VTK cvs repository
    # catch statement is to make file attributes work with RH 7.3
    if { !$isWindows } {
        catch "file attributes $Slicer3_LIB/VTK/VTKConfig.cmake.in -permissions a+rw"
    }
    if {$::GENLIB(buildit)} {

      file mkdir $Slicer3_LIB/VTK-build
      cd $Slicer3_LIB/VTK-build

      set USE_VTK_ANSI_STDLIB ""
      if { $isWindows } {
        if {$MSVC6} {
            set USE_VTK_ANSI_STDLIB "-DVTK_USE_ANSI_STDLIB:BOOL=ON"
        }
      }

      #
      # Note - the two banches are identical down to the line starting -DOPENGL...
      # -- the text needs to be duplicated to avoid quoting problems with paths that have spaces
      #
      if { $isLinux && $::tcl_platform(machine) == "x86_64" } {
        runcmd $::CMAKE \
            -G$GENERATOR \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_SKIP_RPATH:BOOL=ON \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_X:BOOL=ON \
            -DVTK_WRAP_TCL:BOOL=ON \
            -DVTK_USE_HYBRID:BOOL=ON \
            -DVTK_USE_PATENTED:BOOL=ON \
            -DVTK_USE_PARALLEL:BOOL=ON \
            -DVTK_DEBUG_LEAKS:BOOL=$::VTK_DEBUG_LEAKS \
            -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
            -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
            -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
            $USE_VTK_ANSI_STDLIB \
            -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
            -DVTK_USE_64BIT_IDS:BOOL=ON \
            ../VTK
      } elseif { $isDarwin } {
        set OpenGLString "-framework OpenGL;/usr/X11R6/lib/libGL.dylib"
        runcmd $::CMAKE \
            -G$GENERATOR \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_SKIP_RPATH:BOOL=OFF \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DCMAKE_SHARED_LINKER_FLAGS:STRING="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib" \
            -DCMAKE_EXE_LINKER_FLAGS="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib" \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_COCOA:BOOL=OFF \
            -DVTK_USE_X:BOOL=ON \
            -DVTK_WRAP_TCL:BOOL=ON \
            -DVTK_USE_HYBRID:BOOL=ON \
            -DVTK_USE_PATENTED:BOOL=ON \
            -DVTK_USE_PARALLEL:BOOL=ON \
            -DVTK_DEBUG_LEAKS:BOOL=$::VTK_DEBUG_LEAKS \
            -DOPENGL_INCLUDE_DIR:PATH=/usr/X11R6/include \
            -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
            -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
            -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
            -DOPENGL_gl_LIBRARY:STRING=$OpenGLString \
            $USE_VTK_ANSI_STDLIB \
            ../VTK
      } elseif { $isSolaris && $::GENLIB(bitness) == "64" } {
        runcmd $::CMAKE \
            -G$GENERATOR \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_SKIP_RPATH:BOOL=ON \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_X:BOOL=ON \
            -DVTK_WRAP_TCL:BOOL=ON \
            -DVTK_USE_HYBRID:BOOL=ON \
            -DVTK_USE_PATENTED:BOOL=ON \
            -DVTK_USE_PARALLEL:BOOL=ON \
            -DVTK_DEBUG_LEAKS:BOOL=$::VTK_DEBUG_LEAKS \
            -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
            -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
            -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
            $USE_VTK_ANSI_STDLIB \
            -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
            -DVTK_USE_64BIT_IDS:BOOL=ON \
            ../VTK
      } else {
        runcmd $::CMAKE \
            -G$GENERATOR \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_SKIP_RPATH:BOOL=ON \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_X:BOOL=ON \
            -DVTK_WRAP_TCL:BOOL=ON \
            -DVTK_USE_HYBRID:BOOL=ON \
            -DVTK_USE_PATENTED:BOOL=ON \
            -DVTK_USE_PARALLEL:BOOL=ON \
            -DVTK_DEBUG_LEAKS:BOOL=$::VTK_DEBUG_LEAKS \
            -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
            -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
            -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
            -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
            $USE_VTK_ANSI_STDLIB \
            ../VTK
      }

      if { $isWindows } {
        if { $MSVC6 } {
            runcmd $::MAKE VTK.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE VTK.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE 
    }
  }
}

################################################################################
# Get and build kwwidgets
#

if { [BuildThis $::KWWidgets_TEST_FILE "kwwidgets"] == 1 } {
    cd $Slicer3_LIB

    runcmd $::CVS -d :pserver:anoncvs:@www.kwwidgets.org:/cvsroot/KWWidgets login
    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.kwwidgets.org:/cvsroot/KWWidgets checkout -r $::KWWidgets_TAG KWWidgets"

    if {$::GENLIB(buildit)} {
      file mkdir $Slicer3_LIB/KWWidgets-build
      cd $Slicer3_LIB/KWWidgets-build



      runcmd $::CMAKE \
        -G$GENERATOR \
        -DVTK_DIR:PATH=$Slicer3_LIB/VTK-build \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DCMAKE_SKIP_RPATH:BOOL=ON \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DKWWidgets_BUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DKWWidgets_BUILD_TESTING:BOOL=OFF \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        ../KWWidgets

      if { $isDarwin } {
        runcmd $::CMAKE \
          -DCMAKE_SHARED_LINKER_FLAGS:STRING="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib" \
          -DCMAKE_EXE_LINKER_FLAGS="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib" \
          ../KWWidgets
      }

      if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE KWWidgets.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE KWWidgets.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE 
      }
  }
}

################################################################################
# Get and build itk
#

if { [BuildThis $::ITK_TEST_FILE "itk"] == 1 } {
    cd $Slicer3_LIB

    runcmd $::CVS -d :pserver:anoncvs:@www.vtk.org:/cvsroot/Insight login
    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.vtk.org:/cvsroot/Insight checkout -r $::ITK_TAG Insight"

    if {$::GENLIB(buildit)} {
      file mkdir $Slicer3_LIB/Insight-build
      cd $Slicer3_LIB/Insight-build


      if {$isDarwin} {
        runcmd $::CMAKE \
          -G$GENERATOR \
          -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
          -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
          -DITK_USE_REVIEW:BOOL=ON \
          -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON \
          -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON \
          -DBUILD_SHARED_LIBS:BOOL=ON \
          -DCMAKE_SKIP_RPATH:BOOL=OFF \
          -DBUILD_EXAMPLES:BOOL=OFF \
          -DBUILD_TESTING:BOOL=OFF \
          -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
          -DITK_LEGACY_REMOVE:BOOL=ON \
        ../Insight
      } else {
        runcmd $::CMAKE \
          -G$GENERATOR \
          -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
          -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
          -DITK_USE_REVIEW:BOOL=ON \
          -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON \
          -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON \
          -DBUILD_SHARED_LIBS:BOOL=ON \
          -DCMAKE_SKIP_RPATH:BOOL=ON \
          -DBUILD_EXAMPLES:BOOL=OFF \
          -DBUILD_TESTING:BOOL=OFF \
          -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
          -DITK_LEGACY_REMOVE:BOOL=ON \
          ../Insight
      }

      if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE ITK.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE ITK.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE 
    }
    puts "Patching ITK..."

    set fp1 [open "$Slicer3_LIB/Insight-build/Utilities/nifti/niftilib/cmake_install.cmake" r]
    set fp2 [open "$Slicer3_LIB/Insight-build/Utilities/nifti/znzlib/cmake_install.cmake" r]
    set data1 [read $fp1]
    set data2 [read $fp2]

    close $fp1
    close $fp2

    regsub -all /usr/local/lib $data1 \${CMAKE_INSTALL_PREFIX}/lib data1
    regsub -all /usr/local/include $data1 \${CMAKE_INSTALL_PREFIX}/include data1
    regsub -all /usr/local/lib $data2 \${CMAKE_INSTALL_PREFIX}/lib data2
    regsub -all /usr/local/include $data2 \${CMAKE_INSTALL_PREFIX}/include data2

    set fw1 [open "$Slicer3_LIB/Insight-build/Utilities/nifti/niftilib/cmake_install.cmake" w]
    set fw2 [open "$Slicer3_LIB/Insight-build/Utilities/nifti/znzlib/cmake_install.cmake" w]

    puts -nonewline $fw1 $data1
    puts -nonewline $fw2 $data2
 
    close $fw1
    close $fw2
  }
}


################################################################################
# Get and build teem
# -- relies on VTK's png and zlib
#

if { [BuildThis $::Teem_TEST_FILE "teem"] == 1 } {
    cd $Slicer3_LIB

    runcmd $::SVN co $::Teem_TAG teem


    if {$::GENLIB(buildit)} {
      file mkdir $Slicer3_LIB/teem-build
      cd $Slicer3_LIB/teem-build

      if { $isDarwin } {
        set C_FLAGS -DCMAKE_C_FLAGS:STRING=-fno-common \
      } else {
        set C_FLAGS ""
      }
# !!! FIXME How to append -m64 the -fno-common if we want to build 64 bit on Mac?

      if {$::GENLIB(bitness) == "64"} {
        set C_FLAGS -DCMAKE_C_FLAGS:STRING=-m64 \
      } else {
        set C_FLAGS ""
      }

      switch $::tcl_platform(os) {
        "SunOS" -
        "Linux" {
            set zlib "libvtkzlib.so"
            set png "libvtkpng.so"
        }
        "Darwin" {
            set zlib "libvtkzlib.dylib"
            set png "libvtkpng.dylib"
        }
        "Windows NT" {
            set zlib "vtkzlib.lib"
            set png "vtkpng.lib"
        }
      }

      runcmd $::CMAKE \
        -G$GENERATOR \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        $C_FLAGS \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_TESTING:BOOL=ON \
        -DTeem_PTHREAD:BOOL=OFF \
        -DTeem_BZIP2:BOOL=OFF \
        -DTeem_ZLIB:BOOL=ON \
        -DTeem_PNG:BOOL=ON \
        -DTeem_VTK_MANGLE:BOOL=ON \
        -DTeem_VTK_TOOLKITS_IPATH:FILEPATH=$::Slicer3_LIB/VTK-build \
        -DZLIB_INCLUDE_DIR:PATH=$::Slicer3_LIB/VTK/Utilities \
        -DTeem_VTK_ZLIB_MANGLE_IPATH:PATH=$::Slicer3_LIB/VTK/Utilities/vtkzlib \
        -DTeem_ZLIB_DLLCONF_IPATH:PATH=$::Slicer3_LIB/VTK-build/Utilities \
        -DZLIB_LIBRARY:FILEPATH=$::Slicer3_LIB/VTK-build/bin/$::VTK_BUILD_SUBDIR/$zlib \
        -DPNG_PNG_INCLUDE_DIR:PATH=$::Slicer3_LIB/VTK/Utilities/vtkpng \
        -DTeem_PNG_DLLCONF_IPATH:PATH=$::Slicer3_LIB/VTK-build/Utilities \
        -DPNG_LIBRARY:FILEPATH=$::Slicer3_LIB/VTK-build/bin/$::VTK_BUILD_SUBDIR/$png \
        -DTeem_USE_LIB_INSTALL_SUBDIR:BOOL=ON \
        ../teem

      if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE teem.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
          if { $::GENLIB(test-type) == "" } {
            runcmd $::MAKE teem.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
          } else {
            runcmd $::MAKE teem.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE

            # - only Release mode is being built by ctest in spite of explicit request for debug...
            # running ctest through visual studio is broken in cmake2.4, so run ctest directly
            # runcmd $::CMAKE_PATH/bin/ctest -D $::GENLIB(test-type) -C $::VTK_BUILD_TYPE
          }
        }
      } else {
        if { $::GENLIB(test-type) == "" } {
          eval runcmd $::MAKE 
        } else {
          eval runcmd $::MAKE $::GENLIB(test-type)
        }
      }
  }
}



################################################################################
# Get and build OpenIGTLink 
#

if { [BuildThis $::OPENIGTLINK_TEST_FILE "openigtlink"] == 1 && [string tolower $::USE_OPENIGTLINK] == "on" } {
    cd $Slicer3_LIB

    runcmd $::SVN co $::OpenIGTLink_TAG OpenIGTLink

    if {$::GENLIB(buildit)} {
      file mkdir $Slicer3_LIB/OpenIGTLink-build
      cd $Slicer3_LIB/OpenIGTLink-build

      if {$isDarwin} {
        runcmd $::CMAKE \
            -G$GENERATOR \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_SKIP_RPATH:BOOL=OFF \
            -DOpenIGTLink_DIR:FILEPATH=$Slicer3_LIB/OpenIGTLink-build \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            ../OpenIGTLink
      } else {
        runcmd $::CMAKE \
            -G$GENERATOR \
            -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
            -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
            -DBUILD_SHARED_LIBS:BOOL=ON \
            -DCMAKE_SKIP_RPATH:BOOL=ON \
            -DOpenIGTLink_DIR:FILEPATH=$Slicer3_LIB/OpenIGTLink-build \
            -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
            ../OpenIGTLink
      }

      if { $isDarwin } {
        runcmd $::CMAKE \
          -DCMAKE_SHARED_LINKER_FLAGS:STRING="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib" \
          -DCMAKE_EXE_LINKER_FLAGS="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib" \
          ../OpenIGTLink
      }

      if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE OpenIGTLink.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE OpenIGTLink.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
        }
      } else {
        # Running this cmake again will populate those CMake variables 
        eval runcmd $::CMAKE ../OpenIGTLink 

        eval runcmd $::MAKE 
      }
  }
}


################################################################################
# Get and build BatchMake
#
#

if { ![file exists $::BatchMake_TEST_FILE] || $::GENLIB(update) } {
    cd $::Slicer3_LIB

    runcmd $::CVS -d :pserver:anoncvs:@batchmake.org:/cvsroot/BatchMake co -r $::BatchMake_TAG BatchMake

    file mkdir $::Slicer3_LIB/BatchMake-build
    cd $::Slicer3_LIB/BatchMake-build

    runcmd $::CMAKE \
        -G$GENERATOR \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DUSE_FLTK:BOOL=OFF \
        -DDASHBOARD_SUPPORT:BOOL=OFF \
        -DGRID_SUPPORT:BOOL=ON \
        -DUSE_SPLASHSCREEN:BOOL=OFF \
        -DITK_DIR:FILEPATH=$ITK_BINARY_PATH \
        ../BatchMake

    if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE BatchMake.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE BatchMake.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
        }
    } else {
        eval runcmd $::MAKE
    }
}


################################################################################
# Get and build SLICERLIBCURL (slicerlibcurl)
#
#

if { [BuildThis $::SLICERLIBCURL_TEST_FILE "libcurl"] == 1 } {
    cd $::Slicer3_LIB

    runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/cmcurl cmcurl
    if {$::GENLIB(buildit)} {

      file mkdir $::Slicer3_LIB/cmcurl-build
      cd $::Slicer3_LIB/cmcurl-build
if {$isSolaris} {
      runcmd $::CMAKE \
        -G$GENERATOR \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DBUILD_TESTING:BOOL=OFF \
        ../cmcurl
     } else {
      runcmd $::CMAKE \
        -G$GENERATOR \
        -DCMAKE_BUILD_TYPE:STRING=$::VTK_BUILD_TYPE \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
        -DCMAKE_CXX_COMPILER:STRING=$COMPILER_PATH/$COMPILER \
        -DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=$COMPILER_PATH/$COMPILER \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        ../cmcurl
     }
      if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE SLICERLIBCURL.dsw /MAKE "ALL_BUILD - $::VTK_BUILD_TYPE"
        } else {
            runcmd $::MAKE SLICERLIBCURL.SLN /out buildlog.txt /build  $::VTK_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE
      }
  }
}


if {! $::GENLIB(buildit)} {
 exit 0
}

# Are all the test files present and accounted for?  If not, return error code

if { ![file exists $::CMAKE] } {
    puts "CMake test file $::CMAKE not found."
}
if { ![file exists $::Teem_TEST_FILE] } {
    puts "Teem test file $::Teem_TEST_FILE not found."
}
if { ![file exists $::OPENIGTLINK_TEST_FILE] && [string tolower $::USE_OPENIGTLINK] == "on" } {
    puts "OpenIGTLink test file $::OPENIGTLINK_TEST_FILE not found."
}
if { ![file exists $::SLICERLIBCURL_TEST_FILE] } {
    puts "SLICERLIBCURL test file $::SLICERLIBCURL_TEST_FILE not found."
}
if { ![file exists $::TCL_TEST_FILE] } {
    puts "Tcl test file $::TCL_TEST_FILE not found."
}
if { ![file exists $::TK_TEST_FILE] } {
    puts "Tk test file $::TK_TEST_FILE not found."
}
if { ![file exists $::ITCL_TEST_FILE] } {
    puts "incrTcl test file $::ITCL_TEST_FILE not found."
}
if { ![file exists $::IWIDGETS_TEST_FILE] } {
    puts "iwidgets test file $::IWIDGETS_TEST_FILE not found."
}
if { ![file exists $::BLT_TEST_FILE] } {
    puts "BLT test file $::BLT_TEST_FILE not found."
}
if { ![file exists $::VTK_TEST_FILE] } {
    puts "VTK test file $::VTK_TEST_FILE not found."
}
if { ![file exists $::ITK_TEST_FILE] } {
    puts "ITK test file $::ITK_TEST_FILE not found."
}

if { ![file exists $::CMAKE] || \
         ![file exists $::Teem_TEST_FILE] || \
         ![file exists $::SLICERLIBCURL_TEST_FILE] || \
         ![file exists $::TCL_TEST_FILE] || \
         ![file exists $::TK_TEST_FILE] || \
         ![file exists $::ITCL_TEST_FILE] || \
         ![file exists $::IWIDGETS_TEST_FILE] || \
         ![file exists $::BLT_TEST_FILE] || \
         ![file exists $::VTK_TEST_FILE] || \
         ![file exists $::ITK_TEST_FILE] } {
    puts "Not all packages compiled; check errors and run genlib.tcl again."
    exit 1 
} else { 
    puts "All packages compiled."
    exit 0 
}
