add_library(dsTriage SHARED Triage.h TriageGeneral.cpp TriagePerformance.cpp TriageClassifierDistribution.cpp TriageBestCut.cpp TriageHistContainer.h Classifier.h ClassifierList.h Tuple.h MCTuple.h SWTuple.h FTTuple.h AbsFitterTuple.h TupleList.h FoM.h FoM.cpp)

target_link_libraries(dsTriage ${ALL_LIBRARIES})

install(TARGETS dsTriage DESTINATION lib)
install(FILES Triage.h TriageHistContainer.h Classifier.h ClassifierList.h Tuple.h MCTuple.h SWTuple.h FTTuple.h AbsFitterTuple.h TupleList.h FoM.h DESTINATION include/dooselection/triage)
