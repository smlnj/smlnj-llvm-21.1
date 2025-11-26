/// \file cm-registers.hpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief This file defines the `CMRegState` class, which encapsulates the
///        state of the CMachine (e.g., allocation pointer, limit pointer, ...).
///
/// \author John Reppy
///

#ifndef _SML_REGISTERS_HPP_
#define _SML_REGISTERS_HPP_

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include <string_view>
#include <vector>

namespace smlnj {
namespace cfgcg {

/// the CMachine special "registers".  These are registers that need to be
/// threaded through the environment and through function calls as extra
/// parameters.  On some targets, some of these may be allocated in the stack.
//
enum class CMRegId {
    ALLOC_PTR = 0,      ///< allocation pointer
    LIMIT_PTR,          ///< heap-limit pointer
    STORE_PTR,          ///< points to list of store records
    EXN_HNDLR,          ///< exception handler
    VAR_PTR,            ///< var_ptr register
    NUM_REGS            ///< the number of special registers
};

class CMRegInfo {
  public:

    static const int NUM_REGS = static_cast<int>(CMRegId::NUM_REGS);

    /// functions for creating registers
    static CMRegInfo *createReg (CMRegId id, int idx)
    {
        return new CMRegInfo (id, idx, 0);
    }
    static CMRegInfo *createStkReg (CMRegId id, int offset)
    {
        return new CMRegInfo (id, -1, offset);
    }

    CMRegId id () const { return this->_id; }

    /// return the index in the JWA register list for this register; will be < 0 for
    /// memory-allocated registers.
    int index () const { return this->_idx; }

    /// return the stack offset for this register
    int offset () const { return this->_offset; }

    std::string const &name () const { return this->_name; }

    bool isMachineReg () const { return (this->_idx >= 0); }
    bool isMemReg () const { return (this->_idx < 0); }

  private:
    CMRegId _id;                ///< The ID of this register.
    int _idx;                   ///< The index of hardware register assigned to this register.
                                ///  This value is the parameter index in the JWA calling
                                ///  convention.  It will be -1 for stack allocated registers
    int _offset;                ///< For stack allocated registers, this is the offset from
                                ///  the stack pointer to where the register is allocated in
                                ///  the frame
    std::string _name;          ///< The register's name

    CMRegInfo (CMRegId id, int idx, int off);

};

// collective information about the special CMachine registers for a given target
//
class CMRegs {
  public:

    /// setup the register information for the specified target architecture
    //
    CMRegs (struct TargetInfo const *target);

    /// does the target require the base address register?
    bool usesBasePtr () const { return this->_usesBasePtr; }

    CMRegInfo const *info (CMRegId id) const { return this->_info[static_cast<int>(id)]; }

    /// the number of special registers that are mapped to machine registers and thus
    /// must be passed as extra arguments
    int numMachineRegs () const { return this->_nHWRegs; }

    CMRegInfo const *machineReg (int idx) const { return this->_hwRegs[idx]; }

  private:
    bool        _usesBasePtr;                   ///< true if target needs the base register to
                                                ///  compute code-address values
    int         _nHWRegs;                       ///< the number of special registers that are
                                                ///  hardware supported.
    CMRegInfo *  _info[CMRegInfo::NUM_REGS];    ///< information about the registers.
    CMRegInfo *  _hwRegs[CMRegInfo::NUM_REGS];  ///< info about the SML registers that are
                                                ///  mapped to machine registers and, thus, are
                                                ///  passed as arguments in the JWA convention.
};

/// The CMRegState tracks a mapping from CMachine registers to LLVM values.
///
class CMRegState {
  public:

    CMRegState () { }
    explicit CMRegState (CMRegs const & info);
    ~CMRegState () { }

    /// get the LLVM value that represents the specified CMachine register
    llvm::Value *get (CMRegId r) const
    {
        return this->_val[static_cast<int>(r)];
    }
    llvm::Value *get (CMRegInfo const *info) const { return this->get(info->id()); }

    /// assign a value to a CMachine register
    void set (CMRegId r, llvm::Value *v)
    {
        this->_val[static_cast<int>(r)] = v;
    }

    /// get the LLVM value of the base-address pointer
    llvm::Value *getBasePtr () const { return this->_basePtr; }

    /// set the base-address pointer
    void setBasePtr (llvm::Value *v)
    {
        assert (v->getType()->isIntegerTy() && "base pointer should have intTy");
        this->_basePtr = v;
    }

#ifndef NDEBUG
    void clearBasePtr () { this->_basePtr = nullptr; }
#endif

    void copyFrom (CMRegState const & cache);

  private:
    llvm::Value * _basePtr;                     ///< base-address of current function;
                                                ///  used for computing position-independent
                                                ///  labels
    llvm::Value * _val[CMRegInfo::NUM_REGS];    ///< mapping from registers IDs to their
                                                ///  current representation as an LLVM value.
};

} // namespace cfgcg
} // namespace smlnj

#endif // !_SML_REGISTERS_HPP_
