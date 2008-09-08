source SetupPaths.sh
RUN_NAME=BrainTutorial-Defaults

$SLICER_BIN_DIR/EMSegmentCommandLine\
 --verbose\
 --mrmlSceneFileName $EMSEG_TUTORIAL_DIR/EMSeg-Brain-MRT1T2_small.mrml\
 --resultVolumeFileName VolumeData_Output/Segmentation-${RUN_NAME}_small.nhdr


