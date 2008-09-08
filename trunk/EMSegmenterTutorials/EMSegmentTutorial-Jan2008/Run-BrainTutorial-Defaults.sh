source SetupPaths.sh
RUN_NAME=BrainTutorial-Defaults

$SLICER_BIN_DIR/EMSegmentCommandLine\
 --verbose\
 --mrmlSceneFileName $EMSEG_TUTORIAL_DIR/EMSeg-Brain-MRT1T2.mrml\
 --intermediateResultsDirectory $EMSEG_TUTORIAL_DIR/Working/$RUN_NAME\
 --resultVolumeFileName VolumeData_Output/Segmentation-$RUN_NAME.nhdr\


