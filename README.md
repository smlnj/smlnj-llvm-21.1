# LLVM 21.1 with Support for JWA Calling Convention

This repository is a version of [**LLVM**](https://llvm.org) with support for the JWA
calling convention.  It is meant to be used as a submodule for the development
branch of [**Standard ML of New Jersey** (**SML/NJ**)](https://smlnj.org) and is based on **LLVM**
version 21.1.7.

## LLVM Components

The [**LLVM** Project](https://llvm.org) has many components; for our purposes, we
are only interested in the core libraries that support code generation.
There are three directories in this repository that are from the [LLVM 21.1.7 source
distribution](https://github.com/llvm/llvm-project/releases/tag/llvmorg-21.1.7):

1. [`cmake`](https://github.com/llvm/llvm-project/releases/download/llvmorg-21.1.7/cmake-21.1.7.src.tar.xz)
   &mdash; shared CMake modules used by the **LLVM** project
2. [`llvm`](https://github.com/llvm/llvm-project/releases/download/llvmorg-21.1.7/llvm-21.1.7.src.tar.xz)
   &mdash; the core code generation libraries
3. [`third-party`](https://github.com/llvm/llvm-project/releases/download/llvmorg-21.1.7/third-party-21.1.7.src.tar.xz)
   &mdash; third-party dependencies used in various components of **LLVM**

### Removed components

To reduce the size of the repository, we have removed the following unused (by SML/NJ)
components from the **LLVM** source trees:

* `llvm/benchmarks`
* `llvm/bindings`
* `llvm/examples`
* `llvm/test`
* `llvm/unittests`
* `third-party/benchmark`
* `third-party/unittest`
* `third-party/update_benchmark.sh`

## The Jump-with-Arguments Calling Convention

The jump-with-arguments (JWA) calling convention is a [**LLVM**](https://llvm.org) calling
convention designed to support a no-stack call-with-current-continuation
runtime model, such as used in the [**Manticore**](http://manticore.cs.uchicago.edu)
and the [**SML/NJ**](https://smlnj.org) systems.  This source tree is being used
as a submodule to support the **SML/NJ** system.

The important properties of the JWA runtime model are

* The program is in CPS form, so all calls and returns are represented as function
  calls.

* Continuation closures are heap allocated (or allocated in callee-save registers).

* All dynamic instances of functions share the same stack frame, which is allocated
  by the runtime system.  We use LLVM's `naked` attribute on functions to disable
  the allocation/deallocation of stack frames in the function prologue/epilogue code.

* All arguments are passed in hardware registers.

## Organzation

* `build-llvm.sh` &mdash; shell script for configuring and building the LLVM tools
  and libraries.

* `cfgc` &mdash; source code for a tool that compiles the **SML/NJ** CFG IR pickles
  to target code.  This tool is used to debug the **SML/NJ** backend; it is not
  part of the code generation pipeline.

* `cfgc/LICENSE` &mdash; the license for the code in the `cfgc` sub-directory.

* `cmake` &mdash; **LLVM**'s common CMake modules; see `cmake/README.rst` for details.

* `llvm` &mdash; the main **LLVM** source directory.

* `llvm/CMakePresets.json` &mdash; CMake presets that customize the configuration and
  build process.  This file is **not** part of the standard LLVM sources.

* `llvm/LICENSE.TXT` &mdash; the **LLVM** source code license, which covers the code in the
  `cmake` and `llvm` sub-directories.

* `LLVM-VERSION` &mdash; specifies the version of **LLVM** that this source tree is
  derived from (which is 21.1.7).

* `third-party` &mdash; third-party dependencies used in various components of **LLVM**.
  We only need the `siphash` header file from this tree, so the other components have
  been removed.

The `build-llvm.sh` script will produce several additional directories:

* `bin` &mdash; **LLVM** executables (*e.g.*, `llc`)

* `build` &mdash; the directory used to compile the LLM tools and libraries

* `include` &mdash; **LLVM** include files

* `lib` &mdash; **LLVM** libraries

## History

The original JWA convention was introduced in a 2016 ML Workshop paper.

> [**Compiling with continuations and LLVM**](https://people.cs.uchicago.edu/~jhr/papers/2016/ml-llvm.pdf),
> *Proceedings of the 2016 ACM SIGPLAN Workshop on ML*,
> by Kavon Farvardin and John Reppy, September 2016.

which describes using LLVM as a backend for the Manticore system.
A longer version of this paper appeared in
[Volume 285 of EPTCS](https://cgi.cse.unsw.edu.au/~eptcs/content.cgi?MLOCAML2016).

The use of this approach in the **SML/NJ** system is described in an IFL paper.

> [**A New Backend for Standard ML of New Jersey**](https://dl.acm.org/doi/10.1145/3462172.3462191),
> *Proceedings of the 32nd Symposium on Implementation and Application of
> Functional Languages (IFL '20)*, by Kavon Farvardin and John Reppy, September 2020.

Originally, the JWA calling convention required use of the `naked` function attribute
to disable the generation of function prologue and epilogue code.  We now make
the omission of function prologue and  epilogue code part of the implementation of
the JWA convention.

## Differences

The main difference from the original **LLVM** source tree is that we modify the
sources to support the JWA calling convention (details are given below).
In addition to these changes, we prune parts of the source tree that are not
relevant to its use in the **SML/NJ** system and we made a couple of small
changes to integrate with our use of CMake.

### Pruned Sources

As described above, we removed the following subdirectories from
the **LLVM** source tree.

``` bash
rm -rf benchmarks bindings examples test unittests
```

### CMake Changes

The **LLVM** project has many tools and components that we do not use
(*e.g.*, the components that we prune from the source).   Therefore, we
need to specify a large number of CMake variables to disable
these components.  We add the file `src/CMakePresets.json` and use CMake's
`--presets` option to simplify the configuration of the system.

## Patching LLVM

**LLVM** needs to be modified to support the **JWA** calling convention
that we use.  The modifications are fairly simple, and are
described for **LLVM** 21.1.x below, where `$LLVM` denotes the root of
the **LLVM** source tree.  The patch is organized into general changes to
add JWA as a calling convention and target-specific changes to support the
calling convention on specific targets.  We currently have patches to support the 64-bit
X86 and Arm targets.

There is some documentation about how to specify calling conventions in
["Writing an LLVM Backend."](https://llvm.org/docs/WritingAnLLVMBackend.html)

### Modified Files

The following is a list of the source files that we modify.  The details of
the patches are described below.

* `$LLVM/include/llvm/IR/CallingConv.h`
* `$LLVM/lib/CodeGen/PrologEpilogInserter.cpp`
* `$LLVM/include/llvm/AsmParser/LLToken.h` (optional)
* `$LLVM/lib/AsmParser/LLLexer.cpp` (optional)
* `$LLVM/lib/AsmParser/LLParser.cpp` (optional)
* `$LLVM/lib/IR/AsmWriter.cpp` (optional)
* `$LLVM/lib/Target/X86/X86CallingConv.td`
* `$LLVM/lib/Target/X86/X86FastISel.cpp`
* `$LLVM/lib/Target/X86/X86ISelLoweringCall.cpp`
* `$LLVM/lib/Target/X86/X86RegisterInfo.cpp`
* `$LLVM/lib/Target/AArch64/AArch64CallingConvention.td`
* `$LLVM/lib/Target/AArch64/AArch64CallingConvention.h`
* `$LLVM/lib/Target/AArch64/AArch64FastISel.cpp`
* `$LLVM/lib/Target/AArch64/AArch64RegisterInfo.cpp`
* `$LLVM/lib/Target/AArch64/AArch64ISelLowering.cpp`
* `$LLVM/lib/Target/AArch64/AArch64FrameLowering.cpp`

### `CallingConv.h`

The file `$LLVM/include/llvm/IR/CallingConv.h` assigns integer codes for
the various calling conventions.  In **LLVM** 19, the last number assigned
is `21` (for the `PreserveNone` convention), so we use `22` for **JWA**.
Add the following code to the file just before the first target-specific
code (which will be `64`).

``` c++
    /// JWA - "Jump With Arguments" is a calling convention that requires the
    /// use of registers for parameter passing. It is designed for language
    /// implementations that do not use a stack, however, it will not warn
    /// if there are not enough registers for a given function. The lack of
    /// warning is needed in order to properly utilize musttail calls as
    /// jumps because they are picky about parameters.
    JWA = 22,
```

### `PrologEpilogInserter.cpp`

The file `$LLVM/lib/CodeGen/PrologEpilogInserter.cpp` contains a function for
generating the function prologue and epilogue code, which is responsible for
allocating and deallocating the stack frame.  Since we use a single shared
frame for all functions, we disable this process.  We add the following code
to the beginning of the function `PEI::insertPrologEpilogCode`:

``` c++
  // In the JWA calling convention, functions have no prologue/epilogue.
  if (MF.getFunction().getCallingConv() == CallingConv::JWA)
    return;
```

### Supporting `jwacc` in LLVM Assembler

To support using the symbolic name `jwacc` for the calling convention
in LLVM assembler (useful for debugging the code generator, but not
required for the **SML/NJ** compiler), we must modify the parser and
pretty printer for LLVM assembly code.

#### `LLTToken.h`

Add `kw_jwacc` as a token in `$LLVM/include/llvm/AsmParser/LLToken.h`
(I added it following `kw_ghccc`).

#### `LLLexer.cpp`

Add the following keyword definition

``` c++
  KEYWORD(jwacc);
```

to the file `$LLVM/lib/AsmParser/LLLexer.cpp` (again, following `KEYWORD(ghccc);`).

#### `LLParser.cpp`

Add the following comment

``` c++
///   ::= 'jwacc'
```

and case

``` c++
  case lltok::kw_jwacc:          CC = CallingConv::JWA; break;
```

to the function `LLParser::ParseOptionalCallingConv`
in the file `$LLVM/lib/AsmParser/LLParser.cpp`.
I add these following the corresponding code for `ghccc`.

#### `AsmWriter.cpp`

The file `$LLVM/lib/IR/AsmWriter.cpp` implements printing of the
LLVM assembly code.  Modify it by adding the case

``` c++
  case CallingConv::JWA:           Out << "jwacc"; break;
```

to the function `PrintCallingConv`.

### `Target/X86`

To support the **JWA** convention on the `X86`, we need to modify a number
of files in the directory `$LLVM/lib/Target/X86/`.

#### `X86CallingConv.td`

The file `$LLVM/lib/Target/X86/X86CallingConv.td` describes the calling
conventions for the *x86* and *x86-64* (aka *amd64*) architectures
using **LLVM**'s [**TabgeGen**](https://llvm.org/docs/TableGen/) language.
We need to add several chunks of code to the file.  I added the first
just before the `Return Value Calling Conventions` section.

```
// The JWA calling convention for x86_64. Note that this is
// also used as the return convention in order to implement call/cc.
// True returns are not the norm. We _never_ use the stack.
def CC_X86_64_JWA : CallingConv<[

  // Promote i8/i16/i32 arguments to i64.
  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,

  // The only registers we skip are RAX and RSP.

  // registers are ordered according to SML/NJ convention as follows:
  // alloc, limit, store, link, clos, cont, misc0, ..., misc3, arg, ... misc6
  CCIfType<[i64],
    CCAssignToReg<[
        RDI, R14, R15,  // ALLOC, LIMIT, STORE
        R8, R9, RSI,    // LINK, CLOS, CONT
        RBX, RCX, RDX,  // MISC01-MISC2 (CALLEE SAVES)
        RBP, R10, R11,  // ARG, MISC3, MISC4,
        R12, R13        // MISC5, MISC6
    ]>>,

  // Use as many vector registers as possible!
  // NOTE: we are assuming that SSE is never disabled for JWA, since it
  // would break upstream assumptions in the SML/NJ compiler.
  CCIfType<[f32, f64, v16i8, v8i16, v4i32, v2i64, v4f32, v2f64],
    CCAssignToReg<[
      XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
      XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15]>>,

  // AVX (256-bit) vector registers
  CCIfType<[v32i8, v16i16, v8i32, v4i64, v8f32, v4f64],
    CCIfSubtarget<"hasAVX()",
      CCAssignToReg<[
	YMM0, YMM1, YMM2, YMM3, YMM4, YMM5, YMM6, YMM7,
	YMM8, YMM9, YMM10, YMM11, YMM12, YMM13, YMM14, YMM15]>>>,

  // AVX-512 (512-bit) vector registers
  CCIfType<[v64i8, v32i16, v16i32, v8i64, v16f32, v8f64],
    CCIfSubtarget<"hasAVX512()",
      CCAssignToReg<[
	ZMM0, ZMM1, ZMM2, ZMM3, ZMM4, ZMM5, ZMM6, ZMM7,
	ZMM8, ZMM9, ZMM10, ZMM11, ZMM12, ZMM13, ZMM14, ZMM15,
	ZMM16, ZMM17, ZMM18, ZMM19, ZMM20, ZMM21, ZMM22, ZMM23,
	ZMM24, ZMM25, ZMM26, ZMM27, ZMM28, ZMM29, ZMM30, ZMM31]>>>

]>;
```

The convention definition above specifies the decision procedure for assigning
each argument of a function, from left-to-right, based on the type of the
argument.  The first directive says to treat all `i8`/`i16`/`i32` values
as `i64`, since x86-64 general-purpose registers can handle any of these
values via their narrower aliases.  Then, the next directive says if the
argument can be treated as an i64, use the first available register from
the given list.  For calls that do not make use of, say, the standard
return-continuation register, an **LLVM** `undef` value can be passed in to
that argument position in the **LLVM** IR in order to effectively skip-over
that register during this convention decision procedure.

Because the JWA convention does not really return (instead, we use **JWA**
to invoke a return continuation), we delegate the return convention to
the calling convention.  The following lines need to be added to the
definition of the "root return-value convention for the X86-64 backend"
(`RetCC_X86_64`):

```
  // Handle JWA calls.
  CCIfCC<"CallingConv::JWA", CCDelegateTo<CC_X86_64_JWA>>,
```

The following line needs to be added to the definition of the
"root argument convention for the X86-64 backend" (`CC_X86_64`):

```
  CCIfCC<"CallingConv::JWA", CCDelegateTo<CC_X86_64_JWA>>,
```

#### `X86FastISel.cpp`

In the file `$LLVM/lib/Target/X86/X86FastISel.cpp`, the function
`computeBytesPoppedByCalleeForSRet` needs to be modified to
recognize the **JWA** convention.

``` c++
  if (CC == CallingConv::Fast || CC == CallingConv::GHC ||
      CC == CallingConv::HiPE || CC == CallingConv::Tail ||
      CC == CallingConv::SwiftTail || CC == CallingConv::JWA)
    return 0;
```

#### `X86ISelLowering.cpp`

In the file `$LLVM/lib/Target/X86/X86ISelLoweringCall.cpp`, we need to add
a check for **JWA** to the function `canGuaranteeTCO`.

``` c++
static bool canGuaranteeTCO(CallingConv::ID CC) {
  return (CC == CallingConv::Fast || CC == CallingConv::GHC ||
          CC == CallingConv::X86_RegCall || CC == CallingConv::HiPE ||
          CC == CallingConv::Tail || CC == CallingConv::SwiftTail ||
          CC == CallingConv::JWA);
}
```

There are a couple of assertions that check for incorrect use of the calling
conventions that return `true` for `canGuaranteeTCO`.  The assertion messages
for these can be modified to include mention of the JWA calling convention.
For example, in the function `X86TargetLowering::LowerFormalArguments`, change the
the assertion to
``` c++
  assert(
      !(IsVarArg && canGuaranteeTCO(CallConv)) &&
      "Var args not supported with calling conv' regcall, fastcc, ghc, jwa, or hipe");
```

#### `X86RegisterInfo.cpp`

In the file `$LLVM/lib/Target/X86/X86RegisterInfo.cpp`, we need to add
cases for **JWA** to the method `getCalleeSavedRegs`:
``` c++
  case CallingConv::GHC:
  case CallingConv::HiPE:
  case CallingConv::JWA:
    return CSR_NoRegs_SaveList;
```
and to the method `getCallPreservedMask`
``` c++
  case CallingConv::GHC:
  case CallingConv::HiPE:
  case CallingConv::JWA:
    return CSR_NoRegs_RegMask;
```

### `Target/AArch64`

The basic approach to supporting the **AArch64** (aka **arm64**) target is similar
to the **X86**, but the details are different.  Both because of differences in
the number of target registers and because the **LLVM** code for the ``AArch64``
is writting in a different style (*e.g.*, conditionals instead of `switch`
statements for testing calling conventions).  As before, we need to modify a number
of files in the directory `$LLVM/lib/Target/AArch64/`.

#### `AArch64CallingConvention.td`

At the end of the file `$LLVM/lib/Target/AArch64/AArch64CallingConvention.td`,
add the following code:

```
//===----------------------------------------------------------------------===//
// JWA Calling Convention
//===----------------------------------------------------------------------===//

// The "Jump With Arguments" calling convention is designed to support the
// "continuation-passing, closure-passing" model used by SML/NJ and the Manticore
// compiler.  The convention is described in the paper "Compiling with Continuations
// and LLVM" (https://arxiv.org/abs/1805.08842v1).
//
let Entry = 1 in
def CC_AArch64_JWA : CallingConv<[
  CCIfType<[iPTR], CCBitConvertToType<i64>>,
  CCIfType<[i1, i8, i16, i32], CCPromoteToType<i64>>,
  //
  // SML/NJ argument order:
  //	alloc, limit, store, exn, var, link, clos, cont,
  //    misc0, ..., misc3, arg, misc4, .., misc17
  CCIfType<[i64],
    CCAssignToRegWithShadow<[
	X24, X25, X26, X27, X28,	// ALLOC, LIMIT, STORE, EXN, VAR
	X3, X2, X1,			// LINK, CLOS, CONT
	X4, X5, X6,			// MISC0-MISC2 (aka, CS0-CS2)
	X0, X7, X8, X9, X10, X11, X12,	// ARG, MISC3-MISC8
	X13, X14, X15, X16, X19, X20,	// MISC9-MISC14
	X21, X22, X23			// MISC15-MISC17
      ],
      [W24, W25, W26, W27, W28,	W3, W2, W1, W4, W5, W6,	W0, W7, W8, W9, W10, W11, W12,
	W13, W14, W15, W16, W19, W20, W21, W22, W23]>>,
  // there are 32 float/vector registers, but we only make 16 of them available
  // for parameter passing
  CCIfType<[f32], CCAssignToRegWithShadow<
    [S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15],
    [Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, Q10, Q11, Q12, Q13, Q14, Q15]>>,
  CCIfType<[f64], CCAssignToRegWithShadow<
    [D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15],
    [Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, Q10, Q11, Q12, Q13, Q14, Q15]>>
]>;

// use the same convention for returns
let Entry = 1 in
def RetCC_AArch64_JWA : CallingConv<[
  CCDelegateTo<CC_AArch64_JWA>
]>;
```

Note that the `let Entry = 1` is necessary to make the function visible in the `llvm`
namespace; otherwise it will be marked as a `static` function.

#### `AArch64CallingConvention.h`

To the file `$LLVM/lib/Target/AArch64/AArch64CallingConvention.h`
add the following function prototypes:

``` c++
bool CC_AArch64_JWA(unsigned ValNo, MVT ValVT, MVT LocVT,
                    CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                    CCState &State);
bool RetCC_AArch64_JWA(unsigned ValNo, MVT ValVT, MVT LocVT,
                         CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                         CCState &State);
```

#### `AArch64FastISel.cpp`

In the file `$LLVM/lib/Target/AArch64/AArch64FastISel.cpp`, we make
the following changes.
In the method `AArch64FastISel::CCAssignFnForCall`, add the
following statement before the final return:

``` c++
  if (CC == CallingConv::JWA)
    return CC_AArch64_JWA;
```

And in the function `AArch64FastISel::selectRet`, replace the statement
``` c++
    CCInfo.AnalyzeReturn(Outs, RetCC_AArch64_AAPCS);
```
with
``` c++
    if (CC == CallingConv::JWA) {
        CCInfo.AnalyzeReturn(Outs, RetCC_AArch64_JWA);
    } else {
        CCInfo.AnalyzeReturn(Outs, RetCC_AArch64_AAPCS);
    }
```

#### `AArch64RegisterInfo.cpp`

In the file `$LLVM/lib/Target/AArch64/AArch64RegisterInfo.cpp`, we make
several additions.

In the method `AArch64RegisterInfo::getCalleeSavedRegs`,
add the following test following the similar code for the `GHC` convention.

``` c++
  if (MF->getFunction().getCallingConv() == CallingConv::JWA)
    // no callee-saves for JWA
    return CSR_AArch64_NoRegs_SaveList;
```

**NOTE**: it might be better to merge the `GHC` and `JWA` cases into a single
conditional.

In `AArch64RegisterInfo::getCallPreservedMask` method, change the `GHC` test to
the following statement:
``` c++
  if ((CC == CallingConv::GHC) || (CC == CallingConv::JWA))
    // This is academic because all GHC/JWA calls are (supposed to be) tail calls
    return SCS ? CSR_AArch64_NoRegs_SCS_RegMask : CSR_AArch64_NoRegs_RegMask;
```

Add the following assertion to the `AArch64RegisterInfo::getThisReturnPreservedMask`
method:

``` c++
  assert(CC != CallingConv::JWA && "should not be JWA calling convention.");
```

#### `AArch64ISelLowering.cpp`

There are several changes to the `$LLVM/lib/Target/AArch64/AArch64ISelLowering.cpp`
file.  In the method `AArch64TargetLowering::CCAssignFnForCall`, add the case
``` c++
  case CallingConv::JWA:
    return CC_AArch64_JWA;
```

Add the following case to the `switch` statement in
`AArch64TargetLowering::CCAssignFnForReturn`:
``` c++
  case CallingConv::JWA:
    return RetCC_AArch64_JWA;
```

Lastly, change the function `canGuaranteeTCO` to the following:
``` c++
static bool canGuaranteeTCO(CallingConv::ID CC, bool GuaranteeTailCalls) {
  return (CC == CallingConv::Fast && GuaranteeTailCalls) ||
         CC == CallingConv::Tail || CC == CallingConv::SwiftTail ||
         CC == CallingConv::JWA;
}
```

#### `AArch64FrameLowering.cpp`

In `$LLVM/lib/Target/AArch64/AArch64FrameLowering.cpp`, we add the following statement

``` c++
  // All calls are tail calls in JWA calling conv, and functions have no
  // prologue/epilogue.
  if (MF.getFunction().getCallingConv() == CallingConv::JWA)
    return;
```

in three places (following the similar code for the `GHC` calling convention):

* in method `AArch64FrameLowering::emitPrologue`

* in method `AArch64FrameLowering::emitEpilogue`

* in method `AArch64FrameLowering::determineCalleeSaves`
