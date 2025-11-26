/*! \file config.h
 **
 ** \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
 ** All rights reserved.
 **
 ** \brief Configuration parameters as determined by CMake.  This file is
 **        used when the repository is compiled as a top-level project.
 **
 ** \author John Reppy
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/* size of SML words */
#define SMLNJ_LOG2_WORD_SZB     3
#define SMLNJ_WORD_SZB		(1 << SMLNJ_LOG2_WORD_SZB)
#define SMLNJ_WORD_BITS		(8 * SMLNJ_WORD_SZB)

/* Code generation targets */
#cmakedefine SMLNJ_ENABLE_ARM64
#cmakedefine SMLNJ_ENABLE_X86

/* Host architecture parameters */
#cmakedefine SMLNJ_ARCH_AMD64
#cmakedefine SMLNJ_ARCH_ARM64

/* Host operating system parameters */
#cmakedefine SMLNJ_OPSYS_DARWIN
#cmakedefine SMLNJ_OPSYS_LINUX

/* file suffix for object files */
#define OBJECT_FILE_EXTENSION "@CMAKE_CXX_OUTPUT_EXTENSION@"

#endif /* !_CONFIG_H_ */
