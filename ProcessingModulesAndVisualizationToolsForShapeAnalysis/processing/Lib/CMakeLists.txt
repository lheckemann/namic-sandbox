


SUBDIRS(
            IO
            SpatialObject
            Numerics
            Algorithms
       Optimizer
)

IF (USE_SOV)
  SUBDIRS(SOViewer)
ENDIF(USE_SOV)

