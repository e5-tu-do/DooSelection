add_library(dsMCTools SHARED idtranslator/IDTranslator.cpp idtranslator/IDTranslator.h)
target_link_libraries(dsMCTools ${ALL_LIBRARIES})

install(TARGETS dsMCTools DESTINATION lib)
install(FILES idtranslator/IDTranslator.h DESTINATION include/dooselection/mctools/idtranslator)


add_library(dsMCTools2 SHARED mcdecaymatrixreader/MCDecayMatrixReader.cpp mcdecaymatrixreader/MCDecayMatrixReader.h mcdecaymatrixreader/CondDBTranslator.cpp mcdecaymatrixreader/CondDBTranslator.h mcdecaymatrixreader/Particle.cpp mcdecaymatrixreader/Particle.h)
target_link_libraries(dsMCTools2 ${ALL_LIBRARIES})

install(TARGETS dsMCTools2 DESTINATION lib)
install(FILES mcdecaymatrixreader/MCDecayMatrixReader.h mcdecaymatrixreader/CondDBTranslator.h mcdecaymatrixreader/Particle.h mcdecaymatrixreader/CondDB_particle_table.txt DESTINATION include/dooselection/mctools/mcdecaymatrixreader)

