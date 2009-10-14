#!/bin/bash

#if [ $# -ne 1 ]
#then
#  echo "Usage: `basename $0` directory_with_cases"
#  exit 1
#fi

#CASES=$1
SLICER=/workspace/fedorov/Slicer/Release/Slicer3-build/Slicer3
N3=/projects/birn/fedorov/Slicer3fedorov/10579/N3MRILightCLI/N3MRILightCLI

MOVING_IMAGE=./VPA-10-1.1/Vervet_T1_Template_WholeHead.nii
MOVING_IMAGE_ICC=./VPA-10-1.1/Vervet_T1_Template_ICC-mask.nrrd

for CASE in  Calvin Gucci Hugo Ralph Valentino Marc Issac Louis Oscar Tommy
do
  echo "Performing N3 correction for $CASE"

  INPUT_IMAGE=${CASE}/${CASE}-GAD.nrrd
#  INPUT_MASK=${CASE}/${CASE}-ICC.nrrd
  INPUT_MASK=${CASE}/${CASE}-ICC.nrrd
  OUTPUT_IMAGE1=${CASE}/${CASE}-N3.nrrd
  OUTPUT_IMAGE2=${CASE}/${CASE}-BiasField.nrrd

  CMD="${SLICER} --launch $N3 --shrinkfactor 4 --nfittinglevels 4 --niterations 100  --nhistogrambins 200 --weinerfilternoise 0.1 --biasfieldfullwidthathalfmaximum 0.15 --convergencethreshold 0.0001 --splineorder 3 --ncontrolpoints 4,4,4 ${INPUT_IMAGE} ${INPUT_MASK} ${OUTPUT_IMAGE1} ${OUTPUT_IMAGE2}"

  $CMD

  echo "N3 is complete for case ${CASE}"

done
