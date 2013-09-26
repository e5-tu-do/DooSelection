add_executable(SingleCutGrimReaper SingleCutGrimReaper.cpp)

target_link_libraries(SingleCutGrimReaper dsReducer ${ALL_LIBRARIES})

install(TARGETS SingleCutGrimReaper DESTINATION bin)
