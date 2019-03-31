cmake_minimum_required (VERSION 2.8)
project (libmysofa)

INCLUDE(CheckCCompilerFlag)
include(GenerateExportHeader)
include(GNUInstallDirs)

option(BUILD_TESTS "Build test programs" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)

SET(CPACK_PACKAGE_DESCRIPTION "SOFA file reader for better HRTFs")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This library reads AES SOFA files and normalizes them")
SET(CPACK_PACKAGE_VENDOR "Symonics GmbH")
SET(CPACK_PACKAGE_CONTACT "umlaeute@debian.org")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "IOhannes m zmölnig")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "5")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g")

SET(PKG_CONFIG_PRIVATELIBS "")

SET(PROJECT_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

CHECK_C_COMPILER_FLAG(-Wall C_HAS_WALL)
IF(C_HAS_WALL)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
ENDIF(C_HAS_WALL)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DVDEBUG")

install(FILES share/default.sofa DESTINATION ${CMAKE_INSTALL_DATADIR}/libmysofa)
install(FILES share/MIT_KEMAR_normal_pinna.sofa DESTINATION ${CMAKE_INSTALL_DATADIR}/libmysofa)

if(BUILD_TESTS)

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
  include(FindCUnit)
  include_directories(${CUNIT_INCLUDE_DIRS})

  find_package(CUnit REQUIRED cunit)

  enable_testing()

 # add_test(security_1 ./src/mysofa2json ../tests/security/1.sofa)
 # add_test(security_2 /bin/sh -c "./src/mysofa2json ../tests/security/2.sofa || test \$? = 22")

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

INCLUDE(CPack)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/libmysofa.pc.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
  @ONLY
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
  DESTINATION "lib/pkgconfig"
)
