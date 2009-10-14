#!/bin/bash

#if [ $# -ne 1 ]
#then
#  echo "Usage: `basename $0` directory_with_cases"
#  exit 1
#fi

#CASES=$1
SLICER=/workspace/fedorov/Slicer/Release/Slicer3-build/Slicer3

MOVING_IMAGE=Tommy/Skullstrip/Tommy-ICC-3dilations.nrrd

#for CASE in  Calvin Gucci Ralph Marc Issac Louis Oscar # + Hugo Valentino
for CASE in  Hugo Valentino
do
  FIXED_IMAGE=${CASE}/${CASE}-GAD.nrrd
#  XFM=${CASE}/${CASE}-initial_affine-inv.tfm
  XFM=${CASE}/${CASE}-initial_affine2-inv.tfm
  REGISTERED_IMAGE=${CASE}/${CASE}-ICC.nrrd

  ${SLICER} --launch ResampleVolume2 --Reference ${FIXED_IMAGE} --number_of_thread 0 \
    --transformationFile ${XFM} \
    --interpolation nn --default_pixel_value 0 \
    ${MOVING_IMAGE} ${REGISTERED_IMAGE}

  echo "Resampling for ${CASE} is complete"
done
