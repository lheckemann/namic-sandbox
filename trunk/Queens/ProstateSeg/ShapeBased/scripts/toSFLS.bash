#!/bin/bash

exeFile=bin/ConvertImageToLevelSet/ConvertImageToLevelSet

outputDir=output/LevelSetRepresentation
mkdir -p $outputDir

for j in output/alignTrainingShapes/*-reg.nrrd; do
    echo "Processing $j"
    outputName=`echo ${j} | sed s/.nrrd/-SFLS.nrrd/`
    ${exeFile} $j ${outputName}
    mv ${outputName} $outputDir
done
