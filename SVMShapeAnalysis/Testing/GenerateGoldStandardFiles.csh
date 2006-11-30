#! /bin/tcsh -f

#
# This file generates the gold-standard output files that are used for
# comparison during testing.  
#

#
# setup paths
#
setenv ITKSVM_BIN_DIR  /usr/local/dev/MITGollandDist-Build/bin
setenv ITKSVM_DATA_DIR /usr/local/dev/MITGollandDist/Data
setenv ITKSVM_TESTING_DATA_DIR /tmp/testing

#
# variables
#
setenv JACKKNIFE_ITERATIONS 100
setenv JACKKNIFE_TEST_SIZE  20
setenv RBF_KERNEL_PARAMETERS 0.0001
setenv POLYNOMIAL_KERNEL_PARAMETERS '0.01 1.0 2.0'

setenv CLASS1_TRAINING '1_0[0123]?.mvh'
setenv CLASS2_TRAINING '7_0[0123]?.mvh'

setenv CLASS1_TESTING '1_1[0123]?.mvh'
setenv CLASS2_TESTING '7_1[0123]?.mvh'

#
# generate support vector machines
#

# linear
${ITKSVM_BIN_DIR}/svmTrain -v                                             \
--kernelType linear                                                       \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--svmOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_binary.svm     \
--svmOutputFormat binary

${ITKSVM_BIN_DIR}/svmTrain -v                                             \
--kernelType linear                                                       \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--svmOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_ascii.svm      \
--svmOutputFormat ascii

# polynomial
${ITKSVM_BIN_DIR}/svmTrain -v                                             \
--kernelType polynomial                                                   \
--kernelParameters ${POLYNOMIAL_KERNEL_PARAMETERS}                        \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--svmOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_binary.svm \
--svmOutputFormat binary

${ITKSVM_BIN_DIR}/svmTrain -v                                             \
--kernelType polynomial                                                   \
--kernelParameters ${POLYNOMIAL_KERNEL_PARAMETERS}                        \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--svmOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_ascii.svm  \
--svmOutputFormat ascii

# rbf
${ITKSVM_BIN_DIR}/svmTrain -v                                             \
--kernelType rbf                                                          \
--kernelParameters ${RBF_KERNEL_PARAMETERS}                               \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--svmOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_binary.svm        \
--svmOutputFormat binary

${ITKSVM_BIN_DIR}/svmTrain -v                                             \
--kernelType rbf                                                          \
--kernelParameters ${RBF_KERNEL_PARAMETERS}                               \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--svmOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_ascii.svm         \
--svmOutputFormat ascii

#
# classify test vectors
#

# linear
${ITKSVM_BIN_DIR}/svmClassify -v                                          \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_binary.svm           \
--vectorFilenames                                                         \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TESTING}       \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TESTING}       \
--labelOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_class.txt    

# polynomial
${ITKSVM_BIN_DIR}/svmClassify -v                                          \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_binary.svm       \
--vectorFilenames                                                         \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TESTING}       \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TESTING}       \
--labelOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_class.txt    

# rbf
${ITKSVM_BIN_DIR}/svmClassify -v                                          \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_binary.svm              \
--vectorFilenames                                                         \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TESTING}       \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TESTING}       \
--labelOutputFilename ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_class.txt    

#
# run jackknife estimation and write out misclassification histograms
#

# linear
${ITKSVM_BIN_DIR}/svmJackknife -v everything                              \
--kernelType linear                                                       \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--jackknifeIterations $JACKKNIFE_ITERATIONS                               \
--testSetSizes ${JACKKNIFE_TEST_SIZE}                                     \
--seedRandomOff                                                           \
|& tee ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_jackknife.txt    

cat ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_jackknife.txt |                \
sed -e '/\[.*\]/\!d' -e 's/\[//' -e 's/\]//' -e 's/,//g' > ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_hist.txt 

# polynomial
${ITKSVM_BIN_DIR}/svmJackknife -v everything                              \
--kernelType polynomial                                                   \
--kernelParameters ${POLYNOMIAL_KERNEL_PARAMETERS}                        \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--jackknifeIterations $JACKKNIFE_ITERATIONS                               \
--testSetSizes ${JACKKNIFE_TEST_SIZE}                                     \
--seedRandomOff                                                           \
|& tee ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_jackknife.txt

cat ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_jackknife.txt|             \
sed -e '/\[.*\]/\!d' -e 's/\[//' -e 's/\]//' -e 's/,//g' > ${ITKSVM_TESTING_DATA_DIR}/svmPolynomial01_hist.txt 

# rbf
${ITKSVM_BIN_DIR}/svmJackknife -v everything                              \
--kernelType rbf                                                          \
--kernelParameters ${RBF_KERNEL_PARAMETERS}                               \
--class1VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS1_TRAINING}      \
--class2VectorFilenames                                                   \
${ITKSVM_DATA_DIR}/UCI-ML-HandwritingDataset/META/${CLASS2_TRAINING}      \
--jackknifeIterations $JACKKNIFE_ITERATIONS                               \
--testSetSizes ${JACKKNIFE_TEST_SIZE}                                     \
--seedRandomOff                                                           \
|& tee ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_jackknife.txt    

cat ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_jackknife.txt|                    \
sed -e '/\[.*\]/\!d' -e 's/\[//' -e 's/\]//' -e 's/,//g' > ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_hist.txt 

#
# generate discriminative directions
#

# linear
${ITKSVM_BIN_DIR}/svmDDCalculator -v                                      \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_binary.svm           \
--outputFilenamePrefix ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_DD_binary_  \
--vectorOutputFormat binary                              

${ITKSVM_BIN_DIR}/svmDDCalculator -v                                      \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_binary.svm           \
--outputFilenamePrefix ${ITKSVM_TESTING_DATA_DIR}/svmLinear01_DD_ascii_   \
--vectorOutputFormat ascii

# rbf
${ITKSVM_BIN_DIR}/svmDDCalculator -v                                      \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_binary.svm              \
--outputFilenamePrefix ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_DD_binary_     \
--vectorOutputFormat binary                              

${ITKSVM_BIN_DIR}/svmDDCalculator -v                                      \
--svmFilename ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_binary.svm              \
--outputFilenamePrefix ${ITKSVM_TESTING_DATA_DIR}/svmRBF01_DD_ascii_      \
--vectorOutputFormat ascii

