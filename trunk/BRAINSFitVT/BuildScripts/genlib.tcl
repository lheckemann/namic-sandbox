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
# run genlib from the $::PACKAGE_NAME directory next to where you want the packages to be built
# E.g. if you run /home/pieper/$::PACKAGE_NAME/BuildScripts/genlib.tcl it will create
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

################################################################################
#
# check to see if need to build a package
# returns 1 if need to build, 0 else
# if { [BuildThis  ""] == 1 } {
proc BuildThis { testFile packageName } {
#    catch { runcmd ls -l $testFile }
    if {![file exists $testFile] || $::GENLIB(update) || [lsearch $::GENLIB(buildList) $packageName] != -1} {
        puts "Building $packageName (testFile = $testFile, update = $::GENLIB(update), buildlist = $::GENLIB(buildList) )"
        return 1
    } else {
        puts "Skipping $packageName"
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
    set msg "$msg\n  \[target\] is the the PACKAGE_LIB directory"
    set msg "$msg\n             and is determined automatically if not specified"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   --help : prints this message and exits"
    set msg "$msg\n   --clean : delete the target first"
    set msg "$msg\n   --update : do a cvs update even if there's an existing build"
    set msg "$msg\n   --release : compile with optimization flags"
    set msg "$msg\n   --nobuild : only download and/or update, but don't build"
    set msg "$msg\n   optional space separated list of packages to build (lower case)"
    set msg "$msg\n   --build-cmake : build cmake"
    set msg "$msg\n   --build-tcl : build tcl"
    set msg "$msg\n   --build-tk : build tk"
    set msg "$msg\n   --build-vtk : build vtk"
    set msg "$msg\n   --build-kwwidgets : build kwwidgets"
    set msg "$msg\n   --build-itk : build itk"
    set msg "$msg\n   --build-vtkinria3d : build vtkinria3d"
    puts stderr $msg
}

set GENLIB(clean) "false"
set GENLIB(update) "false"
set GENLIB(buildit) "true"
set ::GENLIB(buildList) ""

set isRelease 0
set strippedargs ""
set argc [llength $argv]

set BUILD_PACKAGE(cmake) "no"
set BUILD_PACKAGE(tcl) "no"
set BUILD_PACKAGE(tk) "no"
set BUILD_PACKAGE(vtk) "no"
set BUILD_PACKAGE(kwwidgets) "no"
set BUILD_PACKAGE(itk) "no"
set BUILD_PACKAGE(vtkinria3d) "no"
set ::CMAKE_BUILD_TYPE "DEBUG"
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--build-cmake" {
            set BUILD_PACKAGE(cmake) "yes"
        }
        "--build-tcl" {
            set BUILD_PACKAGE(tcl) "yes"
        }
        "--build-tk" {
            set BUILD_PACKAGE(tk) "yes"
        }
        "--build-vtk" {
            set BUILD_PACKAGE(vtk) "yes"
        }
        "--build-kwwidgets" {
            set BUILD_PACKAGE(kwwidgets) "yes"
        }
        "--build-itk" {
            set BUILD_PACKAGE(itk) "yes"
        }
        "--build-vtkinria3d" {
            set BUILD_PACKAGE(vtkinria3d) "yes"
        }
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
            set ::CMAKE_BUILD_TYPE "Release"
        }
        "--nobuild" {
            set ::GENLIB(buildit) "false"
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
puts "Stripped args = $argv"

set ::PACKAGE_LIB ""
if {$argc > 2 } {
    Usage
    exit 1
    # the stripped args list now has the PACKAGE_LIB first and then the list of packages to build
    set ::GENLIB(buildList) [lrange $strippedargs 1 end]
    set strippedargs [lindex $strippedargs 0]
# puts "Got the list of package to build: '$::GENLIB(buildList)' , stripped args = $strippedargs"
} 
set ::PACKAGE_LIB [lindex $argv 0]
set ::PACKAGE_BUILD [lindex $argv 1]




################################################################################
# First, set up the directory
# - determine the location
# - determine the build
# 

# hack to work around lack of normalize option in older tcl
# set PACKAGE_HOME [file dirname [file dirname [file normalize [info script]]]]
set script [info script]
catch {set script [file normalize $script]}
set ::PACKAGE_HOME [file dirname [file dirname $script]]
set cwd [pwd]
cd $cwd

#######
#
# Note: the local vars file, $PACKAGE_HOME/BuildScripts/package_variables.tcl, overrides the default values in this script
# - use it to set your local environment and then your change won't
#   be overwritten when this file is updated
#
set localvarsfile $PACKAGE_HOME/BuildScripts/package_variables.tcl
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
    set ::CMAKE_BUILD_TYPE "Release"
    set ::env(CMAKE_BUILD_TYPE) $::CMAKE_BUILD_TYPE
    if ($isWindows) {
        set ::VTK_BUILD_SUBDIR "Release"
    } else {
        set ::VTK_BUILD_SUBDIR ""
    }
    puts "Overriding package_variables.tcl; CMAKE_BUILD_TYPE is '$::env(CMAKE_BUILD_TYPE)', VTK_BUILD_SUBDIR is '$::VTK_BUILD_SUBDIR'"
    set ::VTK_DEBUG_LEAKS "OFF"

}

