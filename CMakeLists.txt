cmake_minimum_required(VERSION 3.5)
project(libmysofa C CXX)

include(CheckCCompilerFlag)
include(GenerateExportHeader)
include(GNUInstallDirs)

option(BUILD_TESTS "Build test programs" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_STATIC_LIBS "Build static library" ON)
option(CODE_COVERAGE "Enable coverage reporting" OFF)
option(ADDRESS_SANITIZE "Enable address sanitizer" OFF)

set(CPACK_PACKAGE_DESCRIPTION "SOFA file reader for better HRTFs")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "This library reads AES SOFA files and normalizes them")
set(CPACK_PACKAGE_VENDOR "Symonics GmbH")
set(CPACK_PACKAGE_CONTACT "umlaeute@debian.org")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "IOhannes m zmölnig")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "3")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g")

set(PKG_CONFIG_PRIVATELIBS "")

set(PROJECT_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

check_c_compiler_flag(-Wall C_HAS_WALL)
if(C_HAS_WALL)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif(C_HAS_WALL)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DVDEBUG")

if(${ADDRESS_SANITIZE} STREQUAL "ON")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer -g")
endif()

install(FILES share/default.sofa DESTINATION ${CMAKE_INSTALL_DATADIR}/libmysofa)
install(FILES share/MIT_KEMAR_normal_pinna.sofa
        DESTINATION ${CMAKE_INSTALL_DATADIR}/libmysofa)

add_subdirectory(src)

if(BUILD_TESTS)

  enable_testing()


  add_test(NAME GeneralFIR-E COMMAND src/mysofa2json -o tmp.json
           ${PROJECT_SOURCE_DIR}/tests/testfile.sofa)
  add_test(NAME Mesh2HRTF COMMAND src/mysofa2json -c -o tmp.json
           ${PROJECT_SOURCE_DIR}/tests/Mesh2HRTF.sofa)
  add_test(NAME latTestAziBeRTA_Resamp1_Fran COMMAND src/mysofa2json -c -o tmp.json
           ${PROJECT_SOURCE_DIR}/tests/latTestAziBeRTA_Resamp1_Fran.sofa)
  add_test(NAME D1_48K_24bit_0.3s_FIR_SOFA COMMAND src/mysofa2json -o tmp.json
           ${PROJECT_SOURCE_DIR}/tests/D1_48K_24bit_0.3s_FIR_SOFA.sofa)
  add_test(NAME H20_44K_16bit_256tap_FIR_SOFA COMMAND ${PROJECT_SOURCE_DIR}/tests/compareIgnoreNew.sh
           ${PROJECT_SOURCE_DIR}/tests/H20_44K_16bit_256tap_FIR_SOFA $<TARGET_FILE:mysofa2json>)
  add_test(NAME MIT_KEMAR_large_pinna COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/MIT_KEMAR_large_pinna $<TARGET_FILE:mysofa2json>)
  add_test(NAME MIT_KEMAR_normal_pinna COMMAND ${PROJECT_SOURCE_DIR}/tests/compareIgnoreNew.sh
           ${PROJECT_SOURCE_DIR}/tests/MIT_KEMAR_normal_pinna $<TARGET_FILE:mysofa2json>)
  add_test(NAME MIT_KEMAR_normal_pinna.old COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/MIT_KEMAR_normal_pinna.old $<TARGET_FILE:mysofa2json>)
  add_test(NAME dtf_nh2 COMMAND ${PROJECT_SOURCE_DIR}/tests/compareIgnoreNew.sh
           ${PROJECT_SOURCE_DIR}/tests/dtf_nh2 $<TARGET_FILE:mysofa2json>)
  add_test(NAME hrtf_c_nh898 COMMAND ${PROJECT_SOURCE_DIR}/tests/compareIgnoreNew.sh
           ${PROJECT_SOURCE_DIR}/tests/hrtf_c_nh898 $<TARGET_FILE:mysofa2json>)
  foreach(
    ISSUE
    72
    75
    76
    77
    79
    82
    83
    84
    130
    132
    135
    136
    137
    138
    156
    163
    167a
    167b
    168
    171
    173
)
    # issues with osx    96)
    add_test(NAME fail-issue-${ISSUE} COMMAND ${PROJECT_SOURCE_DIR}/tests/notcrashed.sh
             ${PROJECT_SOURCE_DIR}/tests/fail-issue-${ISSUE})
  endforeach(ISSUE)
  add_test(NAME CIPIC_subject_003_hrir_final COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/CIPIC_subject_003_hrir_final $<TARGET_FILE:mysofa2json>)
  add_test(NAME FHK_HRIR_L2354 COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh ${PROJECT_SOURCE_DIR}/tests/FHK_HRIR_L2354 $<TARGET_FILE:mysofa2json>)
  add_test(NAME LISTEN_1002_IRC_1002_C_HRIR COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/LISTEN_1002_IRC_1002_C_HRIR $<TARGET_FILE:mysofa2json>)
  add_test(NAME Pulse COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh ${PROJECT_SOURCE_DIR}/tests/Pulse $<TARGET_FILE:mysofa2json>)
  add_test(NAME Tester COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh ${PROJECT_SOURCE_DIR}/tests/tester $<TARGET_FILE:mysofa2json>)
  add_test(NAME TU-Berlin_QU_KEMAR_anechoic_radius_0.5_1_2_3_m COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/TU-Berlin_QU_KEMAR_anechoic_radius_0.5_1_2_3_m $<TARGET_FILE:mysofa2json>)
  add_test(NAME TU-Berlin_QU_KEMAR_anechoic_radius_0.5m COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/TU-Berlin_QU_KEMAR_anechoic_radius_0.5m $<TARGET_FILE:mysofa2json>)
  add_test(NAME example_dummy_sofa48 COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/example_dummy_sofa48 $<TARGET_FILE:mysofa2json>)
  add_test(NAME TestSOFA48_netcdf472 COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/TestSOFA48_netcdf472 $<TARGET_FILE:mysofa2json>)
  add_test(NAME example_dummy_sofa48_with_user_defined_variable COMMAND ${PROJECT_SOURCE_DIR}/tests/compare.sh
           ${PROJECT_SOURCE_DIR}/tests/example_dummy_sofa48_with_user_defined_variable $<TARGET_FILE:mysofa2json>)
endif(BUILD_TESTS)

include(CPack)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libmysofa.pc.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

# Install exported targets as config files for use from install tree
install(EXPORT mysofa_exports
  NAMESPACE mysofa::
  FILE mysofaTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mysofa
)

# Create a target export file for use from the build tree.
export(EXPORT mysofa_exports
  NAMESPACE mysofa::
  FILE ${CMAKE_CURRENT_BINARY_DIR}/mysofaTargets.cmake
)

include(CMakePackageConfigHelpers)

# Configure package config file
configure_package_config_file(mysofaConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mysofaConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mysofa
)

# Create package version file
# Assuming major version bump = breaking API changes
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/mysofaConfigVersion.cmake
  VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}
  COMPATIBILITY SameMajorVersion)

# Install package config file for use from install tree
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/mysofaConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/mysofaConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mysofa
)
