cmake_minimum_required(VERSION 3.13)

set(MODULE_NAME Patcher)
project(${MODULE_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE True)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
  set(PostgreSQL_INCLUDE_DIR      "$ENV{PostgreSQL_ROOT}/include")
  set(PostgreSQL_TYPE_INCLUDE_DIR "$ENV{PostgreSQL_ROOT}/include")
  set(PostgreSQL_LIBRARY_DIR      "$ENV{PostgreSQL_ROOT}/lib")
endif()
find_package(PostgreSQL QUIET)

if (PostgreSQL_FOUND AND PostgreSQL_LIBRARY)
  set(PostgreSQL_FOUND TRUE PARENT_SCOPE)
else()
  set(PostgreSQL_FOUND FALSE PARENT_SCOPE)
endif()

if (PostgreSQL_FOUND)
  set(SKIP_BUILD_TEST  ON CACHE BOOL "" FORCE)
  set(SKIP_PQXX_SHARED ON CACHE BOOL "" FORCE)
  add_subdirectory(dependencies/pqxx)
  set(pqxx_libs pqxx_static ${PostgreSQL_LIBRARY})
else()
  set(pqxx_libs "")
endif()

set(DB_PROVIDER DBProvider)
set(PATCH_BUILDER PatchBuilder)
set(PATCH_INSTALLER PatchInstaller)

set(patcher_INCS
  include/${DB_PROVIDER}/${DB_PROVIDER}.h
  include/${PATCH_BUILDER}/${PATCH_BUILDER}.h
  include/${PATCH_INSTALLER}/${PATCH_INSTALLER}.h
  )
set(patcher_SRCS 
  src/${DB_PROVIDER}.cpp
  src/${PATCH_BUILDER}.cpp
  src/${PATCH_INSTALLER}.cpp
  )

add_library(${DB_PROVIDER} ${patcher_INCS} ${patcher_SRCS})
add_library(${PATCH_BUILDER} ${patcher_INCS} ${patcher_SRCS})
add_library(${PATCH_INSTALLER} ${patcher_INCS} ${patcher_SRCS})
#-----------------

target_include_directories(${DB_PROVIDER}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_link_libraries(${DB_PROVIDER} PRIVATE glog::glog ${pqxx_libs} ${FILESYSTEM_LIBRARY})
add_executable(${PATCH_BUILDER}_test tests/DBProvider_tests.cpp)

#-----------------
add_executable(${PATCH_BUILDER}  src/${PATCH_BUILDER}/${PATCH_BUILDER}.cpp)
target_link_libraries(${PATCH_BUILDER} PRIVATE glog::glog ${FILESYSTEM_LIBRARY} ${DB_PROVIDER})

#-----------------
add_executable(${PATCH_INSTALLER} src/${PATCH_INSTALLER}/${PATCH_INSTALLER}.cpp)
target_link_libraries(${PATCH_INSTALLER} PRIVATE glog::glog ${FILESYSTEM_LIBRARY} ${DB_PROVIDER})
#-----------------


if (PostgreSQL_FOUND)
  if (WIN32)
    find_path(pqdll_path NAMES libpq.dll PATHS ${PostgreSQL_LIBRARY_DIR} ${PostgreSQL_ROOT}/bin)
    add_custom_command(TARGET ${DB_PROVIDER} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pqdll_path}/libpq.dll"      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/libpq.dll"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pqdll_path}/libeay32.dll"   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/libeay32.dll"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pqdll_path}/libiconv-2.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/libiconv-2.dll"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pqdll_path}/libintl-8.dll"  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/libintl-8.dll"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pqdll_path}/ssleay32.dll"   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/ssleay32.dll"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${pqdll_path}/zlib1.dll"      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/zlib1.dll"
      )
    unset(pqdll_path)
  endif(WIN32)
endif(PostgreSQL_FOUND)