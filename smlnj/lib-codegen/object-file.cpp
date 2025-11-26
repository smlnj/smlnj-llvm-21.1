/// \file object-file.cpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief A wrapper around LLVM's JIT infrastructure that we use to extract the
///        executable code for the CFG.
///
/// \author John Reppy
///

#include "object-file.hpp"
#include "llvm/Support/Error.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/MemoryBufferRef.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include <cstdint>

namespace smlnj {
namespace cfgcg {

inline uintptr_t alignBy (uintptr_t n, uintptr_t align)
{
    return (n + (align - 1)) & ~(align - 1);
}
inline uintptr_t alignBy (uintptr_t n, llvm::Align align)
{
    return alignBy (n, uintptr_t(align.value()));
}

/***** class MemManager *****/

class MemManager : public llvm::RTDyldMemoryManager
{
public:
    /// constructor
    /// \param align  alignment for the base of the code object; this value is
    ///               is required to ensure that relocation on Arm works correctly
    MemManager (llvm::Align align)
    : _mem(nullptr), _base(nullptr), _baseAlign(uintptr_t(align.value())),
      _code("code"), _roData("rodata"), _rwData("rwdata")
    { }

    /// destructor
    virtual ~MemManager () override
    {
        if (this->_mem != nullptr) {
            delete[] this->_mem;
        }
    }

    /// we want to put all of the sections together in a single chunk of memory, so
    /// we tell the JIT linker that it needs to call `reserveAllocationSpace`.
    virtual bool needsToReserveAllocationSpace () override { return true; }

/*
    virtual bool allowStubAllocation () override { return false; }
*/

    virtual void reserveAllocationSpace (
        uintptr_t codeSzb,
        llvm::Align codeAlign,
        uintptr_t roDataSzb,
        llvm::Align roDataAlign,
        uintptr_t rwDataSzb,
        llvm::Align rwDataAlign) override;

    virtual uint8_t *allocateCodeSection (
        uintptr_t szb,
        unsigned align,
        unsigned secID,
        llvm::StringRef name) override;

    virtual uint8_t *allocateDataSection (
        uintptr_t szb,
        unsigned align,
        unsigned secID,
        llvm::StringRef name,
        bool readOnly) override;

    void registerEHFrames (uint8_t *addr, uint64_t loadAddr, size_t numBytes) override;

    virtual bool finalizeMemory (std::string * ErrMsg = nullptr) override
    {
        // nothing to do here, since we are not going to execute the code
        return true;
    }

    /// return the base address of the memory object
    uint8_t const *data () const
    {
        assert (this->_base != nullptr && "no memory allocated");
        return this->_base;
    }

    /// return the total size of the memory object
    size_t size () const
    {
        return this->_code._paddedSzb + this->_roData._paddedSzb + this->_rwData._paddedSzb;
    }

private:
    /// information about an in-memory section from the object file
    struct _Section {
        uint8_t *_data;         ///< base address of section
        size_t _szb;            ///< current size of code/data in section; this
                                ///  represents the sum of allocations in the section
        size_t _paddedSzb;      ///< padded size of the section
#ifdef DEBUG_CODEGEN
        std::string _name;      ///< section name used in debug messages
#endif

        /// constructor
#ifdef DEBUG_CODEGEN
        _Section (std::string_view name)
        : _data(nullptr), _szb(0), _paddedSzb(0), _name(std::string(name))
        { }
#else
        _Section (std::string_view name)
        : _data(nullptr), _szb(0), _paddedSzb(0)
        { }
#endif

        /// set the padded size of the section
        void setSize (size_t szb, llvm::Align align)
        {
            this->_paddedSzb = alignBy(szb, align);
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# [" << this->_name << "] setSize: szb = " << szb << "; padded szb = "
<< this->_paddedSzb << "\n";
#endif
        }

        /// return the next address past this section
        uint8_t *top () const { return this->_data + this->_paddedSzb; }

        /// allocate memory in the section
        uint8_t *alloc (size_t nb, unsigned align)
        {
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# [" << this->_name << "] alloc: nb = " << nb << "; align = " << align
<< "; current szb = " << this->_szb << "; base = " << this->_data
<< "; current top = " << (void *)(this->_data + alignBy(this->_szb, align)) << "\n";
#endif
            assert (this->_data != nullptr && "no memory allocated for section");
            assert (alignBy(this->_szb, align) + alignBy(nb, align) <= this->_paddedSzb
                && "insufficient space for allocation");

            // the base for this allocation
            uint8_t *ptr = this->_data + alignBy(this->_szb, align);
            // the new size
            this->_szb = (ptr - this->_data) + alignBy(nb, align);
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# [" << this->_name << "] alloc: new size = " << this->_szb
<< "; new top = " << this->_data + this->_szb << "\n";
#endif
            return ptr;
        }
    };

