#!/bin/bash

exeFile=./r/imagePCAAnalysisTest2
listFile=lalist.txt

iii=0
while [ $iii -lt 12 ]; do
    echo ${iii}
    ${exeFile} ${listFile} ${iii}

    let iii=iii+1
done