# tcl file delete is broken on Darwin, so use rm -rf instead
if { $GENLIB(clean) } {
    puts "Deleting slicer lib files..."
    if { $isDarwin } {
        runcmd rm -rf $PACKAGE_LIB
        if { [file exists $PACKAGE_LIB/tcl/isPatched] } {
            runcmd rm $PACKAGE_LIB/tcl/isPatched
        }

    } else {
        file delete -force $PACKAGE_LIB
    }
}

if { ![file exists $PACKAGE_LIB] } {
    file mkdir $PACKAGE_LIB
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
puts "+++ DEBUG BUILD_PACKAGE(cmake) = $BUILD_PACKAGE(cmake) USE_SYSTEM_CMAKE = $::USE_SYSTEM_CMAKE"
set buildthis [BuildThis $::CMAKE "cmake" ]
puts "*** DEBUG puts Buildthis $buildthis "

if { [string compare $BUILD_PACKAGE(cmake) "yes" ] == 0 } {
if { [string compare $::USE_SYSTEM_CMAKE "NO" ] == 0 } {
if { [BuildThis $::CMAKE "cmake"] == 1 } {
    file mkdir $::CMAKE_PATH
    cd $PACKAGE_LIB


    if {$isWindows} {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/Binaries/Windows/CMake-build CMake-build
    } else {
       runcmd $::CVS -d :pserver:anonymous:cmake@www.cmake.org:/cvsroot/CMake login
       eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anonymous@www.cmake.org:/cvsroot/CMake checkout -r $::CMAKE_TAG CMake"

        if {$::GENLIB(buildit)} {
          cd $::CMAKE_PATH
          if { $isSolaris } {
              # make sure to pick up curses.h in /local/os/include
              runcmd $PACKAGE_LIB/CMake/bootstrap --init=$PACKAGE_HOME/BuildScripts/spl.cmake.init
          } else {
              ## CMAKE IS TERRIBLY SLOW WHEN BUILT IN DEBUG MODE
              set TEMP_CFLAGS $env(CFLAGS);
              set TEMP_CXXFLAGS $env(CXXFLAGS);
              unset env(CFLAGS);
              unset env(CXXFLAGS);
              runcmd $PACKAGE_LIB/CMake/bootstrap
              set env(CFLAGS) $TEMP_CFLAGS;
              set env(CXXFLAGS) $TEMP_CXXFLAGS;
          }
          eval runcmd $::MAKE
       }
    }
}
}
}


################################################################################
# Get and build tcl, tk
#
#

if { $isDarwin } {
set SAVECFLAGS $::env(CFLAGS)
set SAVECXXFLAGS $::env(CXXFLAGS)
set ::env(CFLAGS) "$::env(CFLAGS) -framework corefoundation"
set ::env(CXXFLAGS) "$::env(CXXFLAGS) -framework corefoundation"
}

# on windows, tcl won't build right, as can't configure, so save commands have to run
if { [string compare $BUILD_PACKAGE(tcl) "yes" ] == 0 } {
if { [BuildThis $::TCL_TEST_FILE "tcl"] == 1 } {

    if {$isWindows} {
      runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/Binaries/Windows/tcl-build tcl-build
    }

    file mkdir $PACKAGE_LIB/tcl
    cd $PACKAGE_LIB/tcl

    runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/tcl/tcl tcl
    if {$::GENLIB(buildit)} {
      if {$isWindows} {
          # can't do windows
      } else {
          cd $PACKAGE_LIB/tcl/tcl/unix

          eval runcmd ./configure --prefix=$PACKAGE_LIB/tcl-build $::TCL_SHARED_FLAGS
          eval runcmd $::MAKE
          eval runcmd $::MAKE install
      }
    }
}
}

if { [string compare $BUILD_PACKAGE(tk) "yes" ] == 0 } {
if { [BuildThis $::TK_TEST_FILE "tk"] == 1 } {
    cd $PACKAGE_LIB/tcl

    runcmd $::SVN co http://svn.slicer.org/Slicer3-lib-mirrors/trunk/tcl/tk tk

    if {$::GENLIB(buildit)} {
      if {$isWindows} {
         # ignore, already downloaded with tcl
      } else {
         cd $PACKAGE_LIB/tcl/tk/unix
         if { $isDarwin } {
                  eval runcmd ./configure --with-tcl=$PACKAGE_LIB/tcl-build/lib --prefix=$PACKAGE_LIB/tcl-build --disable-corefoundation --x-libraries=/usr/X11R6/lib --x-includes=/usr/X11R6/include --with-x $::TCL_SHARED_FLAGS
               } else {
                  eval runcmd ./configure --with-tcl=$PACKAGE_LIB/tcl-build/lib --prefix=$PACKAGE_LIB/tcl-build $::TCL_SHARED_FLAGS
               }
         eval runcmd $::MAKE
         eval runcmd $::MAKE install
         
         file copy -force $PACKAGE_LIB/tcl/tk/generic/default.h $PACKAGE_LIB/tcl-build/include
         file copy -force $PACKAGE_LIB/tcl/tk/unix/tkUnixDefault.h $PACKAGE_LIB/tcl-build/include
      }
   }
}
}
################################################################################
# Get and build vtk
#

if { [string compare $BUILD_PACKAGE(vtk) "yes" ] == 0 } {
if { [BuildThis $::VTK_TEST_FILE "vtk"] == 1 } {
    cd $PACKAGE_LIB

    eval "runcmd $::CVS -d :pserver:anonymous:vtk@public.kitware.com:/cvsroot/VTK login"
    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anonymous@public.kitware.com:/cvsroot/VTK checkout -D $::VTK_DATE VTK"

    # Andy's temporary hack to get around wrong permissions in VTK cvs repository
    # catch statement is to make file attributes work with RH 7.3
    if { !$isWindows } {
        catch "file attributes $PACKAGE_LIB/VTK/VTKConfig.cmake.in -permissions a+rw"
    }
    if {$::GENLIB(buildit)} {

      file mkdir $PACKAGE_LIB/VTK-build
      cd $PACKAGE_LIB/VTK-build

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
        eval runcmd $::CMAKE \
            $::CMAKE_COMPILE_SETTINGS  \
            $::OPENGL_COMPILE_SETTINGS \
            -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
            -DCMAKE_SKIP_RPATH:BOOL=ON \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_COCOA:BOOL=OFF \
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
        eval runcmd $::CMAKE \
            $::CMAKE_COMPILE_SETTINGS  \
            $::OPENGL_COMPILE_SETTINGS \
            -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
            -DCMAKE_SKIP_RPATH:BOOL=OFF \
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
            $USE_VTK_ANSI_STDLIB \
            ../VTK
      } else {
        eval runcmd $::CMAKE \
            $::CMAKE_COMPILE_SETTINGS \
            -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
            -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
            -DCMAKE_SKIP_RPATH:BOOL=ON \
            -DBUILD_TESTING:BOOL=OFF \
            -DVTK_USE_CARBON:BOOL=OFF \
            -DVTK_USE_COCOA:BOOL=OFF \
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
            runcmd $::MAKE VTK.dsw /MAKE "ALL_BUILD - $::CMAKE_BUILD_TYPE"
        } else {
            runcmd $::MAKE VTK.SLN /build  $::CMAKE_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE 
    }
  }
}
}
if { $isDarwin } {
  set ::env(CFLAGS) "${SAVECFLAGS}"
  set ::env(CXXFLAGS) "${SAVECXXFLAGS}"
}


