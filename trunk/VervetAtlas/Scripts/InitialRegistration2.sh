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

for CASE in  Hugo Valentino
do
  echo "Performing Initial registration for $CASE"

  FIXED_IMAGE=Tommy/Tommy-GAD.nrrd
  MOVING_IMAGE=${CASE}/${CASE}-GAD-Untilted.nrrd
  OUTPUT_XFORM=${CASE}/${CASE}-initial_affine2.mat

  ${SLICER} --launch $BRAINSFIT --fixedVolume $FIXED_IMAGE --movingVolume $MOVING_IMAGE --transformType Rigid,ScaleVersor3D,Affine --initializeTransformMode CenterOfHead --maskInferiorCutOffFromCenter 1000 --numberOfIterations 1500 --numberOfSamples 100000 --minimumStepSize 0.005 --transformScale 1000 --reproportionScale 1 --skewScale 1 --splineGridSize 14,10,12 --outputTransform $OUTPUT_XFORM --outputVolumePixelType float --backgroundFillValue 0 --maskProcessingMode ROIAUTO --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0 --medianFilterSize 0,0,0 --numberOfHistogramBins 50 --numberOfMatchPoints 10 --patientID ANON --studyID ANON --fixedVolumeOrigin 0,0,0 --movingVolumeOrigin 0,0,0 --useCachingOfBSplineWeightsMode ON --useExplicitPDFDerivativesMode AUTO --relaxationFactor 0.5 --maximumStepSize 0.2 --failureExitCode -1 --debugNumberOfThreads 16 

  echo "Initial registration is complete for case ${CASE}"

done
