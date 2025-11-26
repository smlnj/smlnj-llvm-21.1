# CMake configuration for the heap2obj executable
#
# COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
# All rights reserved.
#

if(${SMLNJ_ARCH_AMD64})
  set(target_arch X86)
elseif (${SMLNJ_ARCH_ARM64})
  set(target_arch AArch64)
endif()

# determine the LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_TARGETS_TO_BUILD})

set(SRCS
  main.cpp)

add_executable(heap2obj ${SRCS})
add_dependencies(heap2obj Heap2Obj)

target_compile_options(heap2obj PRIVATE -fno-exceptions -fno-rtti)
target_include_directories(heap2obj PRIVATE
  ${SMLNJ_LLVM_SOURCE_DIR}/smlnj/include
  ${SMLNJ_LLVM_BINARY_DIR}/llvm/include
  ${SMLNJ_LLVM_SOURCE_DIR}/llvm/include)
target_link_libraries(heap2obj Heap2Obj ${LLVM_LIBS})

install(TARGETS heap2obj)
