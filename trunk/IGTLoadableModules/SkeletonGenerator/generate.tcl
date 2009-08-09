#!/bin/sh
# the next line restarts using tclsh \
    exec tclsh "$0" "$@"


## default configurations
set ::GENERATOR(NAME) "Skeleton"
set ::GENERATOR(TYPE) "default"
set ::GENERATOR(WDIR)  "."

proc Usage { {msg ""} } {
    set msg "$msg\nusage: generate \[options\]"
    set msg "$msg\n  "
    set msg "$msg\n \[options\] : one of the following:"
    set msg "$msg\n   --type=<type name>        : type of module (default: \"$::GENERATOR(TYPE)\""
    set msg "$msg\n   --name=<module name>      : module name (default: \"$::GENERATOR(NAME)\")"
    set msg "$msg\n   --dir=<working directory> : working directory (default: \"$::GENERATOR(WDIR)\""
    set msg "$msg\n  "
    puts stderr $msg
}

set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--type=*" {
            set ::GENERATOR(TYPE) [string range $a 7 end]
        }
        "--name=*" {
            set ::GENERATOR(NAME) [string range $a 7 end]
        }
        "--dir=*" {
            set ::GENERATOR(WDIR) [string range $a 6 end]
        }
        #"--steps=*" {
        #    set ::GENERATOR(STEPS) [string range $a 8 end]
        #}
        "--help" {
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

puts "Module NAME = $::GENERATOR(NAME)"
puts "TYPE = $::GENERATOR(TYPE)"

set cwd [file dirname [info script]]
set template "$cwd/$::GENERATOR(TYPE)"

## check if the template directory exists
if { [file exists $template] } {
    puts "Using template data in: $template..."
} else {
    puts "ERROR: Could not find a template directory."
    exit 1
}

## check if the working directory exists
if { ![file exists $::GENERATOR(WDIR)] } {
    puts "ERROR: Could not find the working directory."
    exit 1
}

### check if the generator configuration exists
#if { [file exists "$template/SGConfig.txt"] } {
#    puts "Found a Skeleton Generator configuration in $template..."
#} else {
#    puts "ERROR: Could not find a Skeleton Generator configuration (SGConfig.txt) in $template."
#    exit 1
#}

## glob template files
puts "Searching template files..."
set files [glob "$template/*.in"]
foreach f $files {
    puts $f
}

## create target directory
puts "Creating a skeleton directory..."
set targetdir "$::GENERATOR(WDIR)/$::GENERATOR(NAME)"
file mkdir $targetdir

## create module configuration file
set fd [open "$targetdir/$::GENERATOR(NAME).txt" "w"]
puts $fd "Name: $::GENERATOR(NAME)"
puts $fd "GUIName: $::GENERATOR(NAME)"
close $fd

## To avoid the problem that sed command does not replace $${MODULE_NAME}$$ on Windows
if { $tcl_platform(platform) == "windows" } {
    set repModName "s/\\\\$\\\\$\{MODULE_NAME\}\\\\$\\\\$/$::GENERATOR(NAME)/g"
} else {
    set repModName "s/\\$\\$\{MODULE_NAME\}\\$\\$/$::GENERATOR(NAME)/g"
}

if { $::GENERATOR(TYPE) != "wizard" } {

    foreach src $files {
        #regsub -all "$template/*" $src "" srcfilename
        set srcfilename [file tail $src]


        #set srcfilename [string trimright $srcfilename ".in"]
        set srcfilename [string range $srcfilename 0 [expr [string length $srcfilename] - 4]]

        if { $srcfilename == "CMakeLists.txt" } {
            set dst "$targetdir/$srcfilename"
        } else {
           set dstfilename "vtk$::GENERATOR(NAME)$srcfilename"
           set dst "$targetdir/$dstfilename"
        }

        exec sed $repModName $src > $dst

    }
} else {   ## wizard template
    
    puts "Wizard skeleton is currently not supported."

}    









