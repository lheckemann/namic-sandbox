#! /bin/tcsh -f

#
# this example shows how run jackknife estimation of classification error
#

#
# read enviornment variables 
#
source SetExampleEnviornmentVariables.csh

#
# run svmJackknife
#
${ITKSVM_BIN_DIR}/svmJackknife -v                              \
--kernelType linear                                            \
--class1VectorFilenames                                        \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/1_1??.mvh    \
--class2VectorFilenames                                        \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/7_1??.mvh    \
--jackknifeIterations 100                                      \
--testSetSizeStride 9                                          \
--errorOutputFilename ${ITKSVM_OUTPUT_DIR}/jackknifeErrors.txt
