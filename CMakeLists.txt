cmake_minimum_required(VERSION 2.8)
project(libmysofa)

include(CheckCCompilerFlag)
include(GenerateExportHeader)
include(GNUInstallDirs)

option(BUILD_TESTS "Build test programs" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)
option(CODE_COVERAGE "Enable coverage reporting" ON)

set(CPACK_PACKAGE_DESCRIPTION "SOFA file reader for better HRTFs")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "This library reads AES SOFA files and normalizes them")
set(CPACK_PACKAGE_VENDOR "Symonics GmbH")
set(CPACK_PACKAGE_CONTACT "umlaeute@debian.org")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "IOhannes m zmölnig")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g")

set(PKG_CONFIG_PRIVATELIBS "")

set(PROJECT_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

check_c_compiler_flag(-Wall C_HAS_WALL)
if(C_HAS_WALL)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif(C_HAS_WALL)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DVDEBUG")

install(FILES share/default.sofa DESTINATION ${CMAKE_INSTALL_DATADIR}/libmysofa)
install(FILES share/MIT_KEMAR_normal_pinna.sofa
        DESTINATION ${CMAKE_INSTALL_DATADIR}/libmysofa)

if(BUILD_TESTS)

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
  include(FindCUnit)
  include_directories(${CUNIT_INCLUDE_DIRS})

  find_package(CUnit REQUIRED cunit)

  enable_testing()

  foreach(
    ISSUE
    72
    75
    76
    77
    79
    82
    83
    84)
    # issues with osx    96)
    add_test(fail-issue-${ISSUE} ../tests/notcrashed.sh
             ../tests/fail-issue-${ISSUE})
  endforeach(ISSUE)

  add_test(CIPIC_subject_003_hrir_final ../tests/compare.sh
           ../tests/CIPIC_subject_003_hrir_final)
  add_test(FHK_HRIR_L2354 ../tests/compare.sh ../tests/FHK_HRIR_L2354)
  add_test(LISTEN_1002_IRC_1002_C_HRIR ../tests/compare.sh
           ../tests/LISTEN_1002_IRC_1002_C_HRIR)
  add_test(MIT_KEMAR_large_pinna ../tests/compare.sh
           ../tests/MIT_KEMAR_large_pinna)
  add_test(Pulse ../tests/compare.sh ../tests/Pulse)
  add_test(Tester ../tests/compare.sh ../tests/tester)
  add_test(TU-Berlin_QU_KEMAR_anechoic_radius_0.5_1_2_3_m ../tests/compare.sh
           ../tests/TU-Berlin_QU_KEMAR_anechoic_radius_0.5_1_2_3_m)
  add_test(TU-Berlin_QU_KEMAR_anechoic_radius_0.5m ../tests/compare.sh
           ../tests/TU-Berlin_QU_KEMAR_anechoic_radius_0.5m)
  add_test(example_dummy_sofa48 ../tests/compare.sh
           ../tests/example_dummy_sofa48)
  add_test(TestSOFA48_netcdf472 ../tests/compare.sh
           ../tests/TestSOFA48_netcdf472)
  add_test(example_dummy_sofa48_with_user_defined_variable ../tests/compare.sh
           ../tests/example_dummy_sofa48_with_user_defined_variable)
endif(BUILD_TESTS)

add_subdirectory(src)

include(CPack)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libmysofa.pc.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
