/// \file codegen.cpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief Main code generator code.
///
/// \author John Reppy
///

#include "context.hpp"
#include "cfg.hpp"
#include "codegen.hpp"
#include "target-info.hpp"
#include "object-file.hpp"
#include <iostream>

// Some global flags for controlling the code generator.
// These are just for testing purposes
bool disableGC = false;

static smlnj::cfgcg::Context *gContext = nullptr;

bool setTarget (std::string const &target)
{
    if (gContext != nullptr) {
	if (gContext->targetInfo()->name == target) {
	    return false;
	}
	delete gContext;
    }

    gContext = smlnj::cfgcg::Context::create (target);

    return (gContext == nullptr);

}

// timer support
#include <time.h>

class Timer {
  public:
    static Timer start ()
    {
	struct timespec ts;
	clock_gettime (CLOCK_REALTIME, &ts);
	return Timer (_cvtTimeSpec(ts));
    }
    void restart ()
    {
	struct timespec ts;
	clock_gettime (CLOCK_REALTIME, &ts);
	this->_ns100 = _cvtTimeSpec(ts);
    }
    double msec () const
    {
	struct timespec ts;
	clock_gettime (CLOCK_REALTIME, &ts);
	double t = double(_cvtTimeSpec(ts) - this->_ns100);
	return t / 10000.0;
    }
  private:
    uint64_t _ns100;	// track time in 100's of nanoseconds
    static uint64_t _cvtTimeSpec (struct timespec &ts)
    {
	return (
	    10000000 * static_cast<uint64_t>(ts.tv_sec)
	    + static_cast<uint64_t>(ts.tv_nsec) / 100);
    }
    Timer (uint64_t t) : _ns100(t) { }
};

void codegen (std::string const & src, bool emitLLVM, bool dumpBits, output out)
{
    assert (gContext != nullptr && "call setTarget before calling codegen");

    asdl::file_instream inS(src);

    std::cout << "read pickle ..." << std::flush;
    Timer unpklTimer = Timer::start();
    CFG::comp_unit *cu = CFG::comp_unit::read (inS);
    std::cout << " " << unpklTimer.msec() << "ms\n" << std::flush;

    // generate LLVM
    std::cout << " generate llvm ..." << std::flush;;
    Timer genTimer = Timer::start();
    cu->codegen (gContext);
    std::cout << " " << genTimer.msec() << "ms\n" << std::flush;

    if (emitLLVM) {
	gContext->dump ();
    }

    if (! gContext->verify ()) {
	std::cerr << "Module verified\n";
    }

    std::cout << " optimize ..." << std::flush;;
    Timer optTimer = Timer::start();
    gContext->optimize ();
    std::cout << " " << optTimer.msec() << "ms\n" << std::flush;

//    if (emitLLVM) {
//	gContext->dump ();
//    }

    if (! gContext->verify ()) {
	std::cerr << "Module verified after optimization\n";
    }

  // get the stem of the filename
    std::string stem(src);
    auto pos = stem.rfind(".pkl");
    if (pos+4 != stem.size()) {
	stem = "out";
    }
    else {
	stem = stem.substr(0, pos);
    }

    switch (out) {
      case output::PrintAsm:
	gContext->dumpAsm();
	break;
      case output::AsmFile:
	gContext->dumpAsm (stem);
	break;
      case output::ObjFile:
	gContext->dumpObj (stem);
	break;
      case output::Memory: {
	    auto obj = gContext->compile ();
	    if (obj && dumpBits) {
                size_t sz = obj->size();
                llvm::dbgs()
                    << "##### OBJECT FILE BITS: " << obj->size() << " bytes #####\n";
                uint8_t const *bytes = obj->data();
                for (size_t i = 0;  i < sz; i += 16) {
                    size_t limit = std::min(i + 16, sz);
                    llvm::dbgs () << "  " << llvm::format_hex_no_prefix(i, 4) << ": ";
                    for (int j = i;  j < limit;  j++) {
                        llvm::dbgs() << " " << llvm::format_hex_no_prefix(bytes[j], 2);
                    }
                    llvm::dbgs () << "\n";
                }
	    }
	} break;
      case output::LLVMAsmFile:
        gContext->dumpLL(stem);
        break;
    }

    gContext->endModule();

} /* codegen */
