add_library(dsMCTools SHARED idtranslator/IDTranslator.cpp idtranslator/IDTranslator.h)
target_link_libraries(dsMCTools ${ALL_LIBRARIES})

install(TARGETS dsMCTools DESTINATION lib)
install(FILES idtranslator/IDTranslator.h DESTINATION include/dooselection/mctools/idtranslator)

