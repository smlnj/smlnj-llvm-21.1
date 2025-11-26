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

#include "heap2obj.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

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

/// utility function to check if the leading character of a string_view is '-'
inline bool isCommandLineSwitch (std::string_view s)
{
/* TODO: customize for windows? */
    return (! s.empty()) && (s[0] == '-');
}

int main (int argc, char const **argv)
{
    auto outMode = smlnj::Output::eObjFile;
    std::string heapFile = "";
    std::string outFile = "";

    // process command-line arguments
    std::vector<std::string_view> args(argv+1, argv+argc);

    if (args.empty()) {
	usage();
    }

    for (auto arg = args.cbegin();  arg != args.cend();  ++arg) {
        if (isCommandLineSwitch(*arg)) {
            if (arg->compare("-h") == 0) {
                usage(EXIT_SUCCESS);
            } else if (arg->compare("-o") == 0) {
                if (++arg != args.cend()) {
                    if (isCommandLineSwitch(*arg)) {
                        usage(); // missing output file name (switch instead)
                    }
                    outFile = std::string(*arg);
                } else {
                    usage(); // missing output file
                }
            } else if (arg->compare("-S") == 0) {
                outMode = smlnj::Output::eAsmFile;
            } else if (arg->compare("-emit-llvm") == 0) {
                outMode = smlnj::Output::eLLVMAsmFile;
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
    auto pos = heapFile.find_last_of('.');
    std::string baseName = heapFile.substr(0, pos);

    if (outFile.empty()) {
        // generate the output file name by replacing the heap suffix
        // with the output-file suffix
        switch (outMode) {
        case smlnj::Output::eObjFile:
            outFile = baseName + OBJECT_FILE_EXTENSION;
            break;
        case smlnj::Output::eAsmFile:
            outFile = baseName + ".s";
            break;
        case smlnj::Output::eLLVMAsmFile:
            outFile = baseName + ".ll";
            break;
        }
    }

    // load the heap-image data
    std::error_code ec;
    fs::path srcPath = heapFile;
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

    auto sts = smlnj::emitHeapFile(outFile, heapData, outMode);
    if (sts == smlnj::Status::eFailure) {
        /* TODO: error message */
        return 1;
    } else {
        return 0;
    }

}
