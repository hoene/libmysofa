set(libsrc hrtf/reader.c hdf/superblock.c hdf/dataobject.c hdf/btree.c hdf/fractalhead.c hdf/gunzip.c hdf/gcol.c hrtf/check.c hrtf/spherical.c 
  hrtf/lookup.c hrtf/tools.c hrtf/kdtree.c hrtf/neighbors.c hrtf/interpolate.c hrtf/resample.c hrtf/loudness.c hrtf/minphase.c hrtf/easy.c hrtf/cache.c)
add_library(mysofa-static STATIC ${libsrc})
add_library(mysofa-shared SHARED ${libsrc})
target_link_libraries (mysofa-static m z resample pthread)
target_link_libraries (mysofa-shared m z resample pthread)
SET_TARGET_PROPERTIES(mysofa-static PROPERTIES OUTPUT_NAME mysofa CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(mysofa-shared PROPERTIES OUTPUT_NAME mysofa CLEAN_DIRECT_OUTPUT 1)
set_property(TARGET mysofa-static PROPERTY C_STANDARD 11)
set_property(TARGET mysofa-shared PROPERTY C_STANDARD 11)

add_executable(mysofa2json tests/sofa2json.c tests/json.c )
target_link_libraries (mysofa2json ${CMAKE_SOURCE_DIR}/build/src/libmysofa.a m z)
set_property(TARGET mysofa2json PROPERTY C_STANDARD 11)

add_executable(tests tests/tests.c tests/tools.c tests/check.c tests/lookup.c tests/neighbors.c tests/interpolate.c tests/resample.c tests/loudness.c 
  tests/minphase.c tests/easy.c tests/cache.c tests/json.c)
  
target_link_libraries (tests mysofa-shared cunit)
set_property(TARGET tests PROPERTY C_STANDARD 11)
add_test(NAME tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND tests)

install(TARGETS mysofa-shared mysofa-static mysofa2json 
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
    
install(FILES hrtf/mysofa.h DESTINATION include)


