



#
# some handy info 
# http://curl.haxx.se/docs/httpscripting.html
#

# Note: the following syntax works to upload a file to the wiki if you copy the cookie information
# from firefox - not clear yet how to get curl to login itself and save the info

curl \
    --cookie NAMICNewWikiDBUserID=3 \
    --cookie NAMICNewWikiDBUserName=Pieper \
    --cookie NAMICNewWikiDBToken=99930421212693660455417595135040 \
    --cookie NAMICNewWikiDB_session=9d8f4623b11f1e5563550664d0780d48 \
    --form wpUploadFile=@c:/pieper/bwh/birn-grant-work/mbirn-sedona-2006/mbirn_sedona_MondayMorning.jpg \
    --form wpDestFile=mbirn_sedona_MondayMorning.jpg \
    --form wpUpload="Upload File" \
    http://www.na-mic.org/Wiki/index.php/Special:Upload \
    > c:/tmp/ff.html ; c:/Program\ Files/Mozilla\ Firefox/firefox.exe file://c:/tmp/ff.html
