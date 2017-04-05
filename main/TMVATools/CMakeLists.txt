IF (ROOT_INSTALLED_VERSION_TOO_NEW)
	add_executable(TMVAClassification TMVAClassification_v608.cpp)
ELSE (ROOT_INSTALLED_VERSION_TOO_NEW)
    add_executable(TMVAClassification TMVAClassification.cpp)
ENDIF (ROOT_INSTALLED_VERSION_TOO_NEW)

add_executable(TMVAViewer TMVAViewer.cpp)

target_link_libraries(TMVAClassification ${DooCore_LIBRARIES} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(TMVAViewer ${DooCore_LIBRARIES} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS TMVAClassification DESTINATION bin)
install(TARGETS TMVAViewer DESTINATION bin)
