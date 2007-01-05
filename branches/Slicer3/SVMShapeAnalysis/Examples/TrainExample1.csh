#! /bin/tcsh -f

#
# this example shows how to train a support vector machine from
# labeled training vectors
#

#
# read enviornment variables 
#
source SetExampleEnviornmentVariables.csh

#
# run svmTrain
#
${ITKSVM_BIN_DIR}/svmTrain -v                                 \
--kernelType linear                                           \
--class1VectorFilenames                                       \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/1_0??.mvh   \
--class2VectorFilenames                                       \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/7_0??.mvh   \
--svmOutputFilename ${ITKSVM_OUTPUT_DIR}/svm.svm              \
--svmOutputFormat ascii
