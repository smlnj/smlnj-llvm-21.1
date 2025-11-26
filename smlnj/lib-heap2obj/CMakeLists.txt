# CMake configuration for the heap2obj library
#
# COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
# All rights reserved.
#

set(SRCS
  heap2obj.cpp)

add_library(Heap2Obj STATIC ${SRCS})

add_dependencies(Heap2Obj llvm-headers llvm-libraries)

target_compile_definitions(Heap2Obj PRIVATE "-DSMLNJ_TARGET_TRIPLE=\"${LLVM_HOST_TRIPLE}\"")
target_compile_options(Heap2Obj PRIVATE -fno-exceptions -fno-rtti)
target_include_directories(Heap2Obj PRIVATE
  ${SMLNJ_LLVM_BINARY_DIR}/smlnj/include
  ${SMLNJ_LLVM_BINARY_DIR}/llvm/include
  ${SMLNJ_LLVM_SOURCE_DIR}/llvm/include)

install(TARGETS Heap2Obj
  ARCHIVE DESTINATION lib)
