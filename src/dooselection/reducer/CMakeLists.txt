set(ADDITIONAL_LIBRARIES "-lTreePlayer")

add_library(dsReducer SHARED MergeTupleReducer.cpp MergeTupleReducer.h MultipleCandidateAnalyseReducer.cpp MultipleCandidateAnalyseReducer.h ArrayFlattenerReducer.cpp ArrayFlattenerReducer.h LeafDoublerReducer.cpp LeafDoublerReducer.h SPlotterReducer.cpp SPlotterReducer.h TMVAClassificationReducer.cpp TMVAClassificationReducer.h ShufflerReducer.cpp ShufflerReducer.h BkgCategorizerReducer.cpp BkgCategorizerReducer.h BkgCategorizerReducer2.cpp BkgCategorizerReducer2.h Reducer.cpp Reducer.h ReducerLeaf.cpp ReducerLeaf.h KinematicReducerLeaf.h KinematicReducerLeaf.cpp VariableCategorizerReducer.h VariableCategorizerReducer.cpp)
target_link_libraries(dsReducer dsMCTools dsMCTools2 "-lTMVA" ${ADDITIONAL_LIBRARIES} ${ALL_LIBRARIES})

install(TARGETS dsReducer DESTINATION lib)
install(FILES MergeTupleReducer.h MultipleCandidateAnalyseReducer.h ArrayFlattenerReducer.h LeafDoublerReducer.h SPlotterReducer.h TMVAClassificationReducer.h ShufflerReducer.h BkgCategorizerReducer.h BkgCategorizerReducer2.h Reducer.h ReducerLeaf.h KinematicReducerLeaf.h VariableCategorizerReducer.h DESTINATION include/dooselection/reducer)
