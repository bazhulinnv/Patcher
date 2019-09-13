cmake_minimum_required(VERSION 3.13)

set(MODULE_NAME Patcher)
project(${MODULE_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE True)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#------------------------------------------------------------------------------
#  GOOGLE TEST FRAMEWORK INTEGRATION -- BEGIN
#------------------------------------------------------------------------------
# sorce: https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# # Now simply link against gtest or gtest_main as needed. Eg
# add_executable(example example.cpp)
# target_link_libraries(example gtest_main)
# add_test(NAME example_test COMMAND example)

#------------------------------------------------------------------------------
#  GOOGLE TEST FRAMEWORK INTEGRATION -- END
#------------------------------------------------------------------------------

include(build_options)

if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dbpg_BINARY_DIR}/bin)
endif()

message("CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

#------------------------------------------------------------------------------
#  Setup
#------------------------------------------------------------------------------

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
#----------------------------------------------

#----------------------------------------------
message("pqxx_libs = ${pqxx_libs}")

set(DB_PROVIDER DBProvider)
set(PATCH_BUILDER PatchBuilder)
set(PATCH_INSTALLER PatchInstaller)


#---------------------------------
set(patcher_INCS_prov	include/DBProvider/LoginData.h
						include/DBProvider/DBConnection.h
						include/DBProvider/DBProvider.h

						include/Shared/Logger.h
						include/Shared/ParsingTools.h
						include/Shared/TextTable.h
						)

set(patcher_SRCS_prov	src/LoginData.cpp
						src/DBConnection.cpp
						src/DBProvider.cpp
						
						src/Logger.cpp
						src/ParsingTools.cpp
						src/TextTable.cpp
						)


#---------------------------------
set(patcher_INCS_bld	include/PatchBuilder/PatchBuilder.h
						include/DBProvider/DBProvider.h
						)

set(patcher_SRCS_bld	src/PatchBuilder.cpp
	)


#---------------------------------
set(patcher_INCS_inst	include/PatchInstaller/PatchInstaller.h
						include/PatchInstaller/DependenciesChecker.h
						include/PatchInstaller/FileParser.h
						include/DBProvider/DBProvider.h
						include/Shared/Logger.h
						include/Shared/ParsingTools.h
						)

set(patcher_SRCS_inst	src/PatchInstaller.cpp
						src/DependenciesChecker.cpp
						src/FileParser.cpp
	)


#---------------------------------
set(patcher_INCS_shared	include/Shared/Logger.h
						include/Shared/ParsingTools.h
						include/Shared/TextTable.h
						)

set(patcher_SRCS_shared	src/Logger.cpp
						src/PatchInstaller.cpp
						src/TextTable.cpp
						)


#---------------------------------
add_library(${DB_PROVIDER} ${patcher_INCS_prov} ${patcher_SRCS_prov})
target_include_directories(${DB_PROVIDER}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_link_libraries(${DB_PROVIDER} PRIVATE ${pqxx_libs} ${FILESYSTEM_LIBRARY})

# Locate unit tests for DBProvider
add_executable(${DB_PROVIDER}_tests	tests/DBProvider_tests.cpp
									tests/DBProvider_google_tests/Test_PqxxConnection.cpp
									tests/DBProvider_google_tests/Test_DBConnection.cpp
									tests/DBProvider_google_tests/Test_DBProvider_Objects.cpp
									tests/DBProvider_google_tests/Test_DBProvider_PrepareQueries.cpp
									tests/DBProvider_google_tests/Test_DBProvider_Methods.cpp)

# Link current run-configuration with gtest, pqxx libraries
target_link_libraries(${DB_PROVIDER}_tests PRIVATE ${pqxx_libs} ${FILESYSTEM_LIBRARY} ${DB_PROVIDER} gtest_main)


#---------------------------------
add_library(${PATCH_BUILDER} ${patcher_INCS_bld} ${patcher_SRCS_bld})
target_include_directories(${PATCH_BUILDER}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_link_libraries(${PATCH_BUILDER} PRIVATE ${pqxx_libs} ${FILESYSTEM_LIBRARY} ${DB_PROVIDER})

add_executable(${PATCH_BUILDER}_exe  src/PatchBuilder_exe.cpp)
target_link_libraries(${PATCH_BUILDER}_exe PRIVATE ${pqxx_libs} ${FILESYSTEM_LIBRARY} ${DB_PROVIDER} ${PATCH_BUILDER})


#---------------------------------
add_library(${PATCH_INSTALLER} ${patcher_INCS_inst} ${patcher_SRCS_inst})
target_include_directories(${PATCH_INSTALLER}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_link_libraries(${PATCH_INSTALLER} PRIVATE ${pqxx_libs} ${FILESYSTEM_LIBRARY} ${DB_PROVIDER})

add_executable(${PATCH_INSTALLER}_exe src/PatchInstaller_exe.cpp)
target_link_libraries(${PATCH_INSTALLER}_exe PRIVATE ${pqxx_libs} ${FILESYSTEM_LIBRARY} ${DB_PROVIDER} ${PATCH_INSTALLER})


#---------------------------------
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