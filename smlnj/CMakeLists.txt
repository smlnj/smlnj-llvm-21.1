# CMake configuration for SML/NJ LLVM with JWA patches
#
# COPYRIGHT (c) 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
# All rights reserved.
#

# When the project is not a sub-project, generate a placeholder
# "smlnj/config.h" file
#
if(PROJECT_IS_TOP_LEVEL)

  # workaround for older versions of CMake
  #
  if (CMAKE_VERSION VERSION_LESS 3.25)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
      set(LINUX ON CACHE BOOL "Set to true when the target system is Linux" INTERNAL)
    endif()
  endif()

  # determine the operating system
  #
  if (APPLE)
    set(SMLNJ_OPSYS_DARWIN ON)
  elseif (LINUX)
    set(SMLNJ_OPSYS_LINUX ON)
  else ()
    message(FATAL_ERROR "unsupported operating system")
  endif()

  # determine the archicture
  #
  if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(SMLNJ_ARCH_AMD64 ON)
  elseif ((${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64") OR (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64"))
    set(SMLNJ_ARCH_ARM64 ON)
  else ()
    message(FATAL_ERROR "unsupported architecture ${CMAKE_HOST_SYSTEM_PROCESSOR}")
  endif()

  # determine the enabled targets
  #
  foreach(T IN LISTS LLVM_TARGETS_TO_BUILD)
    if(${T} STREQUAL "X86")
      message(STATUS "enable X86 target")
      set(SMLNJ_ENABLE_X86 ON)
    elseif(${T} STREQUAL "AArch64")
      message(STATUS "enable ARM64 target")
      set(SMLNJ_ENABLE_ARM64 ON)
    else()
      message(FATAL_ERROR "unknown target architecture ${T}")
    endif()
  endforeach()

  set(include_dir ${CMAKE_CURRENT_BINARY_DIR}/include/smlnj)

  # generate the config.h file
  #
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.cmake
    ${include_dir}/config.h
    @ONLY)

  # arrange for the config.h file to be installed
  install(DIRECTORY ${include_dir} TYPE INCLUDE)

else() # NOT PROJECT_IS_TOP_LEVEL

  # when we are building the code-generator library as a sub-project, we need
  # to specify where to find the SML/NJ config.h file.
  #
  include_directories(BEFORE ${CMAKE_BINARY_DIR}/src/include)

endif()

include_directories(include)
add_subdirectory(include)
add_subdirectory(lib-codegen)
add_subdirectory(lib-heap2obj)
add_subdirectory(heap2obj)

if (SMLNJ_CFGC_BUILD)
  message(STATUS "cfgc tool enabled.")
  add_subdirectory(cfgc)
endif()
