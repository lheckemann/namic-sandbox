#!/bin/bash

date >> timeLog

dataDir=./trainingShapes

resultDataDir=alignTrainingShapes

mkdir ${resultDataDir}

exeFile='./src/imageRegByPointSet/c/affine/r/imgRegByPtSet3DTest_uchar'

fixedImg=${dataDir}/02780302_label.nrrd


for j in ${dataDir}/*_label.nrrd; do
    fixedImgIso=${dataDir}/02780302_label-iso.nrrd

    unu resample -s = = x6.4 -i ${fixedImg} -o ${fixedImgIso}


    movingImgIso=`echo ${j} | sed s/.nrrd/-iso.nrrd/`

    unu resample -s = = x6.4 -i ${j} -o ${movingImgIso}

    regMovingImgIso=`echo ${movingImgIso} | sed s/-iso/-reg-iso/`

    ${exeFile} ${fixedImgIso} ${movingImgIso} 5000 0.1 ${regMovingImgIso}

    regMovingImg=`echo ${regMovingImgIso} | sed s/-iso//`

    unu resample -s = = 26 -i ${regMovingImgIso} -o ${regMovingImg}

    mv ${regMovingImg} ${resultDataDir}/

    rm ${movingImgIso}
done




##################################################
rm ${fixedImgIso}



date >> timeLog
