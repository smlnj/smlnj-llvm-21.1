/// \file target-info.hpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief Information about the target architecture and how SML is mapped
///        onto it.
///
/// \author John Reppy
///

#ifndef _TARGET_INFO_HPP_
#define _TARGET_INFO_HPP_

#include "llvm/TargetParser/Triple.h"

#include <cstdint>
#include <string_view>
#include <vector>

#include "cm-registers.hpp"

namespace smlnj {
namespace cfgcg {

/// size and alignment info for a C type; we use this information for raw C calls
struct TypeInfo {
    uint32_t    sizeb;
    uint32_t    align;
};

/// size and alignment info for the standard C typea
struct CTypeInfo {
    TypeInfo    boolTy;
    TypeInfo    charTy;
    TypeInfo    shortTy;
    TypeInfo    intTy;
    TypeInfo    longTy;
    TypeInfo    longLongTy;
    TypeInfo    floatTy;
    TypeInfo    doubleTy;
    TypeInfo    ptrTy;
};

struct TargetInfo {
    std::string name;                   ///< the target's name; this should agree with LLVM's
                                        ///  naming conventions (see lib/Support/Triple.cpp).
    std::string dataLayout;             ///< LLVM data layout string
    std::string spName;                 ///< the assembly name of the stack pointer register
    llvm::Triple::ArchType arch;        ///< target architecture
    int wordSzB;                        ///< size in bytes of ML word (should also be the
                                        ///  same as the native pointer size)
    int wordSz;                         ///< size in bits of ML word (== 8*wordSzB)
    int numGPArgRegs;                   ///< the number of general-purpose registers that
                                        ///  are available for argument passing.  These
                                        ///  are std-link, std-clos, std-cont, std-arg,
                                        ///  and the misc registers.  It does not include
                                        ///  the implicit CMachine arguments (e.g., allocPtr).
    int numFPArgRegs;                   ///  the number of floating-point registers that
                                        ///  are available for argument passing
    int numCalleeSaves;                 ///< the number of registers used for the SML
                                        ///  callee-saves
    bool hasPCRel;                      ///< true if the target supports PC-relative addressing.
    int stkOffset[CMRegInfo::NUM_REGS]; ///< byte offset from stack pointer to location where
                                        ///  the value is stored.  Will be non-zero only
                                        ///  for CMachine registers that stack allocated
    int callGCOffset;                   ///< stack offset of call-gc entry address
    int raiseOvflwOffset;               ///< stack offset of raise_overflow entry address
    unsigned int allocSlopSzb;          ///< byte size of allocation slop

    /// initialization functions
    using init_fn_t = void (*)();

    mutable bool initialized;           ///< set to true after initialization
    init_fn_t initTargetInfo;
    init_fn_t initTarget;
    init_fn_t initMC;
    init_fn_t initAsmParser;
    init_fn_t initAsmPrinter;

    static TargetInfo const *infoForTarget (std::string_view name);

    static std::vector<std::string_view> targetNames ();

    void initialize () const
    {
        if (! this->initialized) {
            this->initTargetInfo();
            this->initTarget();
            this->initMC();
            this->initAsmParser();
            this->initAsmPrinter();
            this->initialized = true;
        }
    }

    /// the target info for the native (host) architecture
    static TargetInfo const *native;

    /// return the number of GC roots, which are std-link, std-clos,
    /// std-cont, the callee saves, and std-arg
    int numGCRoots () const { return this->numCalleeSaves + 4; }

    llvm::Triple getTriple() const;

    /// given a number of bytes, round it up to the next multiple of the
    /// target's word size
    uint64_t roundToWordSz (uint64_t nBytes) const
    {
        uint64_t mask = this->wordSzB - 1;
        return (nBytes + mask) & ~mask;
    }
};

} // namespace cfgcg
} // namespace smlnj

#endif // !_TARGET_INFO_HPP_
