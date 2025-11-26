/// \file object-file.hpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief A wrapper around LLVM's JIT infrastructure that we use to extract the
///        executable code for the CFG.
///
/// \author John Reppy
///

#ifndef _OBJECT_FILE_HPP_
#define _OBJECT_FILE_HPP_

#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include "objfile-stream.hpp"

namespace smlnj {
namespace cfgcg {

/******************** class ObjectFile ********************/

class ObjectFile {
public:

    ObjectFile () = delete;
    ObjectFile (ObjectFile &) = delete;

    /// create an object-file object from the output of the code generator
    /// \param output  the output stream that the code was generated into
    explicit ObjectFile (ObjfileStream const &output);

    ~ObjectFile ();

    /// return the size of the executable parts (code + data) of this object file
    size_t size () const;

    /// return a pointer to the executable parts (code + data) of this object file
    uint8_t const *data () const;

private:
    struct TargetInfo const *_target;
    class MemManager *_memManager;

}; // ObjectFile

} // namespace cfgcg
} // namespace smlnj

#endif //! _OBJECT_FILE_HPP_
