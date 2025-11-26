/// \file main.cpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief Main test driver for the code generator
///
/// \author John Reppy
///

#include "smlnj/config.h"

#include <string>
#include <iostream>
#include <cstdlib>

#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/Value.h"

#include "cfg.hpp"
#include "context.hpp"
#include "object-file.hpp"
#include "target-info.hpp"

#if defined(SMLNJ_ARCH_AMD64)
constexpr std::string_view kHostArch = "x86_64";
#elif defined(SMLNJ_ARCH_ARM64)
constexpr std::string_view kHostArch = "aarch64";
#else
#  error unknown architeture
#endif

/// different output targets
enum class Output {
    PrintAsm,           ///< print native assembly to stdout
    AsmFile,            ///< generate native assembly in a ".s" file
    ObjFile,            ///< generate an object (".o") file
    Memory,             ///< simulates the "in-memory" code generation used in the
                        ///  SML/NJ runtime, including patching relocation information
    LLVMAsmFile         ///> generate LLVM assembly in a ".ll" file
};


// set the target architecture.  This call returns `true` when there
// is an error and `false` otherwise.
//
bool setTarget (std::string_view target);

// generate code
void codegen (std::string const & src, bool emitLLVM, bool dumpBits, Output out);

[[noreturn]] void usage ()
{
    std::cerr << "usage: cfgc [ -o | -S | -c ] [ --emit-llvm ] [ --bits ] [ --target <target> ] <pkl-file>\n";
    std::cerr << "options:\n";
    std::cerr << "    -o                -- generate an object file\n";
    std::cerr << "    -S                -- emit target assembly code to a file\n";
    std::cerr << "    -c                -- use JIT compiler and loader to produce code object\n";
    std::cerr << "    -emit-llvm        -- emit generated LLVM assembly to standard output\n";
    std::cerr << "    -bits             -- output the code-object bits (implies \"-c\" flag)\n";
    std::cerr << "    -target <target>  -- specify the target architecture (default "
              << kHostArch << ")\n";
    exit (1);
}

int main (int argc, char **argv)
{
    Output out = Output::PrintAsm;
    bool emitLLVM = false;
    bool dumpBits = false;
    std::string src = "";
    std::string_view targetArch = kHostArch;

    std::vector<std::string_view> args(argv+1, argv+argc);

    if (args.empty()) {
	usage();
    }

    for (int i = 0;  i < args.size();  i++) {
	if (args[i][0] == '-') {
	    if (args[i] == "-o") {
		out = Output::ObjFile;
            } else if (args[i] == "-L") {
                out = Output::LLVMAsmFile;
	    } else if (args[i] == "-S") {
		out = Output::AsmFile;
	    } else if (args[i] == "-c") {
		out = Output::Memory;
	    } else if (args[i] == "-emit-llvm") {
		emitLLVM = true;
	    } else if (args[i] == "-bits") {
		dumpBits = true;
		out = Output::Memory;
	    } else if (args[i] == "-target") {
		i++;
		if (i < args.size()) {
		    targetArch = args[i];
		} else {
		    usage();
		}
	    } else {
		usage();
	    }
	}
	else if (i < args.size()-1) {
            usage();
	}
	else { // last argument
	    src = args[i];
	}
    }
    if (src.empty()) {
        usage();
    }

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    if (setTarget (targetArch)) {
	std::cerr << "codegen: unable to set target to \"" << targetArch << "\"\n";
	return 1;
    }

    codegen (src, emitLLVM, dumpBits, out);

    return 0;

}

/***** Code Generation *****/

//! points to a dynamically allocated code buffer; this pointer gets
//! reset if we change the target architecture.
//
static smlnj::cfgcg::Context *gContext = nullptr;

/// set the target machine
//
bool setTarget (std::string_view target)
{
    if (gContext != nullptr) {
	if (target.compare(gContext->targetInfo()->name) == 0) {
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

void codegen (std::string const & src, bool emitLLVM, bool dumpBits, Output out)
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
      case Output::PrintAsm:
	gContext->dumpAsm();
	break;
      case Output::AsmFile:
	gContext->dumpAsm (stem);
	break;
      case Output::ObjFile:
	gContext->dumpObj (stem);
	break;
      case Output::Memory: {
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
      case Output::LLVMAsmFile:
        gContext->dumpLL (stem);
        break;
    }

    gContext->endModule();

} /* codegen */
