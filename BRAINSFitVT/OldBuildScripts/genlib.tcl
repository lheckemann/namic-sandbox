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
    set msg "$msg\n  \[target\] is the the PACKAGE_LIB directory"
    set msg "$msg\n             and is determined automatically if not specified"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   --help : prints this message and exits"
    set msg "$msg\n   --clean : delete the target first"
    set msg "$msg\n   --update : do a cvs update even if there's an existing build"
    set msg "$msg\n   --release : compile with optimization flags"
    set msg "$msg\n   --nobuild : only download and/or update, but don't build"
    set msg "$msg\n   optional space separated list of packages to build (lower case)"
    puts stderr $msg
}

set GENLIB(clean) "false"
set GENLIB(update) "false"
set GENLIB(buildit) "true"
set ::GENLIB(buildList) ""

set isRelease 0
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
              runcmd $PACKAGE_LIB/CMake/configure
              set env(CFLAGS) $TEMP_CFLAGS;
              set env(CXXFLAGS) $TEMP_CXXFLAGS;
          }
          eval runcmd $::MAKE
       }
    }
}
}


################################################################################
# Get and build itk
#

if { [BuildThis $::ITK_TEST_FILE "itk"] == 1 } {
    cd $PACKAGE_LIB

#    eval runcmd $::SVN co http://svn.slicer.org/$::PACKAGE_NAME/$::PACKAGE_NAME/Slicer3-lib-mirrors/trunk/Insight Insight
## Checkout CableSwig
## Use a date tag where ITK and CableSwig can be compiled together.
# Moved up to package_variables.tcl    set ::ITK_CABLE_DATE "2008-03-09"
#    eval runcmd $::CVS update -dAP -D $::ITK_CABLE_DATE
    runcmd $::CVS -d :pserver:anoncvs:@www.vtk.org:/cvsroot/Insight login
    eval "runcmd $::CVS $CVS_CO_FLAGS -d :pserver:anoncvs@www.vtk.org:/cvsroot/Insight checkout -D $::ITK_CABLE_DATE Insight"
    cd $::PACKAGE_LIB
    if {$::GENLIB(buildit)} {
      file mkdir $PACKAGE_LIB/Insight-build
      cd $PACKAGE_LIB/Insight-build

     eval runcmd $::CMAKE \
        $::CMAKE_COMPILE_SETTINGS  \
          { -DWRAP_ITK_DIMS:STRING=2\;3 } \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_SHARED_LIBS:BOOL=$::BUILD_SHARED_LIBS \
        -DBUILD_TESTING:BOOL=OFF \
        -DCMAKE_INSTALL_PREFIX:PATH=$::PACKAGE_BUILD \
        -DCMAKE_SKIP_RPATH:BOOL=OFF \
        -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=OFF \
        -DITK_USE_REVIEW:BOOL=ON \
        -DITK_USE_ORIENTED_IMAGE_DIRECTION:BOOL=ON \
        -DITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE:BOOL=ON \
        -DCMAKE_BUILD_TYPE:STRING=$::CMAKE_BUILD_TYPE \
        ../Insight

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


# Are all the test files present and accounted for?  If not, return error code

if { ![file exists $::CMAKE] } {
    puts "CMake test file $::CMAKE not found."
}
if { ![file exists $::ITK_TEST_FILE] } {
    puts "ITK test file $::ITK_TEST_FILE not found."
}

if { ![file exists $::CMAKE] || \
         ![file exists $::ITK_TEST_FILE] } {
    puts "Not all packages compiled; check errors and run genlib.tcl again."
    exit 1 
} else { 
    puts "All packages compiled."
    exit 0 
}
