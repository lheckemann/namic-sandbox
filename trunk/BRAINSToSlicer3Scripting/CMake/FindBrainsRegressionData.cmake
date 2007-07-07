#
# BrainsRegressionData_DATA_DIR   - Directories to include to use GTK
# BrainsRegressionData_FOUND         - If false, don't try to use GTK

# don't even bother under WIN32
IF(UNIX)

ENDIF(UNIX)

  FIND_PATH( BrainsRegressionData_DATA_PATH README.txt
    ${BRAINS_Complete_SOURCE_DIR}/regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../../regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../../../regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../../../../regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../../../../../regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../../../../../../regressiontest
    ${BRAINS_Complete_SOURCE_DIR}/../../../../../../../regressiontest
  )


  IF(EXISTS ${BrainsRegressionData_DATA_PATH})
    SET( BrainsRegressionData_FOUND "YES" )
  ENDIF(EXISTS ${BrainsRegressionData_DATA_PATH})

  MARK_AS_ADVANCED(
    BrainsRegressionData_DATA_PATH
  )
