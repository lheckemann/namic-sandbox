#!/bin/bash

dataDir=./trainingShapes

resultDataDir=alignTrainingShapes

mkdir ${resultDataDir}

exeFile='./src/imageRegByPointSet/c/affine/r/imgRegByPtSet3DTest_uchar'

fixedImg=${dataDir}/02780302_label.nrrd


for j in ${dataDir}/*.nrrd; do
    regedMovImg=`echo ${j} | sed s/.nrrd/-reg.nrrd/`

#    echo ${exeFile} ${fixedImg} ${j} 5000 0.5 ${regedMovImg}
#    echo mv ${regedMovImg} ${resultDataDir}/
    ${exeFile} ${fixedImg} ${j} 5000 0.5 ${regedMovImg}
    mv ${regedMovImg} ${resultDataDir}/
done
