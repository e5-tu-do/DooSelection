set(ADDITIONAL_LIBRARIES "-lTreePlayer")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_library(Reducer SHARED ShufflerReducer.cpp ShufflerReducer.h BkgCategorizerReducer.cpp BkgCategorizerReducer.h Reducer.cpp Reducer.h ReducerLeaf.cpp ReducerLeaf.h)
target_link_libraries(Reducer ${ADDITIONAL_LIBRARIES} ${ALL_LIBRARIES})

install(TARGETS Reducer DESTINATION lib)
install(FILES ShufflerReducer.h BkgCategorizerReducer.h Reducer.h ReducerLeaf.h DESTINATION include/dooselection/reducer)

