/// \file main.cpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief The heap2obj tool converts a SML/NJ heap-image file to a host
///        object file that can be linked against the runtime system.
///
/// \author John Reppy
///

#include <string_view>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/LegacyPassManager.h" /* needed for code gen */
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"

constexpr std::string_view kTriple = SMLNJ_TARGET_TRIPLE;

/// different output targets
enum class Output {
    ObjFile,            ///< generate an object (".o") file
    AsmFile,            ///< generate native assembly in a ".s" file
    LLVMAsmFile         ///> generate LLVM assembly in a ".ll" file
};

// convert an integer to the tagged SML representation
//
inline uint64_t toSML (uint64_t n) { return n+n+1; }

[[noreturn]] void usage (int status = EXIT_FAILURE)
{
    std::cerr << "usage: heap2objfile [ options ] <heap-file>\n";
    std::cerr << "options:\n";
    std::cerr << "    -o <obj-file>     -- specify the name of the output file\n";
    std::cerr << "    -S                -- emit assembly code to the output file\n";
    std::cerr << "    -emit-llvm        -- emit generated LLVM assembly to the output file\n";
    exit (status);
}

void emitFile (
    llvm::Module &module,
    llvm::TargetMachine *tgtMachine,
    std::string const &outFile,
    llvm::CodeGenFileType fileType)
{
    std::error_code ec;
    llvm::raw_fd_ostream outStrm(outFile, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "unable to open output file '" << outFile << "'\n";
        return;
    }

    llvm::legacy::PassManager pass;
    if (tgtMachine->addPassesToEmitFile(pass, outStrm, nullptr, fileType)) {
        std::cerr << "unable to add pass to generate '" << outFile << "'\n";
        return;
    }

    pass.run(module);

    outStrm.flush();

}

/// utility function to check if the leading character of a string_view is '-'
inline bool isCommandLineSwitch (std::string_view s)
{
/* TODO: customize for windows? */
    return (! s.empty() && (s[0] == '-');
}

int main (int argc, char const **argv)
{
    Output out = Output::ObjFile;
    std::string_view heapFile = "";
    std::string stem;
    std::string_view outFile = "";

    // process command-line arguments
    std::vector<std::string_view> args(argv+1, argv+argc);

    if (args.empty()) {
	usage();
    }

    for (auto arg = args.cbegin();  arg != args.cend();  ++arg) {
        if (isCommandLineSwitch(*arg)) {
            arg->remove_prefix(1); // skip the '-'
            if (arg->compare("h") == 0) {
                usage(EXIT_SUCCESS);
            } else if (arg->compare("o") == 0)) {
                if (++arg != args.cend()) {
                    if (commandLineSwitch(*arg) {
                        usage(); // missing output file name (switch instead)
                    }
                    outFile = std::string(*arg);
                } else {
                    usage(); // missing output file
                }
            } else if (arg->compare("S") == 0)) {
                out = Output::AsmFile;
            } else if (arg->compare("emit-llvm") == 0)) {
                out = Output::LLVMAsmFile;
            } else {
                usage(); // invalid option
            }
        } else if (heapFile.empty()) {
            heapFile = std::string(*arg);
        } else {
            usage(); // multiple input files
        }
    }
    if (heapFile.empty()) {
        usage();
    }

    // process source file name to remove suffix
    std::string baseName = smlnj::stripHeapSuffix(heapFile);
    if (outFile.empty()) {
        // generate the output file name by replacing the heap suffix
        // with the output-file suffix
        switch (outMode) {
        case kObjFile:
            outFile = baseName + OBJECT_FILE_EXTENSION;
            break;
        case kLLVM:
            outFile = baseName + ".ll";
            break;
        }
    }

    // load the heap-image data
    std::error_code ec;
    fs::path srcPath = src;
    auto srcSts = fs::status(srcPath, ec);
    if (ec) {
        /* TODO: error */
    }
    auto fileSzb = fs::file_size(srcPath, ec);
    if (ec) {
        /* TODO: error */
    }
    uint64_t nBytes = (fileSzb + 7) & ~7; // round to 64-byte alignment
    std::vector<char> heapData = std::vector<char>(nBytes, 0);
    {
        std::ifstream srcS(srcPath, std::ios_base::in | std::ios_base::binary);
        srcS.read(heapData.data(), fileSzb);
        if (srcS.gcount() != fileSzb) {
            /* TODO: error */
        }
        srcS.close();
    }

    // create the LLVM context
    llvm::LLVMContext cxt;

    // determine the target machine
    llvm::Triple triple(kTriple);

    // lookup the target in the registry using the triple's string representation
    std::string errMsg;
    auto *target = llvm::TargetRegistry::lookupTarget(triple.str(), errMsg);
    if (target == nullptr) {
	std::cerr << "**** Fatal error: unable to find target for \""
	    << triple.str() << "\"\n";
	std::cerr << "    [" << errMsg << "]\n";
        ::exit(1);
    }
    llvm::TargetOptions tgtOptions;
    llvm::TargetMachine *tgtMachine = target->createTargetMachine(
	triple.str(),
	"generic",		/* name of CPU variant */
	"",			/* features string */
	tgtOptions,
	llvm::Reloc::PIC_,
	std::optional<llvm::CodeModel::Model>(),
	llvm::CodeGenOptLevel::Less);

    if (tgtMachine == nullptr) {
	std::cerr << "**** Fatal error: unable to create target machine\n";
        assert(false);
    }

    // get the integer types that we need
    auto i8Ty = llvm::IntegerType::get (cxt, 8);
    auto i64Ty = llvm::IntegerType::get (cxt, 64);
    auto heapTy = llvm::StructType::create (cxt, {
            i64Ty,
            llvm::ArrayType::get(i8Ty, nBytes)
        });

    llvm::Module module(src, cxt);

    // tell the module about the target machine
    module.setTargetTriple(tgtMachine->getTargetTriple().getTriple());
    module.setDataLayout(tgtMachine->createDataLayout());

    // create the constants
    llvm::Constant *mSize = llvm::ConstantInt::get(i64Ty, toSML(nBytes));
    llvm::Constant *mData = llvm::ConstantDataArray::getRaw(
        llvm::StringRef(reinterpret_cast<char *>(heapData.data()), nBytes),
        nBytes,
        i8Ty);
    llvm::Constant *heapStruct = llvm::ConstantStruct::get (
        heapTy,
        { mSize, mData });

    auto heapV = new llvm::GlobalVariable (
        module,
        heapTy,
        true,
        llvm::GlobalValue::ExternalLinkage,
        heapStruct,
        "_sml_heap_image");

    // output the object file
    switch (out) {
    case Output::AsmFile: {
            std::string outFile = stem + ".s";
            emitFile (module, tgtMachine, outFile, llvm::CodeGenFileType::AssemblyFile);
        } break;
    case Output::LLVMAsmFile: {
            std::string outFile = std::string(stem) + ".ll";
            llvm::raw_fd_ostream outS(outFile, ec, llvm::sys::fs::OF_Text);
            if (ec) {
                /* TODO: error */
            }
            module.print(outS, nullptr);
            outS.close();
        } break;
    case Output::ObjFile: {
// FIXME: on Windows, the extension should be ".obj"
            std::string outFile = stem + ".o";
            emitFile (module, tgtMachine, outFile, llvm::CodeGenFileType::ObjectFile);
        } break;
    } // switch

    // cleanup
    delete tgtMachine;
}
