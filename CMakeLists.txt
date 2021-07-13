cmake_minimum_required(VERSION 3.20)
project(libfio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)

# zlib
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
  get_target_property(
  ZLIB_INC
  PkgConfig::zlib
  INTERFACE_INCLUDE_DIRECTORIES
)

# libprim
pkg_check_modules(libprim REQUIRED IMPORTED_TARGET libprim)
  get_target_property(
  LIBPRIM_INC
  PkgConfig::libprim
  INTERFACE_INCLUDE_DIRECTORIES
)

add_library(
  fio
  SHARED
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
  POSITION_INDEPENDENT_CODE
  ON
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
  PkgConfig::zlib
  PkgConfig::libprim
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

configure_file(
  ${PROJECT_SOURCE_DIR}/libfio.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libfio.pc
  @ONLY
  )
install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/libfio.pc
  DESTINATION
  ${CMAKE_INSTALL_LIBDIR}/pkgconfig/
  )
