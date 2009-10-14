#!/bin/bash

#if [ $# -ne 1 ]
#then
#  echo "Usage: `basename $0` directory_with_cases"
#  exit 1
#fi

#CASES=$1
SLICER=/workspace/fedorov/Slicer/Release/Slicer3-build/Slicer3

FIXED_IMAGE=Tommy/Tommy-GAD.nrrd

#for CASE in  Calvin Gucci Ralph Marc Issac Louis Oscar # + Hugo Valentino
for CASE in  Calvin Gucci Hugo Issac
do
  MOVING_IMAGE=${CASE}/${CASE}-GAD.nrrd
#  XFM=${CASE}/${CASE}-initial_affine-inv.tfm
  XFM=${CASE}/${CASE}-PostBias.mat
  REGISTERED_IMAGE=${CASE}/${CASE}-BSpline-NotMasked.nrrd

  ${SLICER} --launch ResampleVolume2 --Reference ${FIXED_IMAGE} --number_of_thread 0 \
    --transformationFile ${XFM} \
    --interpolation linear --default_pixel_value 0 \
    ${MOVING_IMAGE} ${REGISTERED_IMAGE}

  echo "Resampling for ${CASE} is complete"
done
