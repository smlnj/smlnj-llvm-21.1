/// \file objfile-stream.cpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief This file defines a subclass of llvm::raw_pwrite_stream
///        that we use to generate in-memory object files.
///
/// \author John Reppy
///

#include "objfile-stream.hpp"

namespace smlnj {
namespace cfgcg {

/******************** class ObjfileStream ********************/

void ObjfileStream::write_impl (const char *ptr, size_t nBytes)
{
    size_t dstOffset = this->_data.size();
    this->_data.resize(dstOffset + nBytes);
    ::memcpy(this->_data.data() + dstOffset, ptr, nBytes);
}

void ObjfileStream::pwrite_impl (const char *ptr, size_t nBytes, uint64_t offset)
{
    size_t dstOffset = this->_data.size();

    // see llvm/Support/raw_ostream.h
    assert (offset + nBytes <= dstOffset
        && "We don't support extending the stream");

    ::memcpy(this->_data.data() + offset, ptr, nBytes);
}

uint64_t ObjfileStream::current_pos() const
{
    return this->_data.size();
}

} // namespace cfgcg
} // namespace smlnj
