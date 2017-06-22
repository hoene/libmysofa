cmake_minimum_required (VERSION 2.6)
project (libmysofa)

INCLUDE(CheckCCompilerFlag)

option(BUILD_TESTS "Build test programs" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)

CHECK_C_COMPILER_FLAG(-Wall C_HAS_WALL)
IF(C_HAS_WALL)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
ENDIF(C_HAS_WALL)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

install(FILES share/default.sofa DESTINATION share/libmysofa)

if(BUILD_TESTS)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")  
    include(FindCUnit)
    find_package(CUnit REQUIRED cunit)

    enable_testing()

    add_test(CIPIC_subject_003_hrir_final ../tests/compare.sh ../tests/CIPIC_subject_003_hrir_final)
    add_test(FHK_HRIR_L2354.sofa ../tests/compare.sh ../tests/FHK_HRIR_L2354)
    add_test(LISTEN_1002_IRC_1002_C_HRIR.sofa ../tests/compare.sh ../tests/LISTEN_1002_IRC_1002_C_HRIR)
    add_test(MIT_KEMAR_large_pinna.sofa ../tests/compare.sh ../tests/MIT_KEMAR_large_pinna)
    add_test(Pulse ../tests/compare.sh ../tests/Pulse)
    add_test(Tester ../tests/compare.sh ../tests/tester)
    add_test(TU-Berlin_QU_KEMAR_anechoic_radius_0.5_1_2_3_m.sofa ../tests/compare.sh ../tests/TU-Berlin_QU_KEMAR_anechoic_radius_0.5_1_2_3_m)
    add_test(TU-Berlin_QU_KEMAR_anechoic_radius_0.5m.sofa ../tests/compare.sh ../tests/TU-Berlin_QU_KEMAR_anechoic_radius_0.5m)
endif(BUILD_TESTS)

add_subdirectory(src)

SET(CPACK_PACKAGE_DESCRIPTION "SOFA file reader for better HRTFs")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This library reads AES SOFA files and normalizes them")
SET(CPACK_PACKAGE_VENDOR "Symonics GmbH")
SET(CPACK_PACKAGE_CONTACT "christian.hoene@symonics.com")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Christian Hoene") 
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "5")
SET(CPACK_PACKAGE_VERSION_PATCH "pre") 
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g")
INCLUDE(CPack)