################################################################################
# Get and build kwwidgets
#

if { [string compare $BUILD_PACKAGE(kwwidgets) "yes" ] == 0 } {
if { [BuildThis $::KWWidgets_TEST_FILE "kwwidgets"] == 1 } {
    cd $PACKAGE_LIB

    runcmd $::CVS -d :pserver:anoncvs:@www.kwwidgets.org:/cvsroot/KWWidgets login
#    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.kwwidgets.org:/cvsroot/KWWidgets checkout -r $::KWWidgets_TAG KWWidgets"
#    runcmd cvs -d :pserver:anoncvs@www.kwwidgets.org:/cvsroot/KWWidgets co KWWidgets
#    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.kwwidgets.org:/cvsroot/KWWidgets checkout -r $::KWWidgets_TAG KWWidgets"
    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.kwwidgets.org:/cvsroot/KWWidgets checkout -D $::KWWidgets_DATE KWWidgets"

    if {$::GENLIB(buildit)} {
      file mkdir $PACKAGE_LIB/KWWidgets-build
      cd $PACKAGE_LIB/KWWidgets-build



     eval runcmd $::CMAKE \
        $::CMAKE_COMPILE_SETTINGS  \
        -DVTK_DIR:PATH=$PACKAGE_LIB/VTK-build \
        -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
        -DCMAKE_SKIP_RPATH:BOOL=ON \
        -DBUILD_EXAMPLES:BOOL=ON \
        -DKWWidgets_BUILD_EXAMPLES:BOOL=ON \
        -DBUILD_TESTING:BOOL=ON \
        -DKWWidgets_BUILD_TESTING:BOOL=ON \
        -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
        ../KWWidgets

    if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE KWWidgets.dsw /MAKE "ALL_BUILD - $::CMAKE_BUILD_TYPE"
        } else {
            runcmd $::MAKE KWWidgets.SLN /build  $::CMAKE_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE 
      }
  }
}
}

