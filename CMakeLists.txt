cmake_minimum_required(VERSION 3.10)
project(libfio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# zlib
find_library(ZLIB_A NAMES libz.a PATHS ${CMAKE_INSTALL_RPATH})
if(ZLIB_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libz.a not found")
endif()
string(REPLACE lib/libz.a include/ ZLIB_INC ${ZLIB_A})
if(NOT EXISTS ${ZLIB_INC})
  message(FATAL_ERROR "zlib include dir doesn't exist")
endif()

# libprim
find_library(LIBPRIM_A NAMES libprim.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBPRIM_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libprim.a not found")
endif()
string(REPLACE lib/libprim.a include/ LIBPRIM_INC ${LIBPRIM_A})
if(NOT EXISTS ${LIBPRIM_INC})
  message(FATAL_ERROR "libprim include dir doesn't exist")
endif()

add_library(
  fio
  STATIC
  ${PROJECT_SOURCE_DIR}/src/fio/OutFile.cc
  ${PROJECT_SOURCE_DIR}/src/fio/InFile.cc
  ${PROJECT_SOURCE_DIR}/src/fio/InFile.h
  ${PROJECT_SOURCE_DIR}/src/fio/OutFile.h
  )

set_target_properties(
  fio
  PROPERTIES
  LINKER_LANGUAGE
  CXX
  )

target_include_directories(
  fio
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${ZLIB_INC}
  ${LIBPRIM_INC}
  )

target_link_libraries(
  fio
  ${ZLIB_A}
  ${LIBPRIM_A}
  )

include(GNUInstallDirs)

install(
  FILES
  ${PROJECT_SOURCE_DIR}/src/fio/InFile.h
  DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR}/fio/
  )

install(
  FILES
  ${PROJECT_SOURCE_DIR}/src/fio/OutFile.h
  DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR}/fio/
  )

install(
  TARGETS
  fio
  )

