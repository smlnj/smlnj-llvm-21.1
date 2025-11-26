/// \file heap2obj.hpp
///
/// \copyright 2025 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief Support for generating object files from heap images.
///
/// \author John Reppy
///

#ifndef _HEAP2OBJ_HPP_
#define _HEAP2OBJ_HPP_

#include <string_view>
#include <vector>

namespace smlnj {

#ifndef _SMLNJ_BASE_HPP_

// return status
enum class Status {
    eSuccess = 0,
    eFailure
};

#ifndef OBJECT_FILE_EXTENSION
#define OBJECT_FILE_EXTENSION ".o"
#endif

#endif

/// different output targets
enum class Output {
    eObjFile,           ///< generate an object (".o") file
    eAsmFile,           ///< generate native assembly in a ".s" file
    eLLVMAsmFile        ///< generate LLVM assembly in a ".ll" file
};

/// output the heap data to an object file
/// \param outFile   the name of the output file (including extension)
/// \param heap      the heap-image data
/// \param fileType  the format of the output file
/// \param heapFile  the optional name of the source file
/// \return the status of the output operation
Status emitHeapFile (
    std::string_view outFile,
    std::vector<char> const &heap,
    Output fileType,
    std::string_view heapFile = "");

} // namespace smlnj

#endif // !_HEAP2OBJ_HPP_