################################################################################
# Get and build itk
#

if { [string compare $BUILD_PACKAGE(itk) "yes" ] == 0 } {
if { [BuildThis $::ITK_TEST_FILE "itk"] == 1 } {
    cd $PACKAGE_LIB

#    eval runcmd $::SVN co http://svn.slicer.org/$::PACKAGE_NAME/$::PACKAGE_NAME/Slicer3-lib-mirrors/trunk/Insight Insight
## Checkout CableSwig
## Use a date tag where ITK and CableSwig can be compiled together.
# Moved up to package_variables.tcl    set ::ITK_CABLE_DATE "2008-03-09"
#    eval runcmd $::CVS update -dAP -D $::ITK_CABLE_DATE
    runcmd $::CVS -d :pserver:anoncvs:@www.vtk.org:/cvsroot/Insight login
    if { [ string compare $::ITK_CABLE_DATE "CVS" ] == 0} {
       eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.vtk.org:/cvsroot/Insight co -dAP Insight"
    } else {
       eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.vtk.org:/cvsroot/Insight checkout -D $::ITK_CABLE_DATE Insight"
    }
    puts "ADD_WRAPITK = $::ADD_WRAPITK"
    if { [string compare $::ADD_WRAPITK "ON" ] == 0 } {

        cd $::PACKAGE_LIB/Insight/Utilities
#         set patchfile "$::PACKAGE_HOME/SlicerPatches/wrap_itkWindowedSincInterpolateImageFunction.cmake"
#         set plib "$::PACKAGE_LIB/Insight/Wrapping/WrapITK/Modules/Interpolators"
#         puts "PATCHFILE = $patchfile PACKAGE_LIB = $plib"
#         if { [ file exists  $plib ] == 1 } {
#             if { [ file exists $patchfile ] } {
#             puts "***** copying $patchfile"
#             runcmd cp $patchfile $plib
#             }
#         }
    if { [ string compare $::ITK_CABLE_DATE "CVS" ] == 0} {
            eval "runcmd $::CVS -d :pserver:anonymous@public.kitware.com:/cvsroot/CableSwig co -dAP  CableSwig"
        } else {
            eval "runcmd $::CVS -d :pserver:anonymous@public.kitware.com:/cvsroot/CableSwig co -D $::ITK_CABLE_DATE  CableSwig"
        }
    }
    cd $::PACKAGE_LIB

    if {$::GENLIB(buildit)} {
      file mkdir $PACKAGE_LIB/Insight-build
      cd $PACKAGE_LIB/Insight-build


      if {$isDarwin} {
     eval runcmd $::CMAKE \
        $::CMAKE_COMPILE_SETTINGS  \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
        -DBUILD_TESTING:BOOL=OFF \
        -DCMAKE_INSTALL_PREFIX:PATH=$::PACKAGE_BUILD \
        -DCMAKE_SKIP_RPATH:BOOL=OFF \
        -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON \
        -DITK_USE_REVIEW:BOOL=ON \
        -DITK_USE_REVIEW_STATISTICS:BOOL=ON \
        -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON \
        -DITK_USE_ORIENTED_IMAGE_DIRECTION:BOOL=ON \
        -DITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE:BOOL=ON \
        -DITK_USE_PORTABLE_ROUND:BOOL=ON \
        -DITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY:BOOL=ON \
         { -DWRAP_ITK_DIMS:STRING=2\;3 } \
        -DUSE_WRAP_ITK:BOOL=$::ADD_WRAPITK \
        -DINSTALL_WRAP_ITK_COMPATIBILITY:BOOL=OFF \
        -DWRAP_ITK_JAVA:BOOL=OFF \
        -DWRAP_ITK_PYTHON:BOOL=OFF \
        -DWRAP_ITK_TCL:BOOL=$::ADD_WRAPITK  \
        -DWRAP_complex_float:BOOL=OFF \
        -DWRAP_covariant_vector_float:BOOL=OFF \
        -DWRAP_float:BOOL=ON \
        -DWRAP_rgb_signed_short:BOOL=OFF \
        -DWRAP_rgb_unsigned_char:BOOL=OFF \
        -DWRAP_rgb_unsigned_short:BOOL=OFF \
        -DWRAP_signed_short:BOOL=ON \
        -DWRAP_unsigned_char:BOOL=ON \
        -DWRAP_unsigned_short:BOOL=OFF \
        -DWRAP_vector_float:BOOL=ON \
        -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
        -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
        -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
        -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
        -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
        -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
        ../Insight
      } else {
     eval runcmd $::CMAKE \
        $::CMAKE_COMPILE_SETTINGS \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
        -DBUILD_TESTING:BOOL=OFF \
        -DCMAKE_INSTALL_PREFIX:PATH=$::PACKAGE_BUILD \
        -DCMAKE_SKIP_RPATH:BOOL=ON \
        -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON \
        -DITK_USE_REVIEW:BOOL=ON \
        -DITK_USE_REVIEW_STATISTICS:BOOL=ON \
        -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON \
        -DITK_USE_ORIENTED_IMAGE_DIRECTION:BOOL=ON \
        -DITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE:BOOL=ON \
        -DITK_USE_PORTABLE_ROUND:BOOL=ON \
        -DITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY:BOOL=ON \
        { -DWRAP_ITK_DIMS:STRING=2\;3 } \
        -DUSE_WRAP_ITK:BOOL=$::ADD_WRAPITK \
        -DINSTALL_WRAP_ITK_COMPATIBILITY:BOOL=OFF \
        -DWRAP_ITK_JAVA:BOOL=OFF \
        -DWRAP_ITK_PYTHON:BOOL=OFF \
        -DWRAP_ITK_TCL:BOOL=$::ADD_WRAPITK  \
        -DWRAP_complex_float:BOOL=OFF \
        -DWRAP_covariant_vector_float:BOOL=OFF \
        -DWRAP_float:BOOL=ON \
        -DWRAP_rgb_signed_short:BOOL=OFF \
        -DWRAP_rgb_unsigned_char:BOOL=OFF \
        -DWRAP_rgb_unsigned_short:BOOL=OFF \
        -DWRAP_signed_short:BOOL=ON \
        -DWRAP_unsigned_char:BOOL=ON \
        -DWRAP_unsigned_short:BOOL=OFF \
        -DWRAP_vector_float:BOOL=ON \
        -DTCL_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
        -DTK_INCLUDE_PATH:PATH=$TCL_INCLUDE_DIR \
        -DTCL_LIBRARY:FILEPATH=$::VTK_TCL_LIB \
        -DTK_LIBRARY:FILEPATH=$::VTK_TK_LIB \
        -DTCL_TCLSH:FILEPATH=$::VTK_TCLSH \
        -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
        ../Insight
      }

      if {$isWindows} {
        if { $MSVC6 } {
            runcmd $::MAKE ITK.dsw /MAKE "ALL_BUILD - $::CMAKE_BUILD_TYPE"
        } else {
            runcmd $::MAKE ITK.SLN /build  $::CMAKE_BUILD_TYPE
        }
      } else {
        eval runcmd $::MAKE 
        eval runcmd $::MAKE install
    }
    puts "Patching ITK..."

    set fp1 [open "$PACKAGE_LIB/Insight-build/Utilities/nifti/niftilib/cmake_install.cmake" r]
    set fp2 [open "$PACKAGE_LIB/Insight-build/Utilities/nifti/znzlib/cmake_install.cmake" r]
    set data1 [read $fp1]
#    puts "data1 is $data1"
    set data2 [read $fp2]
#    puts "data2 is $data2"

    close $fp1
    close $fp2

    regsub -all /usr/local/lib $data1 \${CMAKE_INSTALL_PREFIX}/lib data1
    regsub -all /usr/local/include $data1 \${CMAKE_INSTALL_PREFIX}/include data1
    regsub -all /usr/local/lib $data2 \${CMAKE_INSTALL_PREFIX}/lib data2
    regsub -all /usr/local/include $data2 \${CMAKE_INSTALL_PREFIX}/include data2

    set fw1 [open "$PACKAGE_LIB/Insight-build/Utilities/nifti/niftilib/cmake_install.cmake" w]
    set fw2 [open "$PACKAGE_LIB/Insight-build/Utilities/nifti/znzlib/cmake_install.cmake" w]

    puts -nonewline $fw1 $data1
#    puts "data1out is $data1"
    puts -nonewline $fw2 $data2
#    puts "data2out is $data2"
 
    close $fw1
    close $fw2
  }
}
}


