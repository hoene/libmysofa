add_library(mysofa mysofa.c superblock.c dataobject.c btree.c fractalhead.c gunzip.c gcol.c normalize.c)
target_link_libraries (mysofa m z)

#add_executable(../mysofa2json sofa2json.c)
#target_link_libraries (../mysofa2json ../mysofa)

add_executable(mysofa2json sofa2json.c mysofa.c superblock.c dataobject.c btree.c fractalhead.c gunzip.c gcol.c normalize.c)
target_link_libraries (mysofa2json m z)