    uint8_t *_mem;              ///< pointer to allocated memory
    uint8_t *_base;             ///< pointer to base address for object file data
    uintptr_t _baseAlign;       ///< alignment for _base pointer
    _Section _code;             ///< code section
    _Section _roData;           ///< read-only data section
    _Section _rwData;           ///< read-write data section

}; // class MemManager

void MemManager::reserveAllocationSpace (
    uintptr_t codeSzb,
    llvm::Align codeAlign,
    uintptr_t roDataSzb,
    llvm::Align roDataAlign,
    uintptr_t rwDataSzb,
    llvm::Align rwDataAlign)
{
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# reserve: codeSzb = " << codeSzb << "@" << codeAlign.value()
<< "; roData = " << roDataSzb << "@" << roDataAlign.value()
<< "; rwData = " << rwDataSzb << "@" << rwDataAlign.value() << "\n";
#endif

    assert (this->_mem == nullptr && "memory already allocated");

    // set the section sizes with padding for alignment.  Since we are going to
    // concatenate the sections, we use the greatest alignment value as the padding
    // for all sections
    auto maxAlign = llvm::Align(8); /* this is a workaround for an LLVM bug (Issue #125529) */
    if (codeAlign > maxAlign) { maxAlign = codeAlign; }
    if (roDataAlign > maxAlign) { maxAlign = roDataAlign; }
    if (rwDataAlign > maxAlign) { maxAlign = rwDataAlign; }
    this->_code.setSize(codeSzb, maxAlign);
    this->_roData.setSize(roDataSzb, maxAlign);
    this->_rwData.setSize(rwDataSzb, maxAlign);

    // pad the size of the allocation so that we can guarantee correct alignment
    auto totalSize = this->size() + this->_baseAlign - 1;

    /// allocate the memory for the in-memory sections
    this->_mem = new uint8_t [totalSize];
    this->_base =
        reinterpret_cast<uint8_t *>(
            alignBy(reinterpret_cast<uintptr_t>(this->_mem), this->_baseAlign));

    assert ((this->_base + this->size()) <= (this->_mem + totalSize) && "too small");

/*DEBUG*/
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# reserve: mem = " << this->_mem
<< "; base = " << this->_base
<< "; size = " << (int)(this->size())
<< "; align = " << codeAlign.value()
<< "; top = " << (void *)(this->_base + this->size()) << "\n";
#endif
/*DEBUG*/

    /// assign base addresses for the sections
    this->_code._data = this->_base;
    this->_roData._data = this->_code.top();
    this->_rwData._data = this->_roData.top();
/*DEBUG*/
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# reserve: code = [" << this->_code._data << ".." << this->_code.top()
<< "]; roData = [" << this->_roData._data << ".." << this->_roData.top()
<< "]; rwData = [" << this->_rwData._data << ".." << this->_rwData.top()
<< "]\n";
#endif
/*DEBUG*/

} // MemManager::reserveAllocationSpace

uint8_t *MemManager::allocateCodeSection (
    uintptr_t szb,
    unsigned align,
    unsigned secID,
    llvm::StringRef name)
{
    assert (this->_base != nullptr && "memory has not been reserved");

#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# allocate code[" << name << "]: szb = " << szb
<< "; align = " << align << "\n";
#endif

    return this->_code.alloc(szb, align);

} // MemManager::allocateCodeSection

uint8_t *MemManager::allocateDataSection (
    uintptr_t szb,
    unsigned align,
    unsigned secID,
    llvm::StringRef name,
    bool readOnly)
{
    assert (this->_base != nullptr && "memory has not been reserved");
//    assert (readOnly && "unexpected allocation of RW data");

#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# allocate data[" << name << "]: szb = " << szb
<< "; align = " << align << "; ro = " << readOnly << "\n";
#endif

    if (readOnly) {
        return this->_roData.alloc(szb, align);
    } else {
        return this->_rwData.alloc(szb, align);
    }

} // MemManager::allocateDataSection

void MemManager::registerEHFrames (uint8_t *addr, uint64_t loadAddr, size_t numBytes)
{
#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# register EH frames: addr = " << (void*)addr
<< ", loadAddr = " << loadAddr << "; numBytes = " << numBytes << "\n";
#endif
} // MemManager::registerEHFrames


/***** class SymbolResolver *****/

// Create the LLVM object loader.
class SymbolResolver : public llvm::JITSymbolResolver
{
public:

    SymbolResolver () { }
    virtual ~SymbolResolver () override { }

    virtual void lookup (
        llvm::JITSymbolResolver::LookupSet const &syms,
        llvm::JITSymbolResolver::OnResolvedFunction orf
    ) override
    {
        /* there should not be any symbols to look up, since
         * the generated LLVM is closed.
         */
        assert (false && "unexpected external symbol lookup");
    }

    virtual llvm::Expected<LookupSet> getResponsibilitySet (
        const llvm::JITSymbolResolver::LookupSet & symbols
    ) override
    {
        return llvm::JITSymbolResolver::LookupSet(); /* return empty set */
    }

}; /* class SymbolResolver */


/***** class ObjectFile *****/

ObjectFile::ObjectFile (ObjfileStream const &output)
: _memManager(new MemManager(llvm::Align(0x1000))) /* FIXME: make the alignment target dependent */
{
    auto bytes = output.getData();

    std::unique_ptr<llvm::object::ObjectFile> object
        = cantFail(
            llvm::object::ObjectFile::createObjectFile(
                llvm::MemoryBufferRef(bytes, "object-file memory")));

    SymbolResolver symbolResolver;
    llvm::RuntimeDyld loader(*this->_memManager, symbolResolver);

#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# link object file: " << object->makeTriple().str() << "\n";
#endif

    // Use the LLVM object loader to load the object.
    std::unique_ptr<llvm::RuntimeDyld::LoadedObjectInfo> loadedObject =
        loader.loadObject(*object);
    loader.finalizeWithMemoryManagerLocking();
    if (loader.hasError()) {
        std::string errMsg =
            std::string("RuntimeDyld failed: ") + loader.getErrorString().data();
        llvm::report_fatal_error (llvm::StringRef(errMsg), false);
     }

#ifdef DEBUG_CODEGEN
llvm::dbgs() << "# object file linking done\n";
#endif

}

ObjectFile::~ObjectFile ()
{
    delete this->_memManager;
}

size_t ObjectFile::size () const
{
    return this->_memManager->size();
}

uint8_t const *ObjectFile::data () const
{
    return this->_memManager->data();
}

} // namespace cfgcg
} // namespace smlnj