##set inriaSiteStatus "Down"
set inriaSiteStatus "Up"
if { [string compare $BUILD_PACKAGE(vtkinria3d) "yes" ] == 0 } {
#
# get and build vtkinria3d
if { ![file exists $::vtkinria3d_TEST_FILE] || $::GENLIB(update) } {
    cd $PACKAGE_LIB

#    catch { eval runcmd $::SVN co svn://scm.gforge.inria.fr/svn/vtkinria3d -r "\\{$::vtkinria3d_TAG\\}" }
    catch { eval runcmd $::SVN co svn://scm.gforge.inria.fr/svn/vtkinria3d/trunk -r "\\{$::vtkinria3d_TAG\\}" vtkinria3d }
    file mkdir $PACKAGE_LIB/vtkinria3d-build
    cd $PACKAGE_LIB/vtkinria3d-build

    if { $isWindows } {
      set ::GETBUILDTEST(cpack-generator) "NSIS"
    } else {
      set ::GETBUILDTEST(cpack-generator) "STGZ"
    }

    if {$isWindows} {
      set ::env(PATH) "$::env(PATH):$::PACKAGE_BUILD/lib/InsightToolkit:$::PACKAGE_BUILD/lib/vtk"
    }
    if { $isDarwin } {
      if { [info exists ::env(DYLD_LIBRARY_PATH)] } {
        set ::env(DYLD_LIBRARY_PATH) "$::env(DYLD_LIBRARY_PATH):$::PACKAGE_BUILD/lib/InsightToolkit:$::PACKAGE_BUILD/lib/vtk-5.1"
      } else {
        set ::env(DYLD_LIBRARY_PATH) "$::PACKAGE_BUILD/lib/InsightToolkit:$::PACKAGE_BUILD/lib/vtk"
      }
    }
    if { $isLinux } {
      if { [info exists ::env(LD_LIBRARY_PATH)] } {
        set ::env(LD_LIBRARY_PATH) "$::env(LD_LIBRARY_PATH):$::PACKAGE_BUILD/lib/InsightToolkit:$::PACKAGE_BUILD/lib/vtk-5.1"
      } else {
        set ::env(LD_LIBRARY_PATH) "$::PACKAGE_BUILD/lib/InsightToolkit:$::PACKAGE_BUILD/lib/vtk"
      }
    }

    eval runcmd $::CMAKE \
        $::CMAKE_COMPILE_SETTINGS  \
        $::OPENGL_COMPILE_SETTINGS \
        -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
        -DCMAKE_INSTALL_PREFIX:PATH=$::PACKAGE_BUILD \
        -DITK_DIR:FILEPATH=$::PACKAGE_LIB/Insight-build \
        -DKWWidgets_DIR:FILEPATH=$::PACKAGE_LIB/KWWidgets-build \
        -DVTK_DIR:PATH=$PACKAGE_LIB/VTK-build \
        -DUSE_KWWidgets:BOOL=ON \
        -DUSE_ITK:BOOL=ON \
        -DCMAKE_BUILD_TYPE=$::CMAKE_BUILD_TYPE \
        ../vtkinria3d

#        -DCPACK_GENERATOR:STRING=$::GETBUILDTEST(cpack-generator) \
#        -DCPACK_PACKAGE_FILE_NAME:STRING=$::GETBUILDTEST(binary-filename) \

    if {$isWindows} {
        if { $MSVC6 } {
            eval runcmd $::MAKE vtkinria3d.dsw /MAKE "ALL_BUILD - $::CMAKE_BUILD_TYPE"
        } else {
            eval runcmd $::MAKE vtkinria3d.SLN /build  $::CMAKE_BUILD_TYPE
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
set file_missing 0
if { $BUILD_PACKAGE(cmake) == "yes" && ![file exists $::CMAKE] } {
    set file_missing = 1 
    puts "CMake test file $::CMAKE not found."
}
if { $BUILD_PACKAGE(tcl) == "yes" &&  ![file exists $::TCL_TEST_FILE] } {
    set file_missing = 1 
    puts "Tcl test file $::TCL_TEST_FILE not found."
}
if { $BUILD_PACKAGE(tk) == "yes" &&  ![file exists $::TK_TEST_FILE] } {
    set file_missing = 1 
    puts "Tk test file $::TK_TEST_FILE not found."
}
if { $BUILD_PACKAGE(vtk) == "yes" &&  ![file exists $::VTK_TEST_FILE] } {
    set file_missing = 1 
    puts "VTK test file $::VTK_TEST_FILE not found."
}
if { $BUILD_PACKAGE(itk) == "yes" &&  ![file exists $::ITK_TEST_FILE] } {
    set file_missing = 1 
    puts "ITK test file $::ITK_TEST_FILE not found."
}
if { $BUILD_PACKAGE(vtkinria3d) == "yes" &&  ![file exists $::vtkinria3d_TEST_FILE] } {
    set file_missing = 1 
    puts "vtkinria3d test file $::vtkinria3d_TEST_FILE not found."
}

if { $file_missing == 1 } {
    puts "Not all packages compiled; check errors and run genlib.tcl again."
    exit 1 
} else { 
    puts "All packages compiled."
    exit 0 
}
