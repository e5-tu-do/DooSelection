add_library(dsPerformance SHARED performance.cpp performance.h SelectionClassifier/SelectionClassifier.h SelectionTuple/SelectionTuple.h)
target_link_libraries(dsPerformance ${ALL_LIBRARIES})

install(TARGETS dsPerformance DESTINATION lib)
install(FILES performance.h SelectionClassifier/SelectionClassifier.h SelectionTuple/SelectionTuple.h DESTINATION include/dooselection/performance)

