source SetupPaths.sh
RUN_NAME=BrainTutorial-SetTarget1

$SLICER_BIN_DIR/EMSegmentCommandLine\
 --verbose\
 --mrmlSceneFileName $EMSEG_TUTORIAL_DIR/EMSeg-Brain-MRT1T2_small.mrml\
 --resultVolumeFileName VolumeData_Output/Segmentation-${RUN_NAME}_small.nhdr\
 --targetVolumeFileNames\
   $EMSEG_TUTORIAL_DIR/VolumeData_Input/Target_NewPatient1/spgr_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Target_NewPatient1/t2w_small.nhdr



