# CMake configuration for SML/NJ LLVM with JWA patches
#
# COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://www.smlnj.org)
# All rights reserved.
#

# LLVM 21.1 requires 3.20, but we require a more recent version
#
cmake_minimum_required(VERSION 3.23.0)

project(smlnj-llvm
  VERSION 21.1.6
  DESCRIPTION "LLVM 21.1.6 with modifications to support SML/NJ"
  HOMEPAGE_URL https://smlnj.org
  LANGUAGES C CXX)

# LLVM cache variable settings
include(cmake/llvm_settings.cmake)

# more checks for when we are building the LLVM stuff as a standalone project
#
if (PROJECT_IS_TOP_LEVEL)

  # Disable builds in the source tree to prevent corruption.
  if ("${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
  ERROR: builds in the source tree are not allowed.
         Create a separate directory for build files.
")
  endif()

  # check the build type
  if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
  endif()

  # LLVM 18.1 uses C++17
  #
  set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
  set(CMAKE_CXX_STANDARD_REQUIRED YES)

endif(PROJECT_IS_TOP_LEVEL)

# build options
#
option (SMLNJ_CFGC_BUILD
  "Enable building the cfgc command-line tool" OFF)
option (SMLNJ_DEBUG_CODEGEN
  "Enable debug messages in the code-generator library" OFF)

# LLVM uses "all" for all supported architectures, but we only support 2
if(NOT LLVM_TARGETS_TO_BUILD OR LLVM_TARGETS_TO_BUILD STREQUAL "all")
    set(LLVM_TARGETS_TO_BUILD "X86;AArch64")
endif()

## Begin LLVM Configuration ##
message(VERBOSE "** START LLVM CONFIGURATION **")
add_subdirectory(llvm EXCLUDE_FROM_ALL)
message(VERBOSE "** LLVM CONFIGURATION DONE **")
## End LLVM Configuration ##


# The CFG Code Generation Library
#
include_directories(
  include
  llvm/include
  ${CMAKE_CURRENT_BINARY_DIR}/llvm/include)
add_subdirectory(smlnj)
