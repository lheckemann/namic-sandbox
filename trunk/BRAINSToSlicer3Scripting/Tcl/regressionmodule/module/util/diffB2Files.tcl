proc diffB2Files {} {


    set fName [grep "FILENAME:" $originalFile | awk '{print $2'}]
    set date [grep "DATE:" $originalFile | awk '{print $2'}]
    set name [grep "NAME:" $originalFile | awk '{print $2'}]
    set version [grep "VERSION:" $originalFile | awk '{print $2'}]
    set creator [grep "CREATOR:" $originalFile | awk '{print $2'}]
    set module [grep "MODULE:" $originalFile | awk '{print $2'}]
    set program [grep "PROGRAM:" $originalFile | awk '{print $2'}]
    set patId [grep "PATIENT_ID:" $originalFile | awk '{print $2'}]
    set patId [grep "SCAN_ID:" $originalFile | awk '{print $2'}]

    set dataLine [grep -n "IPL_HEADER_END" $originalFile | awk -F ":" 'print $1']

    set origDataFile [filename $originalFile]/diff/origData
    set origHdrFile [filename $originalFile]/diff/origHdr
    set newDataFile [filename $originalFile]/diff/newData
    set newHdrFile [filename $originalFile]/diff/newHdr

    exec sed 1,${dataLine} $originalFile > $origHdrFile
    exec sed [expr ${dataLine} + 1]~1 $originalFile > $origDataFile

    set dataDiff [exec diff $origDataFile $newDataFile]
    set hdrDiff [exec diff $origHdrFile $newHdrFile]

}

proc getBrains2FileInfo {} {

    set b2FileInfo(FILENAME)   [grep "FILENAME:" $originalFile | awk '{print \$2}']
    set b2FileInfo(DATE)     [grep "DATE:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(NAME)     [grep "NAME:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(VERSION)   [grep "VERSION:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(CREATOR)   [grep "CREATOR:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(MODULE)     [grep "MODULE:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(PROGRAM)   [grep "PROGRAM:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(PATID)     [grep "PATIENT_ID:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(SCANID)     [grep "SCAN_ID:" $originalFile | awk '{print \$2'}]
    set b2FileInfo(ENDLINE)   [grep -n "IPL_HEADER_END" $originalFile | awk -F ":" '{print $1}']

    return $b2FileInfo

}


patchBrains2FileInfo {brains2FileName originalFileInfo} {
    $sed -e "s/^BYTE_ORDER: .*/BYTE_ORDER: $patchOrder/" $1 > /tmp/${1:t}.tmp

}
