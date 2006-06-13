#!/bin/sh
# \
exec wish "$0" ${1+"$@"}

#####################################################################
# arg parsing

proc Usage { {msg ""} } {
    global SLICER
    
    set msg "$msg\nusage: wikimage \[options\] <image_filename>"
    set msg "$msg\n  <image_filename> is the file to upload"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   -u --user : wiki user name (default is env USER)"
    set msg "$msg\n   -p --password : wiki login password"
    set msg "$msg\n   --url : url of the wiki (default wiki.na-mic.org)"
    set msg "$msg\n   -c --cookie-jar : file to save your cookies (default 'mycookies')"
    set msg "$msg\n   --help : prints this message and exits"
    puts stderr $msg
}

set ::WIKIMAGE(url) "http://wiki.na-mic.org"
set ::WIKIMAGE(user) $::env(USER)
set ::WIKIMAGE(password) ""
set ::WIKIMAGE(cookie-jar) "mycookies"

set strippedargs ""
set argc [llength $argv]

for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--clean" -
        "-f" {
            set ::WIKIMAGE(clean) "true"
        }
        "--url" {
            incr i ;# skip this argument
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set ::WIKIMAGE(url) [lindex $argv $i]
            }
        }
        "-u" -
        "--user" {
            incr i ;# skip this argument
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set ::WIKIMAGE(user) [lindex $argv $i]
            }
        }
        "-p" -
        "--password" {
            incr i ;# skip this argument
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set ::WIKIMAGE(password) [lindex $argv $i]
            }
        }
        "-c" -
        "--cookie-jar" {
            incr i ;# skip this argument
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set ::WIKIMAGE(cookie-jar) [lindex $argv $i]
            }
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

if {$argc != 1 } {
    Usage "Only one file at a time supported"
    exit 1
}

set ::WIKIMAGE(filename) $argv
set ::WIKIMAGE(imagename) [file tail $::WIKIMAGE(filename)]

#
#####################################################################

#####################################################################
#
# curl/wiki helper procs
#
# some handy info 
# http://curl.haxx.se/docs/httpscripting.html
#

proc login {} {

    if { $::WIKIMAGE(password) == "" } {
        set pw [get_password]
    } else {
        set pw $::WIKIMAGE(password)
    }

    set cmd "exec curl --verbose --cookie-jar $::WIKIMAGE(cookie-jar)  \
                --form wpName=$::WIKIMAGE(user)  \
                --form wpPassword=$pw \
                $::WIKIMAGE(url)/index.php/Special:Userlogin \
                --form wpLoginattempt=\"Log in\" "

    set ret [catch $cmd res]

    if { $ret && $::errorCode != "NONE" } {
        error $::errorCode
    }

    set fp [open c:/tmp/login.html "w"]
    puts $fp $res
    close $fp

    return $res
}

proc check_file_exists {} {


    set ret [catch "exec curl --url $::WIKIMAGE(url)/$::WIKIMAGE(imagename)" res]

    if { [string first "No file by this name exists" $res] == 1 } {
      return 0
    } else {
      return 1
    }
}

proc upload {} {

    exec curl \
        --basic \
        --url $::WIKIMAGE(url)/index.php/Special:Upload \
        --cookie $::WIKIMAGE(cookie-jar) \
        --form wpUploadFile=@$::WIKIMAGE(filename) \
        --form wpDestFile=$::WIKIMAGE(imagename) \
        --form wpUpload="Upload File" \
        > c:/tmp/ff.html ; c:/Program\ Files/Mozilla\ Firefox/firefox.exe file://c:/tmp/ff.html
}


# put up a simple password prompt dialog box
proc get_password {} {

    if { [catch "package require Tk"] } {
        puts -nonewline "password: " ; flush stdout
        exec stty -echo
        gets stdin pw
        exec stty echo
        puts ""
    } else {
        wm withdraw .
        catch "destroy .pw"
        toplevel .pw
        pack [label .pw.l -text "Password: "] -side left 
        pack [entry .pw.e -show "*"] -side left 
        bind .pw.e <Return> "destroy .pw.b"
        pack [button .pw.b -text "Ok" -command "destroy .pw.b"] -side bottom -fill x
        focus .pw.e
        tkwait window .pw.b
        set pw [.pw.e get]
        destroy .pw
        return $pw
    }
}

#
#####################################################################

#####################################################################
#
# actual application
# - relies on the WIKIMAGE global array for parameters

if { [check_file_exists] } {
  puts stderr "Sorry, $::WIKIMAGE(imagename) already exists on $WIKIMAGE(url)"
  exit -1
}

login 

upload

exit

# end
#####################################################################
