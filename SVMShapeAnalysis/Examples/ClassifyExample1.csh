#! /bin/tcsh -f

#
# this example shows how to classify unlabeled vectors given a trained
# support vector machine
#

#
# read enviornment variables 
#
source SetExampleEnviornmentVariables.csh

#
# run svmClassify
#
${ITKSVM_BIN_DIR}/svmClassify -v                               \
--svmFilename ${ITKSVM_OUTPUT_DIR}/svm.svm                     \
--vectorFilenames                                              \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/1_10?.mvh    \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/7_10?.mvh    \
--labelOutputFilename ${ITKSVM_OUTPUT_DIR}/classifications.txt 
