source SetupPaths.sh
RUN_NAME=BrainTutorial-SetTargetAndAtlas

$SLICER_BIN_DIR/EMSegmentCommandLine\
 --verbose\
 --mrmlSceneFileName $EMSEG_TUTORIAL_DIR/EMSeg-Brain-MRT1T2_small.mrml\
 --resultVolumeFileName VolumeData_Output/Segmentation-${RUN_NAME}_small.nhdr\
 --targetVolumeFileNames\
   $EMSEG_TUTORIAL_DIR/VolumeData_Input/Target/t1_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Target/t2_small.nhdr\
 --atlasVolumeFileNames\
   $EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_background_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_background_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_csf_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_greymatter_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_whitematter_small.nhdr,$EMSEG_TUTORIAL_DIR/VolumeData_Input/Atlas/atlas_t1_small.nhdr


