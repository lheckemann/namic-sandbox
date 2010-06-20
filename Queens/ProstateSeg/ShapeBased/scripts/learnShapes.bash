#!/bin/bash

SLICER=/d/scratchbuild/fedorov/Slicer3-3.6-2010-06-10-linux-x86_64/Slicer3
#exeFile=./r/imagePCAAnalysisTest2
exeFile=bin/LearnShapeModel/LearnShapeModel
#listFile=shapeList.txt

outputDir=output/ShapeLearning
listFile=${outputDir}/shapeList.txt

mkdir -p $outputDir

find . | grep output | grep LevelSetRepresentation | grep nrrd > $listFile

iii=0
N=2     # originally 6
while [ $iii -lt $N ]; do
    echo ${iii}
    ${exeFile} ${listFile} ${iii}
    mv eigenImg_${iii}.nrrd $outputDir
    let iii=iii+1
done

mv meanImage.nrrd $outputDir
mv eigenValueFile.txt $outputDir

# flip z-dir

$SLICER --launch unu flip -a 2 -i $outputDir/meanImage.nrrd -o ${outputDir}/meanImage.nrrd

iii=0
while [ $iii -lt $N ]; do
    eigenName='eigenImg_'$iii'.nrrd'
    $SLICER --launch unu flip -a 2 -i ${outputDir}/${eigenName} -o ${outputDir}/${eigenName}

    let iii=iii+1
done
