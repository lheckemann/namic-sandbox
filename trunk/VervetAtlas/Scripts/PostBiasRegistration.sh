#!/bin/bash

#if [ $# -ne 1 ]
#then
#  echo "Usage: `basename $0` directory_with_cases"
#  exit 1
#fi

#CASES=$1
SLICER=/workspace/fedorov/Slicer/Release/Slicer3-build/Slicer3
N3=/projects/birn/fedorov/Slicer3fedorov/10579/N3MRILightCLI/N3MRILightCLI
BRAINSFIT=/projects/birn/fedorov/Slicer3fedorov/10578/BRAINSFit/BRAINSFit

for CASE in  Calvin Gucci Hugo Tommy Ralph Valentino Marc Issac Louis Oscar
do
  echo "Performing Initial registration for $CASE"

  FIXED_IMAGE=Tommy/Tommy-N3.nrrd
  MOVING_IMAGE=${CASE}/${CASE}-N3.nrrd
  OUTPUT_XFORM=${CASE}/${CASE}-PostBias.mat
  FIXED_MASK=Tommy/Skullstrip/Tommy-ICC-3dilations.nrrd
  MOVING_MASK=${CASE}/${CASE}-ICC.nrrd


#  CMD="${SLICER} --launch $BRAINSFIT --fixedVolume $FIXED_IMAGE --movingVolume $MOVING_IMAGE --transformType Rigid,ScaleVersor3D,ScaleSkewVersor3D,Affine,BSpline --initializeTransformMode CenterOfHead --maskInferiorCutOffFromCenter 1000 --numberOfIterations 1500 --numberOfSamples 100000 --minimumStepSize 0.005 --transformScale 1000 --reproportionScale 1 --skewScale 1 --splineGridSize 14,10,12 --outputTransform $OUTPUT_XFORM --outputVolumePixelType float --backgroundFillValue 0 --maskProcessingMode ROI --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0 --medianFilterSize 0,0,0 --numberOfHistogramBins 50 --numberOfMatchPoints 10 --patientID ANON --studyID ANON --fixedVolumeOrigin 0,0,0 --movingVolumeOrigin 0,0,0 --useCachingOfBSplineWeightsMode ON --useExplicitPDFDerivativesMode AUTO --relaxationFactor 0.5 --maximumStepSize 0.2 --failureExitCode -1 --debugNumberOfThreads 16 --fixedBinaryVolume ${FIXED_MASK} --movingBinaryVolume ${MOVING_MASK}"

  CMD="${SLICER} --launch $BRAINSFIT --fixedVolume $FIXED_IMAGE --movingVolume $MOVING_IMAGE --transformType Rigid,ScaleVersor3D,ScaleSkewVersor3D,Affine,BSpline --initializeTransformMode CenterOfHead --maskInferiorCutOffFromCenter 1000 --numberOfIterations 1500 --numberOfSamples 100000 --minimumStepSize 0.005 --transformScale 1000 --reproportionScale 1 --skewScale 1 --splineGridSize 14,10,12 --outputTransform $OUTPUT_XFORM --outputVolumePixelType float --backgroundFillValue 0 --maskProcessingMode ROIAUTO --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0 --medianFilterSize 0,0,0 --numberOfHistogramBins 50 --numberOfMatchPoints 10 --patientID ANON --studyID ANON --fixedVolumeOrigin 0,0,0 --movingVolumeOrigin 0,0,0 --useCachingOfBSplineWeightsMode ON --useExplicitPDFDerivativesMode AUTO --relaxationFactor 0.5 --maximumStepSize 0.2 --failureExitCode -1 --debugNumberOfThreads 16"
  #--fixedBinaryVolume ${FIXED_MASK} --movingBinaryVolume ${MOVING_MASK}"

  echo $CMD
  ${CMD}

  echo "Initial registration is complete for case ${CASE}"

done
