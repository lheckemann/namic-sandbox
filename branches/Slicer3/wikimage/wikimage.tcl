#!/bin/sh
# \
exec wish "$0" ${1+"$@"}

#####################################################################
# arg parsing

proc Usage { {msg ""} } {
    global SLICER
    
    set msg "$msg\nusage: wikimage \[options\] <image_filename> <caption>"
    set msg "$msg\n  <image_filename> is the file to upload"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   --gallery : wiki page where image link should be added"
    set msg "$msg\n   --thumb-size : size of gallery thumbnail (default 200px)"
    set msg "$msg\n   -u --user : wiki user name (default is env USER)"
    set msg "$msg\n   -p --password : wiki login password"
    set msg "$msg\n   --url : url of the wiki (default wiki.na-mic.org)"
    set msg "$msg\n   -c --cookie-jar : file to save your cookies (default 'mycookies')"
    set msg "$msg\n   --help : prints this message and exits"
    puts stderr $msg
}

set ::WIKIMAGE(gallery) "Slicer3:VisualBlog"
set ::WIKIMAGE(thumb-size) ""
set ::WIKIMAGE(url) "http://wiki.na-mic.org"
set ::WIKIMAGE(user) $::env(USER)
set ::WIKIMAGE(password) ""
set ::WIKIMAGE(cookie-jar) "mycookies"
set ::WIKIMAGE(caption) "No caption"

set strippedargs ""
set argc [llength $argv]

for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--gallery" {
            incr i ;# skip this argument
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set ::WIKIMAGE(gallery) [lindex $argv $i]
            }
        }
        "--thumb-size" {
            incr i ;# skip this argument
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set ::WIKIMAGE(thumb-size) [lindex $argv $i]
            }
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

set ::WIKIMAGE(filename) [lindex $argv 0]
set ::WIKIMAGE(imagename) [file tail $::WIKIMAGE(filename)]
if { $argc > 1 } {
  set ::WIKIMAGE(caption) [lrange $argv 1 end]
}

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

    if { [string first "No file by this name exists" $res] == -1 } {
      return 0
    } else {
      return 1
    }
}

proc upload {} {

    catch "exec curl \
        --basic \
        --url $::WIKIMAGE(url)/index.php/Special:Upload \
        --cookie $::WIKIMAGE(cookie-jar) \
        --form wpUploadFile=@$::WIKIMAGE(filename) \
        --form wpDestFile=$::WIKIMAGE(imagename) \
        --form wpUpload=\"Upload File\" \
        --form wpUploadDescription=\"$::WIKIMAGE(caption)\" "
}

proc add_gallery_link {} {

  set ret [catch "exec curl --url $::WIKIMAGE(url)/$::WIKIMAGE(gallery)&action=edit" res]


  #
  # get the current page contents
  #
  set ret [catch "exec curl --cookie mycookies --url $::WIKIMAGE(url)/Wiki/index.php?title=$::WIKIMAGE(gallery)&action=edit" res]

  #
  # find the time and the token
  #
  set fingerend [string first "name=\"wpEdittime\"" $res]
  set fingerstart [string last "value" $res $fingerend]
  set valuetext [string range $res $fingerstart $fingerend]
  set fingerstart [expr [string first "\"" $valuetext] + 1]
  set fingerend [expr [string last "\"" $valuetext] - 1]
  set Edittime [string range $valuetext $fingerstart $fingerend]
  puts "edittime is $Edittime"

  set fingerend [string first "name=\"wpEditToken\"" $res]
  set fingerstart [string last "value" $res $fingerend]
  set valuetext [string range $res $fingerstart $fingerend]
  set fingerstart [expr [string first "\"" $valuetext] + 1]
  set fingerend [expr [string last "\"" $valuetext] - 1]
  set EditToken [string range $valuetext $fingerstart $fingerend]
  puts "EditToken is $EditToken"

  #
  # find the actual text on the page
  #
  set finger [string first "<textarea" $res]
  set pagetext [string range $res $finger end]
  set finger [string first ">" $pagetext]
  set pagetext [string range $pagetext [expr 1+$finger] end]
  set finger [string first "</textarea>" $pagetext]
  set pagetext [string range $pagetext 0 [expr $finger-1] ]


  set uploadtime [clock format [clock seconds] -format "%A, %B %d, %Y at %l:%M%p"]

  set imagetext "=== Image from $::env(USER) on $uploadtime ===\n"
  append imagetext "\[\[image:$::WIKIMAGE(imagename) | thumb | 200px\]\]\n"
  append imagetext "$::WIKIMAGE(caption) \n"

  set pagetext "$imagetext\n$pagetext"

  #
  # submit the new text, but put it through stdin of curl so we don't need
  # to worry about shell quoting
  #
  set fp [open "| curl --silent --cookie mycookies -F wpTextbox1=<- -F wpSave=Save%20page -F wpEdittime=$Edittime -F wpEditToken=$EditToken --url $::WIKIMAGE(url)/Wiki/index.php?title=$::WIKIMAGE(gallery)\\&action=submit" "w"]

  puts $fp $pagetext

  close $fp
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

add_gallery_link

exit

# end
#####################################################################
