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
    set msg "$msg\n   -u --user : wiki user name"
    set msg "$msg\n   -p --password : wiki login password"
    set msg "$msg\n   --url : url of the wiki"
    set msg "$msg\n   -c --cookie-jar : file to save your cookies"
    set msg "$msg\n   --help : prints this message and exits"
    puts stderr $msg
}

set ::WIKIMAGE(url) ""
set ::WIKIMAGE(user) ""
set ::WIKIMAGE(password) ""
set ::WIKIMAGE(cookie-jar) ""

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
    Usage
    exit 1
}
set ::WIKIMAGE(filename) $argv

#
#####################################################################

#####################################################################
#
# curl/wiki helper procs
#
# some handy info 
# http://curl.haxx.se/docs/httpscripting.html
#

proc login_test {} {

    set pw [get_password]

    set cmd "exec curl --verbose --cookie-jar mycookies  \
                --form wpName=pieper  \
                --form wpPassword=$pw \
                http://wiki.ncigt.org/IGT/index.php/Special:Userlogin \
                --form wpLoginattempt=\"Log in\" "

    set ret [catch $cmd res]

    if { $ret && $::errorCode != "NONE" } {
        error $::errorCode
    }

    set ret [catch "exec curl --cookie mycookies http://wiki.ncigt.org/IGT/index.php/Main_Page" res]

    if { $ret && $::errorCode != "NONE" } {
        error $::errorCode
    }

    return $res
}

proc upload_test {} {


# Note: the following syntax works to upload a file to the wiki if you copy the cookie information
# from firefox - not clear yet how to get curl to login itself and save the info

    exec curl \
        --cookie NAMICNewWikiDBUserID=3 \
        --cookie NAMICNewWikiDBUserName=Pieper \
        --cookie NAMICNewWikiDBToken=99930421212693660455417595135040 \
        --cookie NAMICNewWikiDB_session=9d8f4623b11f1e5563550664d0780d48 \
        --form wpUploadFile=@c:/pieper/bwh/birn-grant-work/mbirn-sedona-2006/mbirn_sedona_MondayMorning.jpg \
        --form wpDestFile=mbirn_sedona_MondayMorning.jpg \
        --form wpUpload="Upload File" \
        http://www.na-mic.org/Wiki/index.php/Special:Upload \
        > c:/tmp/ff.html ; c:/Program\ Files/Mozilla\ Firefox/firefox.exe file://c:/tmp/ff.html

}

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

    return $res
}

proc upload {} {

    set imagename [file tail $::WIKIMAGE(filename)]

    exec curl \
        --cookie-jar mycookies \
        --form wpUploadFile=@$filename \
        --form wpDestFile=$imagename \
        --form wpUpload="Upload File" \
        $::WIKIMAGE(url)/index.php/Special:Upload \
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

login 

upload

exit

# end
#####################################################################
