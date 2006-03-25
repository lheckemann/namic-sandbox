#!/bin/sh
# \
exec wish "$0" ${1+"$@"}



#
# some handy info 
# http://curl.haxx.se/docs/httpscripting.html
#

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

# put up a simple password prompt dialog box
proc get_password {} {

    if { [catch "package require Tk"] } {
        puts -nonewline "password: " ; flush stdout
        gets stdin pw
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

login_test


exit
