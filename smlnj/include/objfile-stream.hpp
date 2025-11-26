/// \file objfile-stream.hpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief This file defines a subclass of llvm::raw_pwrite_stream
///        that we use to generate in-memory object files.
///
/// \author John Reppy
///

#ifndef _OBJFILE_STREAM_HPP_
#define _OBJFILE_STREAM_HPP_

#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace smlnj {
namespace cfgcg {

/******************** class ObjfileStream ********************/

class ObjfileStream : public llvm::raw_pwrite_stream {
public:
    /// Construct a new ObjfilePWriteStream.
    explicit ObjfileStream () : llvm::raw_pwrite_stream(true) { }

    ~ObjfileStream () override = default;

    void flush () = delete;

    /// Return a StringRef for the vector contents.
    llvm::StringRef getData () const
    {
        return llvm::StringRef (
            reinterpret_cast<const char *>(this->_data.data()),
            this->_data.size());
    }

    /// clear the buffer so that it is empty
    void clear () { this->_data.clear(); }

    /// return the number of bytes that have been written into the stream
    size_t size () const { return this->_data.size(); }

private:
    std::vector<uint8_t> _data;         /// vector of bytes to hold output

    /// See raw_ostream::write_impl.
    void write_impl (const char *ptr, size_t nBytes) override;

    void pwrite_impl (const char *ptr, size_t nBytes, uint64_t offset) override;

    /// Return the current position within the stream.
    uint64_t current_pos() const override;

};

} // namespace cfgcg
} // namespace smlnj

#endif // !_OBJFILE_STREAM_HPP_
