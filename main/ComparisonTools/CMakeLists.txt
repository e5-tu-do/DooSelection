add_executable(MCDataSeparationComparison MCDataSeparationComparison.cpp)
add_executable(SweightDataChi2ClassifierSeparationComparison SweightDataChi2ClassifierSeparationComparison.cpp)

target_link_libraries(MCDataSeparationComparison ${DooFit_LIBRARIES} ${DooCore_LIBRARIES} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(SweightDataChi2ClassifierSeparationComparison ${DooFit_LIBRARIES} ${DooCore_LIBRARIES} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS SweightDataChi2ClassifierSeparationComparison DESTINATION bin)
install(TARGETS MCDataSeparationComparison DESTINATION bin)
