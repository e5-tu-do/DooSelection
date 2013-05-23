set(ADDITIONAL_LIBRARIES "-lTreePlayer")

add_library(dsReducer SHARED MultipleCandidateAnalyseReducer.cpp MultipleCandidateAnalyseReducer.h ArrayFlattenerReducer.cpp ArrayFlattenerReducer.h LeafDoublerReducer.cpp LeafDoublerReducer.h SPlotterReducer.cpp SPlotterReducer.h TMVAClassificationReducer.cpp TMVAClassificationReducer.h ShufflerReducer.cpp ShufflerReducer.h BkgCategorizerReducer.cpp BkgCategorizerReducer.h BkgCategorizerReducer2.cpp BkgCategorizerReducer2.h Reducer.cpp Reducer.h ReducerLeaf.cpp ReducerLeaf.h)
target_link_libraries(dsReducer dsMCTools "-lTMVA" ${ADDITIONAL_LIBRARIES} ${ALL_LIBRARIES})

install(TARGETS dsReducer DESTINATION lib)
install(FILES MultipleCandidateAnalyseReducer.h ArrayFlattenerReducer.h LeafDoublerReducer.h SPlotterReducer.h TMVAClassificationReducer.h ShufflerReducer.h BkgCategorizerReducer.h BkgCategorizerReducer2.h Reducer.h ReducerLeaf.h DESTINATION include/dooselection/reducer)

