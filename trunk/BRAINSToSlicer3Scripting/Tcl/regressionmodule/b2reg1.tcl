# \author    Hans J. Johnson
# \date     $Date: 2005-07-27 20:47:33 -0500 (Wed, 27 Jul 2005) $
# \brief    This file contains the outline that actually runs each
#           of the modular tests
# \fn        proc b2reg1.tcl 

#######################################################################
# Include the utilties required for running regression tests
puts "source module/util/DateStamp.tcl"
source module/util/DateStamp.tcl
puts "source module/ReportTestStatus.tcl"
source module/util/ReportTestStatus.tcl
puts "source module/ModuleUtils.tcl"
source module/util/ModuleUtils.tcl

#######################################################################
# Preliminary setup
set dateString [DateStamp]
set pathToRegressionDir ../regressiontest

#######################################################################
#List module names that are to be run
#Ording of this list determines order of processing
set passedlist "
"

set nobatch "
createRoi
getMaskColor
getRoiColor
getSetLocation
interactiveModes
interactiveShowHide
setInterpolation
setPixmapDirectory
"

#OUTDATED TEST applyROIdelta
set moduleslist "
saveTalairachBox
saveTissueClass
loadImageAnalyzeFileTypes
loadImageAnalyzeFormats
loadImageAnalyzeT1Strict
loadImageAnalyzeT1
loadImageAnalyzeT2
loadImageAnalyzeTissueClassified
loadImage-PetFlow
loadHistogram
loadImage-GE4X
loadImage-GE5X
loadImage-GEADW
loadImage-GEDICOM
loadImageTrimodal
loadImageVarianEPI
loadImageVarianT1
loadImageVarianT2FSE
loadLandmarks
loadPalette
loadRoi
loadSurface
loadTable
loadTalairachBox
loadTalairachParameters
loadTissueClass
loadTransform
loadImageBrainsResampled
itkFilters
averageLandmarks
booleanMasks
checkCloseROI
cleanMask
clipImage
codeImageAndMasks
computeWbbf
convertClassImageToDiscrete
convertLandmarksToLandmarks
convertMaskToRoi
convertRGBImageToHSI
convertRoiToMask
convertTalairachMask
convertTalairachParameters
createBoundedImage
createBoxedImage
createBulletImage
createEnhancedImages
createHemRoi
createHistogram
createROIHull
createTableMask
createWitelsonRois
divideImages
equalImages
erodeSurfaceMask
extrudeROI
fillBrainSegMask
fitInterleaved
getDimsResImage
getDimsResMask
getDimsResRoi
getDimsResTalairach
getDimsResTransform
getHistogramData
getId
getLandmarkLocation
getLandmarkNames
getName
getPaletteData
getPaletteType
getSetTalairach
getSetThreshold
getSetTransform
getTableData
getTissueClassKappa
getTransformType
maxImages
measureClassVolume
measureImageVolume
measureStudentT
measureVolume
minImages
multiplyImages
normalizePet
resampleImage
resampleMask
resampleWorsleyMR
saveHistogram
saveLandmark
saveMask
savePalette
saveRoi
saveSurface
saveTable
saveTalairachBox
saveTalairachParameters
saveTissueClass
setDebugLevel
setInterpolation
setPixmapDirectory
setStandardSize
subtractConstantImages
subtractImages
sumImages
talairachBoxPartition
thinROI
thresholdImage
worsleyAverageMR
saveImage
saveTransform
"
set done "
"


#######################################################################
#Actually run through all the test that you can
set lenmoduleslist [llength  $moduleslist]
for {set currmodule 0} { $currmodule < $lenmoduleslist } {incr currmodule} {
  set ModuleName [lindex $moduleslist $currmodule]
  if { [ lsearch $done $ModuleName ] == -1 } {
      puts "Loading module($currmodule) $ModuleName.tcl"

      source module/$ModuleName.tcl
      #Actually run the command
      catch [ $ModuleName $pathToRegressionDir $dateString ]
  } else {
      puts "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ The module already past module($currmodule) $ModuleName.tcl"
  }
}
