#!/bin/bash

exeFile=./r/imagePCAAnalysisTest2
listFile=shapeList.txt

iii=0
N=6
while [ $iii -lt $N ]; do
    echo ${iii}
    ${exeFile} ${listFile} ${iii}

    let iii=iii+1
done



# flip z-dir

unu flip -a 2 -i meanImage.nrrd -o meanImage.nrrd

iii=0
while [ $iii -lt $N ]; do
    eigenName='eigenImg_'$iii'.nrrd'
    unu flip -a 2 -i ${eigenName} -o ${eigenName}

    let iii=iii+1
done
