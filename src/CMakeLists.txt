add_library(mysofa mysofa.c superblock.c dataobject.c btree.c fractalhead.c gunzip.c gcol.c check.c spherical.c lookup.c tools.c kdtree.c neighbors.c interpolate.c resample.c loudness.c minphase.c)
target_link_libraries (mysofa m z resample)

#add_executable(../mysofa2json sofa2json.c)
#target_link_libraries (../mysofa2json ../mysofa)

add_executable(mysofa2json sofa2json.c)
target_link_libraries (mysofa2json mysofa)

add_executable(test_tools test_tools.c tools.c)
target_link_libraries (test_tools m)
add_test(tools test_tools)

add_executable(test_lookup test_lookup.c)
target_link_libraries (test_lookup mysofa)
add_test(sort test_lookup)

add_executable(test_neighbors test_neighbors.c)
target_link_libraries (test_neighbors mysofa)
add_test(sort test_neighbors)

add_executable(test_interpolate test_interpolate.c)
target_link_libraries (test_interpolate mysofa)
add_test(sort test_interpolate)

add_executable(test_resample test_resample.c)
target_link_libraries (test_resample mysofa)
add_test(sort test_resample)

add_executable(test_loudness test_loudness.c)
target_link_libraries (test_loudness mysofa)
add_test(sort test_loudness)

add_executable(test_minphase test_minphase.c)
target_link_libraries (test_minphase mysofa)
add_test(sort test_minphase)
