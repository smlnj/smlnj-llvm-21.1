/// \file cm-registers.cpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief Implementation of methods for the classes defined in "cm-registers.hpp"
///
/// \author John Reppy
///

#include "cm-registers.hpp"
#include "target-info.hpp"

namespace smlnj {
namespace cfgcg {

/***** CMRegInfo methods *****/

// the extra arguments that are added to thread the state of the reserved
// registers through the control-flow graph.
static std::string gRegNames[CMRegInfo::NUM_REGS] = {
	"allocPtr", "limitPtr", "storePtr", "exnPtr", "varPtr"
    };

CMRegInfo::CMRegInfo (CMRegId id, int idx, int off)
  : _id(id), _idx(idx), _offset(off), _name(gRegNames[static_cast<int>(id)])
{
}


/***** CMRegs methods *****/

CMRegs::CMRegs (TargetInfo const *target)
{
    if (target == nullptr) {
	this->_nHWRegs = 0;
	return;
    }

  // initialize the register info for the target
    this->_usesBasePtr = ! target->hasPCRel;
    for (int i = 0;  i < CMRegInfo::NUM_REGS;  ++i) {
	CMRegId id = static_cast<CMRegId>(i);
	if (target->stkOffset[i] != 0) {
	    this->_info[i] = CMRegInfo::createStkReg (id, target->stkOffset[i]);
	}
	else {
	    this->_info[i] = CMRegInfo::createReg (id, i);
	}
    }

  // initialize the info about the SML registers that are mapped to machine registers
    int i = 0;
    int nHW = 0;
    for ( ;  i < CMRegInfo::NUM_REGS;  ++i) {
	if ((this->_info[i] != nullptr) && this->_info[i]->isMachineReg()) {
	    this->_hwRegs[nHW++] = this->_info[i];
	}
    }
    while (i < CMRegInfo::NUM_REGS) {
	this->_hwRegs[i] = nullptr;
    }
    this->_nHWRegs = nHW;

}

/***** CMRegState methods *****/

CMRegState::CMRegState (CMRegs const & info)
  : _basePtr(nullptr)
{
  // we initialize all of the registers to nullptr
    for (int i = 0;  i < CMRegInfo::NUM_REGS;  i++) {
	this->_val[i] = nullptr;
    }
}

void CMRegState::copyFrom (CMRegState const & cache)
{
  /* NOTE: we do not copy the _basePtr, because it is invariant */
    for (int i = 0;  i < CMRegInfo::NUM_REGS;  i++) {
	this->_val[i] = cache._val[i];
    }

}

} // namespace cfgcg
} // namespace smlnj
