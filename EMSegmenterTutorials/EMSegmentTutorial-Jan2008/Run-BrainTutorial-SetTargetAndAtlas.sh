source SetupPaths.sh
RUN_NAME=BrainTutorial-SetTargetAndAtlas

$SLICER_BIN_DIR/EMSegmentCommandLine\
 --verbose\
 --mrmlSceneFileName $EMSEG_TUTORIAL_DIR/EMSeg-Brain-MRT1T2.mrml\
 --intermediateResultsDirectory $EMSEG_TUTORIAL_DIR/Working/$RUN_NAME\
 --resultVolumeFileName VolumeData_Output/Segmentation-$RUN_NAME.nhdr\
 --targetVolumeFileNames\
   $EMSEG_TUTORIAL_DIR/VolumeData_Input/Target/t1.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Target/t2.nhdr\
 --atlasVolumeFileNames\
   $EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_background.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_background.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_csf.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_greymatter.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_whitematter.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_t1.nhdr


