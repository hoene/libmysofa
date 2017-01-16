add_library(mysofa mysofa.c superblock.c dataobject.c btree.c fractalhead.c gunzip.c gcol.c check.c spherical.c sort.c tools.c)
target_link_libraries (mysofa m z)

#add_executable(../mysofa2json sofa2json.c)
#target_link_libraries (../mysofa2json ../mysofa)

add_executable(mysofa2json sofa2json.c mysofa.c superblock.c dataobject.c btree.c fractalhead.c gunzip.c gcol.c check.c spherical.c sort.c tools.c)
target_link_libraries (mysofa2json m z)

add_executable(test_tools test_tools.c tools.c)
target_link_libraries (test_tools m)
add_test(tools test_tools)

add_executable(test_sort test_sort.c)
target_link_libraries (test_sort m mysofa)
add_test(sort test_sort)
