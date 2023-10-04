# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(external_lib)

target_sources(app PRIVATE src/main.c)


# export flags to external lib

zephyr_get_include_directories_for_lang_as_string(          C includes)
zephyr_get_system_include_directories_for_lang_as_string(   C system_includes)
zephyr_get_compile_definitions_for_lang_as_string(          C definitions)
zephyr_get_compile_options_for_lang_as_string(              C options)

set(external_project_cflags
    "${includes} ${system_includes} ${definitions} ${options}"
)


include(ExternalProject)

set(mylib_src_dir   ${CMAKE_CURRENT_SOURCE_DIR}/mylib)
set(mylib_build_dir ${CMAKE_CURRENT_BINARY_DIR}/mylib)

set(MYLIB_LIB_DIR   ${mylib_build_dir}/lib)
set(MYLIB_INCLUDE_DIR ${mylib_src_dir}/include)

if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
# https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html
set(submake "$(MAKE)")
else() # Obviously no MAKEFLAGS. Let's hope a "make" can be found somewhere.
set(submake "make")
endif()

ExternalProject_Add(
  mylib_project                 # Name for custom target
  PREFIX     ${mylib_build_dir} # Root dir for entire project
  SOURCE_DIR ${mylib_src_dir}
  BINARY_DIR ${mylib_src_dir} # This particular build system is invoked from the root
  CONFIGURE_COMMAND ""    # Skip configuring the project, e.g. with autoconf
  BUILD_COMMAND
  ${submake}
  PREFIX=${mylib_build_dir}
  CC=${CMAKE_C_COMPILER}
  AR=${CMAKE_AR}
  CFLAGS=${external_project_cflags}
  INSTALL_COMMAND ""      # This particular build system has no install command
  BUILD_BYPRODUCTS ${MYLIB_LIB_DIR}/libmylib.a
  )

add_library(mylib_lib STATIC IMPORTED GLOBAL)
add_dependencies(
    mylib_lib
    mylib_project
)

set_target_properties(mylib_lib PROPERTIES IMPORTED_LOCATION    ${MYLIB_LIB_DIR}/libmylib.a)
set_target_properties(mylib_lib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MYLIB_INCLUDE_DIR})


target_link_libraries(app PUBLIC mylib_lib)