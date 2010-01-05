#!/bin/bash

exeFile=./r/imagePCAAnalysisTest2
listFile=shapeList.txt

iii=0
while [ $iii -lt 6 ]; do
    echo ${iii}
    ${exeFile} ${listFile} ${iii}

    let iii=iii+1
done
