/// \file lambda-var.hpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief The definitions of the primitive LambdaVar module from cfg.asdl
///
/// \author John Reppy
///

#ifndef __LAMBDA_VAR_HPP__
#define __LAMBDA_VAR_HPP__

#include "asdl/asdl.hpp"
#include <vector>

namespace LambdaVar {

    typedef int64_t lvar;

    lvar read_lvar (asdl::instream & is);
    std::vector<lvar> read_lvar_seq (asdl::instream & is);

} // namespace LambdaVar

#endif // !__LAMBDA_VAR_HPP__
