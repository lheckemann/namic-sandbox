#! /bin/tcsh -f

#
# this example shows how to compute the discriminative directions of a
# trained SVM classifier at the support vectors
#

#
# read enviornment variables 
#
source SetExampleEnviornmentVariables.csh

#
# run svmDDCalculator
#
${ITKSVM_BIN_DIR}/svmDDCalculator -v                \
--svmFilename ${ITKSVM_OUTPUT_DIR}/svm.svm          \
--outputFilenamePrefix ${ITKSVM_OUTPUT_DIR}/DD_     \
--vectorOutputFormat ascii
