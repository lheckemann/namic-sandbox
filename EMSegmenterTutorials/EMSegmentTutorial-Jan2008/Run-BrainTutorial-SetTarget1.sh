source SetupPaths.sh
RUN_NAME=BrainTutorial-SetTarget1

$SLICER_BIN_DIR/EMSegmentCommandLine\
 --verbose\
 --mrmlSceneFileName $EMSEG_TUTORIAL_DIR/EMSeg-Brain-MRT1T2.mrml\
 --intermediateResultsDirectory $EMSEG_TUTORIAL_DIR/Working/$RUN_NAME\
 --resultVolumeFileName VolumeData_Output/Segmentation-$RUN_NAME.nhdr\
 --targetVolumeFileNames\
   $EMSEG_TUTORIAL_DIR/VolumeData_Input/Target_NewPatient1/spgr.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Target_NewPatient1/t2w.nhdr



