macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 -pedantic ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)

use_c99()

configure_file (config.h.in ${PROJECT_SOURCE_DIR}/src/config.h)

if(NOT MSVC)
  if(NOT WIN32)
    find_library(MATH m)
  else()
    set(MATH "")
  endif()
  include(FindZLIB)
else()
  set(MATH "")
  find_program(NUGET nuget)
  if (NUGET)
    execute_process(COMMAND ${NUGET} install zlib)
  endif()
  include_directories(${PROJECT_SOURCE_DIR}/windows/third-party/zlib-1.2.11/include/)
endif()

# include(FindZLIB)
# include_directories(${ZLIB_INCLUDE_DIRS})
# MESSAGE(STATUS "ZLIB libs" ${ZLIB_LIBRARIES})

if(NOT MSVC)
  if(NOT WIN32)
    find_library(MATH m)
  else()
    set(MATH "")
  endif()
  include(FindZLIB)
  include_directories(${ZLIB_INCLUDE_DIRS})
  SET(PKG_CONFIG_PRIVATELIBS "-lm ${PKG_CONFIG_PRIVATELIBS}")
  SET(PKG_CONFIG_PRIVATELIBS "-lz ${PKG_CONFIG_PRIVATELIBS}")
else()
    set(MATH "")
    find_program(NUGET nuget)
    if (NOT NUGET)
       message(FATAL "Cannot find nuget command line tool.\nInstall it with e.g. choco install nuget.commandline")
    else()
         execute_process(COMMAND ${NUGET} install zlib)
    endif()
    include_directories(${PROJECT_SOURCE_DIR}/windows/third-party/zlib-1.2.11/include/)
endif()


set(libsrc hrtf/reader.c hdf/superblock.c hdf/dataobject.c hdf/btree.c hdf/fractalhead.c hdf/gunzip.c hdf/gcol.c hrtf/check.c hrtf/spherical.c 
  hrtf/lookup.c hrtf/tools.c hrtf/kdtree.c hrtf/neighbors.c hrtf/interpolate.c hrtf/resample.c hrtf/loudness.c hrtf/minphase.c hrtf/easy.c hrtf/cache.c 
  resampler/speex_resampler.c)
add_library(mysofa-static STATIC ${libsrc})
target_link_libraries (mysofa-static ${MATH} ${ZLIB_LIBRARIES})
SET_TARGET_PROPERTIES(mysofa-static PROPERTIES OUTPUT_NAME mysofa CLEAN_DIRECT_OUTPUT 1 POSITION_INDEPENDENT_CODE ON)
install(TARGETS mysofa-static
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(BUILD_SHARED_LIBS)
  add_library(mysofa-shared SHARED ${libsrc})
  target_link_libraries (mysofa-shared ${MATH} ${ZLIB_LIBRARIES})
  SET_TARGET_PROPERTIES(mysofa-shared PROPERTIES OUTPUT_NAME mysofa CLEAN_DIRECT_OUTPUT 1)
  set_property(TARGET mysofa-shared PROPERTY VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  set_property(TARGET mysofa-shared PROPERTY SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} )
  set_property(TARGET mysofa-shared PROPERTY C_VISIBILITY_PRESET hidden)
  GENERATE_EXPORT_HEADER(mysofa-shared BASE_NAME mysofa EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/src/hrtf/mysofa_export.h)
  install(TARGETS mysofa-shared 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
else()
  GENERATE_EXPORT_HEADER(mysofa-static BASE_NAME mysofa EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/src/hrtf/mysofa_export.h)
endif()

install(FILES hrtf/mysofa.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(BUILD_TESTS)
  add_executable(mysofa2json tests/sofa2json.c tests/json.c)
  target_link_libraries (mysofa2json mysofa-static)

  add_executable(external tests/external.c tests/check.c tests/lookup.c tests/neighbors.c tests/interpolate.c tests/resample.c tests/loudness.c
    tests/minphase.c tests/easy.c tests/cache.c tests/json.c)
  target_link_libraries (external mysofa-shared ${CUNIT_LIBRARIES})
  add_test(NAME external WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND external)

  add_executable(internal tests/internal.c tests/tools.c hrtf/tools.c)
  target_link_libraries (internal m ${CUNIT_LIBRARIES})
  add_test(NAME internal WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND internal)
  

  install(TARGETS mysofa2json
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(BUILD_TESTS)

SET(PKG_CONFIG_PRIVATELIBS "${PKG_CONFIG_PRIVATELIBS}" PARENT_SCOPE)

