set(ADDITIONAL_LIBRARIES "-lTreePlayer")

add_library(dsReducer SHARED SPlotterReducer.cpp SPlotterReducer.h TMVAClassificationReducer.cpp TMVAClassificationReducer.h ShufflerReducer.cpp ShufflerReducer.h BkgCategorizerReducer.cpp BkgCategorizerReducer.h Reducer.cpp Reducer.h ReducerLeaf.cpp ReducerLeaf.h)
target_link_libraries(dsReducer dsMCTools "-lTMVA" ${ADDITIONAL_LIBRARIES} ${ALL_LIBRARIES})

install(TARGETS dsReducer DESTINATION lib)
install(FILES SPlotterReducer.h TMVAClassificationReducer.h ShufflerReducer.h BkgCategorizerReducer.h Reducer.h ReducerLeaf.h DESTINATION include/dooselection/reducer)

