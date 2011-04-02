REM Remember the home directory to return there after script completion
set HOME_DIR=%CD%

chdir /d %RESULT_DATA_DIR%

REM Compute average ground truth deformation
%COMP_MULTI_IMAGE_STAT_EXE% --fileList=%HOME_DIR%\FileList_DeformImage-B2_IntraOpDefField.txt --meanImage=IntraOpDefFieldMean.mha --maxImage=IntraOpDefFieldMax.mha --vectorInput --meanHistogram=IntraOpDefFieldMeanHistogram.csv --maxHistogram=IntraOpDefFieldMaxHistogram.csv --histogramBinCount=101 --histogramMin=0.0 --histogramMax=10.0

REM Compute average TRE for rigid registration result, with full volume ROI
%COMP_MULTI_IMAGE_STAT_EXE% --fileList=%HOME_DIR%\FileList_RigidFieldDiffMag_RoiFullVolume.txt --meanImage=RigidTreMean_RoiFullVolume.mha --maxImage=RigidTreMax_RoiFullVolume.mha --meanHistogram=RigidTreMeanHistogram_RoiFullVolume.csv --maxHistogram=RigidTreMaxHistogram_RoiFullVolume.csv --histogramBinCount=101 --histogramMin=0.0 --histogramMax=10.0
REM Compute average TRE for deformable registration result, with full volume ROI
%COMP_MULTI_IMAGE_STAT_EXE% --fileList=%HOME_DIR%\FileList_DefFieldDiffMag_RoiFullVolume.txt --meanImage=DefTreMean_RoiFullVolume.mha --maxImage=DefTreMax_RoiFullVolume.mha --meanHistogram=DefTreMeanHistogram_RoiFullVolume.csv --maxHistogram=DefTreMaxHistogram_RoiFullVolume.csv --histogramBinCount=101 --histogramMin=0.0 --histogramMax=10.0

REM Compute average TRE for rigid registration result, with full volume ROI
%COMP_MULTI_IMAGE_STAT_EXE% --fileList=%HOME_DIR%\FileList_RigidFieldDiffMag_Roi3sPRPB.txt --meanImage=RigidTreMean_Roi3sPRPB.mha --maxImage=RigidTreMax_Roi3sPRPB.mha --meanHistogram=RigidTreMeanHistogram_Roi3sPRPB.csv --maxHistogram=RigidTreMaxHistogram_Roi3sPRPB.csv --histogramBinCount=101 --histogramMin=0.0 --histogramMax=10.0
REM Compute average TRE for deformable registration result, with full volume ROI
%COMP_MULTI_IMAGE_STAT_EXE% --fileList=%HOME_DIR%\FileList_DefFieldDiffMag_Roi3sPRPB.txt --meanImage=DefTreMean_Roi3sPRPB.mha --maxImage=DefTreMax_Roi3sPRPB.mha --meanHistogram=DefTreMeanHistogram_Roi3sPRPB.csv --maxHistogram=DefTreMaxHistogram_Roi3sPRPB.csv --histogramBinCount=101 --histogramMin=0.0 --histogramMax=10.0

:end
chdir /d %HOME_DIR%
