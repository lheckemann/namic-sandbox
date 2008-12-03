SET ( Tracking_SRCS
vtkTracker.cxx
vtkTrackerTool.cxx
vtkTrackerBuffer.cxx
vtkFrameToTimeConverter.cxx
vtkNDITracker.cxx
#vtkNDICertusTracker.cxx
#vtkTrackerSimulator.cxx
)

SET_SOURCE_FILES_PROPERTIES(
vtkTracker
ABSTRACT
)

# extra tracking libs (starts empty)
SET (Tracking_LIBS
)

#FIND_PATH(NDIOAPI_INCLUDE_DIR ndopto.h)
#FIND_LIBRARY(NDIOAPI_LIBRARY oapi)

#INCLUDE_DIRECTORIES(${NDICAPI_SOURCE_DIR}
#                    ${NDICAPI_BINARY_DIR}
#                    ${NDIOAPI_INCLUDE_DIR})

#SET (Tracking_LIBS ${Tracking_LIBS} ndicapi ${NDIOAPI_LIBRARY})

ADD_LIBRARY(vtkOpenIGTTracking ${Tracking_SRCS})
TARGET_LINK_LIBRARIES(vtkOpenIGTTracking vtkCommon vtkParallel
#                      ${Tracking_LIBS}
                      )

