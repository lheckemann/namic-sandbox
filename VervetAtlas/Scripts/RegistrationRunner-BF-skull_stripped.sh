#!/bin/bash

#if [ $# -ne 1 ]
#then
#  echo "Usage: `basename $0` directory_with_cases"
#  exit 1
#fi

#CASES=$1
SLICER=/workspace/fedorov/Slicer/Release/Slicer3-build/Slicer3

MOVING_IMAGE=./VPA-10-1.1/Vervet_T1_Template_WholeHead.nii
MOVING_IMAGE_ICC=./VPA-10-1.1/Vervet_T1_Template_ICC-mask.nrrd

for CASE in `ls Subjects`
do
  INPUT_DIR=Subjects/${CASE}/Target/Input
  FIXED_IMAGE=${INPUT_DIR}/${CASE}_HistMatch2Template.nii
  FIXED_IMAGE_ICC=${INPUT_DIR}/${CASE}_VT-ICC.nrrd
  BF_TRANSFORM_R1=${INPUT_DIR}/${CASE}_BF_transform_BSpline-VT-R1.tfm
  BF_TRANSFORM_R2=${INPUT_DIR}/${CASE}_BF_transform_BSpline-VT-R2.tfm
  BF_LOG=${INPUT_DIR}/${CASE}_BF_output_BSpline-VT.log
  OUTPUT_VOL=${INPUT_DIR}/Atlas2${CASE}_BF_warped_BSpline-ICC-VT.nii

  # 1. register atlas to subject

  date > ${BF_LOG}
  echo "Beginning registration 1"
  ${SLICER} --launch /tmp/10411/BRAINSFitVT/BRAINSFitVT \
  --fixedVolume ${FIXED_IMAGE}  --movingVolume ${MOVING_IMAGE} \
  --transformType Rigid,ScaleVersor3D,ScaleSkewVersor3D,Affine,BSpline --initializeTransformMode CenterOfHead \
  --numberOfIterations 1000 --numberOfSamples 100000 --minimumStepSize 0.005 \
  --spatialScale 1000 --reproportionScale 1 \
  --outputTransform ${BF_TRANSFORM_R1} \
  --patientID ANON --studyID ANON --outputVolumePixelType float \
  --backgroundFillValue 0 --maskProcessingMode ROIAUTO \
  --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0 \
  --fixedVolumeOrigin 0,0,0 --movingVolumeOrigin 0,0,0 \
  --medianFilterSize 0,0,0 --permitParameterVariation 1,1,1 \
  --useCachingOfBSplineWeightsMode ON --useExplicitPDFDerivativesMode Whatever \
  --relaxationFactor 0.5 --maximumStepSize 0.2 --failureExitCode -1 \
  --numberOfHistogramBins 50 --numberOfMatchPoints 10 --debugNumberOfThreads 8 \
  --outputVolume ${OUTPUT_VOL}
  echo "Registration 1 is complete for case ${CASE}"

#  --outputFixedVolumeROI ${FIXED_IMAGE_ROI} --outputMovingVolumeROI ${MOVING_IMAGE_ROI} \
  # 2. resample atlas icc with the derived transform
  ${SLICER} --launch ResampleVolume2 --Reference ${FIXED_IMAGE} --number_of_thread 0 \
    --transformationFile ${BF_TRANSFORM_R1} \
    --interpolation nn --default_pixel_value 0 \
    ${MOVING_IMAGE_ICC} ${FIXED_IMAGE_ICC}

  echo "ICC resampling is complete for case ${CASE}"

  # 3. run registration with the same parameters, but this time using derived
  # ICC masks

  date >> ${BF_LOG}
  ${SLICER} --launch /tmp/10411/BRAINSFitVT/BRAINSFitVT \
  --fixedVolume ${FIXED_IMAGE}  --movingVolume ${MOVING_IMAGE} \
  --transformType Rigid,ScaleVersor3D,ScaleSkewVersor3D,Affine,BSpline --initializeTransformMode CenterOfHead \
  --numberOfIterations 1000 --numberOfSamples 100000 --minimumStepSize 0.005 \
  --spatialScale 1000 --reproportionScale 1 \
  --outputTransform ${BF_TRANSFORM_R2} \
  --patientID ANON --studyID ANON --outputVolumePixelType float \
  --backgroundFillValue 0 --maskProcessingMode ROI \
  --fixedBinaryVolume ${FIXED_IMAGE_ICC} --movingBinaryVolume ${MOVING_IMAGE_ICC} \
  --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0 \
  --fixedVolumeOrigin 0,0,0 --movingVolumeOrigin 0,0,0 \
  --medianFilterSize 0,0,0 --permitParameterVariation 1,1,1 \
  --useCachingOfBSplineWeightsMode ON --useExplicitPDFDerivativesMode Whatever \
  --relaxationFactor 0.5 --maximumStepSize 0.2 --failureExitCode -1 \
  --numberOfHistogramBins 50 --numberOfMatchPoints 10 --debugNumberOfThreads 8 \
  --outputVolume ${OUTPUT_VOL} 

#  --outputFixedVolumeROI ${FIXED_IMAGE_ROI} --outputMovingVolumeROI ${MOVING_IMAGE_ROI} \
  echo "Registration 2 is complete for case ${CASE}"

done
