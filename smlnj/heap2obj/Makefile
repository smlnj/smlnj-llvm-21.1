# Makefile
#
# COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://www.smlnj.org)
# All rights reserved.
#
# Makefile for the heap2obj tool
#

SHELL =		/bin/sh

RUNTIME_DIR =	../../runtime
LLVM_DIR =	$(RUNTIME_DIR)/llvm18

CPP_DEFS =	-I$(RUNTIME_DIR)/include -I$(LLVM_DIR)/smlnj/include
CXX =		clang++ -std=c++17
CXX_DEFS =	-O2
LD_DEFS =	-L$(RUNTIME_DIR)/lib

heap2obj:	main.cpp $(RUNTIME_DIR)/lib/libheap2obj.a
	$(CXX) -o heap2obj $(CXX_DEFS) $(CPP_DEFS) main.cpp $(LD_DEFS) -lheap2obj $(LLVM_LIBS)
