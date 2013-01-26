MESSAGE(STATUS "Looking for DooSelection...")

SET(DooSelection_FOUND FALSE)

IF(IS_DIRECTORY $ENV{DOOSELECTIONSYS})
  SET(DooSelection_BASE $ENV{DOOSELECTIONSYS})
  SET(DooSelection_FOUND TRUE)
  SET(DooSelection_INCLUDES "${DooSelection_BASE}/include")
  SET(DooSelection_INCLUDE_DIR "${DooSelection_BASE}/include")
  SET(DooSelection_LIBRARY_DIR "${DooSelection_BASE}/lib")
  SET(DooSelection_LIBRARIES "-ldsReducer -ldsMCTools -ldsPerformance")
  #SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${DooSelection_LIBRARY_DIR})
  MESSAGE(STATUS "Looking for DooSelection... - found at ${DooSelection_BASE}")
ENDIF(IS_DIRECTORY $ENV{DOOSELECTIONSYS})
