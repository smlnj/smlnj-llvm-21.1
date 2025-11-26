# CMake configuration for the CFGCodeGen library interface
#
# COPYRIGHT (c) 2024 The Fellowship of SML/NJ (https://smlnj.org)
# All rights reserved.
#

set(SRCS
  cfg.hpp
  cm-registers.hpp
  context.hpp
  lambda-var.hpp
  object-file.hpp
  objfile-stream.hpp
  target-info.hpp)

install(DIRECTORY asdl TYPE INCLUDE)
install(FILES ${SRCS} TYPE INCLUDE)
