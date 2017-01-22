add_library(mysofa hrtf/reader.c hdf/superblock.c hdf/dataobject.c hdf/btree.c hdf/fractalhead.c hdf/gunzip.c hdf/gcol.c hrtf/check.c hrtf/spherical.c hrtf/lookup.c hrtf/tools.c hrtf/kdtree.c hrtf/neighbors.c hrtf/interpolate.c hrtf/resample.c hrtf/loudness.c hrtf/minphase.c hrtf/easyapi.c)
target_link_libraries (mysofa m z resample)
set_property(TARGET mysofa PROPERTY C_STANDARD 11)

add_executable(mysofa2json tests/sofa2json.c tests/json.c)
target_link_libraries (mysofa2json mysofa)
set_property(TARGET mysofa2json PROPERTY C_STANDARD 11)

add_executable(tests tests/tests.c tests/tools.c tests/check.c tests/lookup.c tests/neighbors.c tests/interpolate.c tests/resample.c tests/loudness.c tests/minphase.c)
target_link_libraries (tests mysofa cunit)
set_property(TARGET tests PROPERTY C_STANDARD 11)
add_test(tests tests)

#add_executable(test_minphase tests/test_minphase.c)
#add_executable(test_easy tests/test_easy.c tests/json.c)
