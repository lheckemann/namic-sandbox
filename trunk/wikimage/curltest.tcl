#!/bin/sh
# \
exec tclsh "$0" ${1+"$@"}

#
#
# This is a curl test for editing a wiki page
# -- warning: some is fragile if mediawiki changes.
#    While the parsing is simple and fragile (and maybe hackish),
#    in fact any screenscraping like this will only work as long
#    as the form names and semantics stay the same.
#


set WIKIMAGE(url) http://wiki.na-mic.org

set WIKIMAGE(gallery) Slicer3:VisualBlog

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


#
# submit the new text, but put it through stdin of curl so we don't need
# to worry about shell quoting
#
set fp [open "| curl --silent --cookie mycookies -F wpTextbox1=<- -F wpSave=Save%20page -F wpEdittime=$Edittime -F wpEditToken=$EditToken --url $::WIKIMAGE(url)/Wiki/index.php?title=$::WIKIMAGE(gallery)\\&action=submit" "w"]

puts $fp $pagetext

close $fp

exit 0
