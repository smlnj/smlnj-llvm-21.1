/// \file heap2obj.cpp
///
/// \copyright 2025 The Fellowship of SML/NJ (https://www.smlnj.org)
/// All rights reserved.
///
/// \brief The implementation of the heap-to-object-file library.  This
///        library uses the LLVM infrastructure to generate a host-native
///        object file that contains the contents of an exported SML/NJ
///        heap-image.
///
/// \author John Reppy
///

#include "smlnj/config.h"
#include "heap2obj.hpp"
#include <memory>
#include <filesystem>
#include <iostream>

#include "llvm-c/Core.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"

namespace smlnj {

// convert an integer to the tagged SML representation
//
inline uint64_t toSML (uint64_t n) { return n+n+1; }

Status emitHeapFile (
    std::string_view outFile,
    std::vector<char> const &heap,
    Output fileType,
    std::string_view heapFile)
{
    // LLVM initialization
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();

    // set up the LLVM context, etc
    llvm::LLVMContext llvmCxt;

    // determine the target info
    auto llvmTripleStr = llvm::Triple::normalize(llvm::sys::getDefaultTargetTriple());
    auto llvmTriple = llvm::Triple(llvmTripleStr);
    // lookup the target in the registry using the triple's string representation
    std::string errMsg;
    auto *target = llvm::TargetRegistry::lookupTarget(llvmTripleStr, errMsg);
    if (target == nullptr) {
        std::cerr << "[FATAL ERROR] unable to find target for '"
            << llvmTripleStr << "'\n"
            << "    [" << errMsg << "]\n";
        return Status::eFailure;
    }
    llvm::TargetOptions tgtOptions{};
    llvm::TargetMachine *tgtMachine = target->createTargetMachine(
	llvmTripleStr,
	"generic",		/* name of CPU variant */
	"",			/* features string */
	tgtOptions,
	llvm::Reloc::PIC_,
	std::optional<llvm::CodeModel::Model>(),
	llvm::CodeGenOptLevel::Less);

    if (tgtMachine == nullptr) {
        std::cerr << "[FATAL ERROR] unable to create target machine\n";
        return Status::eFailure;
    }

    // get the integer types that we need
    auto i8Ty = llvm::IntegerType::get (llvmCxt, 8);
    auto i64Ty = llvm::IntegerType::get (llvmCxt, 64);
    auto heapTy = llvm::StructType::create (llvmCxt, {
            i64Ty,
            llvm::ArrayType::get(i8Ty, heap.size())
        });

    // create the module
    llvm::Module llvmModule(heapFile.empty() ? "heap-image" : heapFile, llvmCxt);
    if (! heapFile.empty()) {
        llvmModule.setSourceFileName(heapFile);
    }

    // tell the module about the target machine
    llvmModule.setTargetTriple(tgtMachine->getTargetTriple().getTriple());
    llvmModule.setDataLayout(tgtMachine->createDataLayout());

    // create the constants
    llvm::Constant *mSize = llvm::ConstantInt::get(i64Ty, toSML(heap.size()));
    llvm::Constant *mData = llvm::ConstantDataArray::getRaw(
        llvm::StringRef(heap.data(), heap.size()),
        heap.size(),
        i8Ty);
    llvm::Constant *heapStruct = llvm::ConstantStruct::get (
        heapTy,
        { mSize, mData });

    auto heapV = new llvm::GlobalVariable (
        llvmModule,
        heapTy,
        true,
        llvm::GlobalValue::ExternalLinkage,
        heapStruct,
        "smlnj_heap_image");

    if (fileType == Output::eLLVMAsmFile) {
        // LLVM output
        std::error_code EC;
        llvm::raw_fd_ostream llOS(outFile, EC, llvm::sys::fs::OF_Text);
        if (EC) {
            std::cerr << "[FATAL ERROR] unable to open output file '"
                << outFile << "'\n";
            return Status::eFailure;
        }
        llvmModule.print(llOS, nullptr);
        llOS.close();
    } else {
        // Object/assembly file output

        // open the output object file
        std::error_code EC;
        llvm::raw_fd_ostream objOS(outFile, EC, llvm::sys::fs::OF_None);
        if (EC) {
            std::cerr << "[FATAL ERROR] unable to open output file '"
                << outFile << "'\n";
            return Status::eFailure;
        }

/* FIXME: assembly output does not work */
        // define a pass for output to the object file
        llvm::legacy::PassManager pass;
        auto cgFileType = (fileType == Output::eObjFile
                ? llvm::CodeGenFileType::ObjectFile
                : llvm::CodeGenFileType::AssemblyFile);
        if (tgtMachine->addPassesToEmitFile(pass, objOS, nullptr, cgFileType)) {
            std::cerr << "[FATAL ERROR] unable to add pass to generate '"
                << outFile << "'\n";
            return Status::eFailure;
        }

        pass.run(llvmModule);

        objOS.close();
    }

    return Status::eSuccess;

} // emitHeapFile

} // namespace smlnj
