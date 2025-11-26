// cfg.cpp
//
// Generated from cfg.asdl by asdlgen.
//

#include "cfg.hpp"

namespace CTypes {
    c_type * c_type::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_C_void:
            return C_void::make();
          case _con_C_float:
            return C_float::make();
          case _con_C_double:
            return C_double::make();
          case _con_C_long_double:
            return C_long_double::make();
          case _con_C_unsigned:
            {
                auto f0 = read_c_int(is);
                return C_unsigned::make(f0);
            }
          case _con_C_signed:
            {
                auto f0 = read_c_int(is);
                return C_signed::make(f0);
            }
          case _con_C_PTR:
            return C_PTR::make();
          case _con_C_ARRAY:
            {
                auto f0 = c_type::read(is);
                auto f1 = asdl::read_int(is);
                return C_ARRAY::make(f0, f1);
            }
          case _con_C_STRUCT:
            {
                auto f0 = read_c_type_seq(is);
                return C_STRUCT::make(f0);
            }
          case _con_C_UNION:
            {
                auto f0 = read_c_type_seq(is);
                return C_UNION::make(f0);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "c_type");
        }
    }
    c_type::~c_type () { }
    C_void::~C_void () { }
    C_float::~C_float () { }
    C_double::~C_double () { }
    C_long_double::~C_long_double () { }
    C_unsigned::~C_unsigned () { }
    C_signed::~C_signed () { }
    C_PTR::~C_PTR () { }
    C_ARRAY::~C_ARRAY ()
    {
        delete this->_v0;
    }
    C_STRUCT::~C_STRUCT ()
    {
        for (auto item : this->_v0) {
            delete item;
        }
    }
    C_UNION::~C_UNION ()
    {
        for (auto item : this->_v0) {
            delete item;
        }
    }
    // c_type_seq pickler suppressed
    std::vector<c_type *> read_c_type_seq (asdl::instream & is)
    {
        return asdl::read_seq<c_type>(is);
    }
    // pickler suppressed for c_int
    c_int read_c_int (asdl::instream & is)
    {
        return static_cast<c_int>(asdl::read_tag8(is));
    }
    // pickler suppressed for calling_convention
    calling_convention read_calling_convention (asdl::instream & is)
    {
        auto v = asdl::read_string(is);
        return v;
    }
    c_proto * c_proto::read (asdl::instream & is)
    {
        auto fconv = read_calling_convention(is);
        auto fretTy = c_type::read(is);
        auto fparamTys = read_c_type_seq(is);
        return c_proto::make(fconv, fretTy, fparamTys);
    }
    c_proto::~c_proto ()
    {
        delete this->_v_retTy;
        for (auto item : this->_v_paramTys) {
            delete item;
        }
    }
} // namespace CTypes
namespace CFG_Prim {
    // pickler suppressed for numkind
    numkind read_numkind (asdl::instream & is)
    {
        return static_cast<numkind>(asdl::read_tag8(is));
    }
    // pickler suppressed for rounding_mode
    rounding_mode read_rounding_mode (asdl::instream & is)
    {
        return static_cast<rounding_mode>(asdl::read_tag8(is));
    }
    raw_ty * raw_ty::read (asdl::instream & is)
    {
        auto fkind = read_numkind(is);
        auto fsz = asdl::read_int(is);
        return raw_ty::make(fkind, fsz);
    }
    raw_ty::~raw_ty () { }
    // raw_ty_seq pickler suppressed
    std::vector<raw_ty *> read_raw_ty_seq (asdl::instream & is)
    {
        return asdl::read_seq<raw_ty>(is);
    }
    alloc * alloc::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_SPECIAL:
            return SPECIAL::make();
          case _con_RECORD:
            {
                auto fdesc = asdl::read_integer(is);
                auto fmut = asdl::read_bool(is);
                return RECORD::make(fdesc, fmut);
            }
          case _con_RAW_RECORD:
            {
                auto fdesc = asdl::read_integer(is);
                auto falign = asdl::read_int(is);
                auto ffields = read_raw_ty_seq(is);
                return RAW_RECORD::make(fdesc, falign, ffields);
            }
          case _con_RAW_ALLOC:
            {
                auto fdesc = asdl::read_integer_option(is);
                auto falign = asdl::read_int(is);
                auto flen = asdl::read_int(is);
                return RAW_ALLOC::make(fdesc, falign, flen);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "alloc");
        }
    }
    alloc::~alloc () { }
    SPECIAL::~SPECIAL () { }
    RECORD::~RECORD () { }
    RAW_RECORD::~RAW_RECORD ()
    {
        for (auto item : this->_v_fields) {
            delete item;
        }
    }
    RAW_ALLOC::~RAW_ALLOC () { }
    // pickler suppressed for arithop
    arithop read_arithop (asdl::instream & is)
    {
        return static_cast<arithop>(asdl::read_tag8(is));
    }
    arith * arith::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_ARITH:
            {
                auto foper = read_arithop(is);
                auto fsz = asdl::read_int(is);
                return ARITH::make(foper, fsz);
            }
          case _con_FLOAT_TO_INT:
            {
                auto fmode = read_rounding_mode(is);
                auto ffrom = asdl::read_int(is);
                auto fto = asdl::read_int(is);
                return FLOAT_TO_INT::make(fmode, ffrom, fto);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "arith");
        }
    }
    arith::~arith () { }
    ARITH::~ARITH () { }
    FLOAT_TO_INT::~FLOAT_TO_INT () { }
    // pickler suppressed for pureop
    pureop read_pureop (asdl::instream & is)
    {
        return static_cast<pureop>(asdl::read_tag8(is));
    }
    pure * pure::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_PURE_ARITH:
            {
                auto foper = read_pureop(is);
                auto fsz = asdl::read_int(is);
                return PURE_ARITH::make(foper, fsz);
            }
          case _con_EXTEND:
            {
                auto fsigned = asdl::read_bool(is);
                auto ffrom = asdl::read_int(is);
                auto fto = asdl::read_int(is);
                return EXTEND::make(fsigned, ffrom, fto);
            }
          case _con_TRUNC:
            {
                auto ffrom = asdl::read_int(is);
                auto fto = asdl::read_int(is);
                return TRUNC::make(ffrom, fto);
            }
          case _con_INT_TO_FLOAT:
            {
                auto ffrom = asdl::read_int(is);
                auto fto = asdl::read_int(is);
                return INT_TO_FLOAT::make(ffrom, fto);
            }
          case _con_FLOAT_TO_BITS:
            {
                auto fsz = asdl::read_int(is);
                return FLOAT_TO_BITS::make(fsz);
            }
          case _con_BITS_TO_FLOAT:
            {
                auto fsz = asdl::read_int(is);
                return BITS_TO_FLOAT::make(fsz);
            }
          case _con_PURE_SUBSCRIPT:
            return PURE_SUBSCRIPT::make();
          case _con_PURE_RAW_SUBSCRIPT:
            {
                auto fkind = read_numkind(is);
                auto fsz = asdl::read_int(is);
                return PURE_RAW_SUBSCRIPT::make(fkind, fsz);
            }
          case _con_RAW_SELECT:
            {
                auto fkind = read_numkind(is);
                auto fsz = asdl::read_int(is);
                auto foffset = asdl::read_int(is);
                return RAW_SELECT::make(fkind, fsz, foffset);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "pure");
        }
    }
    pure::~pure () { }
    PURE_ARITH::~PURE_ARITH () { }
    EXTEND::~EXTEND () { }
    TRUNC::~TRUNC () { }
    INT_TO_FLOAT::~INT_TO_FLOAT () { }
    FLOAT_TO_BITS::~FLOAT_TO_BITS () { }
    BITS_TO_FLOAT::~BITS_TO_FLOAT () { }
    PURE_SUBSCRIPT::~PURE_SUBSCRIPT () { }
    PURE_RAW_SUBSCRIPT::~PURE_RAW_SUBSCRIPT () { }
    RAW_SELECT::~RAW_SELECT () { }
    looker * looker::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_DEREF:
            return DEREF::make();
          case _con_SUBSCRIPT:
            return SUBSCRIPT::make();
          case _con_RAW_SUBSCRIPT:
            {
                auto fkind = read_numkind(is);
                auto fsz = asdl::read_int(is);
                return RAW_SUBSCRIPT::make(fkind, fsz);
            }
          case _con_RAW_LOAD:
            {
                auto fkind = read_numkind(is);
                auto fsz = asdl::read_int(is);
                return RAW_LOAD::make(fkind, fsz);
            }
          case _con_GET_HDLR:
            return GET_HDLR::make();
          case _con_GET_VAR:
            return GET_VAR::make();
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "looker");
        }
    }
    looker::~looker () { }
    DEREF::~DEREF () { }
    SUBSCRIPT::~SUBSCRIPT () { }
    RAW_SUBSCRIPT::~RAW_SUBSCRIPT () { }
    RAW_LOAD::~RAW_LOAD () { }
    GET_HDLR::~GET_HDLR () { }
    GET_VAR::~GET_VAR () { }
    setter * setter::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_UNBOXED_UPDATE:
            return UNBOXED_UPDATE::make();
          case _con_UPDATE:
            return UPDATE::make();
          case _con_UNBOXED_ASSIGN:
            return UNBOXED_ASSIGN::make();
          case _con_ASSIGN:
            return ASSIGN::make();
          case _con_RAW_UPDATE:
            {
                auto fkind = read_numkind(is);
                auto fsz = asdl::read_int(is);
                return RAW_UPDATE::make(fkind, fsz);
            }
          case _con_RAW_STORE:
            {
                auto fkind = read_numkind(is);
                auto fsz = asdl::read_int(is);
                return RAW_STORE::make(fkind, fsz);
            }
          case _con_SET_HDLR:
            return SET_HDLR::make();
          case _con_SET_VAR:
            return SET_VAR::make();
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "setter");
        }
    }
    setter::~setter () { }
    UNBOXED_UPDATE::~UNBOXED_UPDATE () { }
    UPDATE::~UPDATE () { }
    UNBOXED_ASSIGN::~UNBOXED_ASSIGN () { }
    ASSIGN::~ASSIGN () { }
    RAW_UPDATE::~RAW_UPDATE () { }
    RAW_STORE::~RAW_STORE () { }
    SET_HDLR::~SET_HDLR () { }
    SET_VAR::~SET_VAR () { }
    // pickler suppressed for cmpop
    cmpop read_cmpop (asdl::instream & is)
    {
        return static_cast<cmpop>(asdl::read_tag8(is));
    }
    // pickler suppressed for fcmpop
    fcmpop read_fcmpop (asdl::instream & is)
    {
        return static_cast<fcmpop>(asdl::read_tag8(is));
    }
    branch * branch::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_CMP:
            {
                auto foper = read_cmpop(is);
                auto fsigned = asdl::read_bool(is);
                auto fsz = asdl::read_int(is);
                return CMP::make(foper, fsigned, fsz);
            }
          case _con_FCMP:
            {
                auto foper = read_fcmpop(is);
                auto fsz = asdl::read_int(is);
                return FCMP::make(foper, fsz);
            }
          case _con_FSGN:
            {
                auto f0 = asdl::read_int(is);
                return FSGN::make(f0);
            }
          case _con_PEQL:
            return PEQL::make();
          case _con_PNEQ:
            return PNEQ::make();
          case _con_LIMIT:
            {
                auto f0 = asdl::read_uint(is);
                return LIMIT::make(f0);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "branch");
        }
    }
    branch::~branch () { }
    CMP::~CMP () { }
    FCMP::~FCMP () { }
    FSGN::~FSGN () { }
    PEQL::~PEQL () { }
    PNEQ::~PNEQ () { }
    LIMIT::~LIMIT () { }
} // namespace CFG_Prim
namespace CFG {
    ty * ty::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_LABt:
            return LABt::make();
          case _con_PTRt:
            return PTRt::make();
          case _con_TAGt:
            return TAGt::make();
          case _con_NUMt:
            {
                auto fsz = asdl::read_int(is);
                return NUMt::make(fsz);
            }
          case _con_FLTt:
            {
                auto fsz = asdl::read_int(is);
                return FLTt::make(fsz);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "ty");
        }
    }
    ty::~ty () { }
    LABt::~LABt () { }
    PTRt::~PTRt () { }
    TAGt::~TAGt () { }
    NUMt::~NUMt () { }
    FLTt::~FLTt () { }
    // ty_seq pickler suppressed
    std::vector<ty *> read_ty_seq (asdl::instream & is)
    {
        return asdl::read_seq<ty>(is);
    }
    exp * exp::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_VAR:
            {
                auto fname = LambdaVar::read_lvar(is);
                return VAR::make(fname);
            }
          case _con_LABEL:
            {
                auto fname = LambdaVar::read_lvar(is);
                return LABEL::make(fname);
            }
          case _con_NUM:
            {
                auto fiv = asdl::read_integer(is);
                auto fsz = asdl::read_int(is);
                return NUM::make(fiv, fsz);
            }
          case _con_LOOKER:
            {
                auto foper = CFG_Prim::looker::read(is);
                auto fargs = read_exp_seq(is);
                return LOOKER::make(foper, fargs);
            }
          case _con_PURE:
            {
                auto foper = CFG_Prim::pure::read(is);
                auto fargs = read_exp_seq(is);
                return PURE::make(foper, fargs);
            }
          case _con_SELECT:
            {
                auto fidx = asdl::read_int(is);
                auto farg = exp::read(is);
                return SELECT::make(fidx, farg);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "exp");
        }
    }
    exp::~exp () { }
    VAR::~VAR () { }
    LABEL::~LABEL () { }
    NUM::~NUM () { }
    LOOKER::~LOOKER ()
    {
        delete this->_v_oper;
        for (auto item : this->_v_args) {
            delete item;
        }
    }
    PURE::~PURE ()
    {
        delete this->_v_oper;
        for (auto item : this->_v_args) {
            delete item;
        }
    }
    SELECT::~SELECT ()
    {
        delete this->_v_arg;
    }
    // exp_seq pickler suppressed
    std::vector<exp *> read_exp_seq (asdl::instream & is)
    {
        return asdl::read_seq<exp>(is);
    }
    param * param::read (asdl::instream & is)
    {
        auto fname = LambdaVar::read_lvar(is);
        auto fty = ty::read(is);
        return param::make(fname, fty);
    }
    param::~param ()
    {
        delete this->_v_ty;
    }
    // param_seq pickler suppressed
    std::vector<param *> read_param_seq (asdl::instream & is)
    {
        return asdl::read_seq<param>(is);
    }
    // pickler suppressed for probability
    probability read_probability (asdl::instream & is)
    {
        auto v = asdl::read_int(is);
        return v;
    }
    stm * stm::read (asdl::instream & is)
    {
        _tag_t tag = static_cast<_tag_t>(asdl::read_tag8(is));
        switch (tag) {
          case _con_LET:
            {
                auto f0 = exp::read(is);
                auto f1 = param::read(is);
                auto f2 = stm::read(is);
                return LET::make(f0, f1, f2);
            }
          case _con_ALLOC:
            {
                auto f0 = CFG_Prim::alloc::read(is);
                auto f1 = read_exp_seq(is);
                auto f2 = LambdaVar::read_lvar(is);
                auto f3 = stm::read(is);
                return ALLOC::make(f0, f1, f2, f3);
            }
          case _con_APPLY:
            {
                auto f0 = exp::read(is);
                auto f1 = read_exp_seq(is);
                auto f2 = read_ty_seq(is);
                return APPLY::make(f0, f1, f2);
            }
          case _con_THROW:
            {
                auto f0 = exp::read(is);
                auto f1 = read_exp_seq(is);
                auto f2 = read_ty_seq(is);
                return THROW::make(f0, f1, f2);
            }
          case _con_GOTO:
            {
                auto f0 = LambdaVar::read_lvar(is);
                auto f1 = read_exp_seq(is);
                return GOTO::make(f0, f1);
            }
          case _con_SWITCH:
            {
                auto f0 = exp::read(is);
                auto f1 = read_stm_seq(is);
                return SWITCH::make(f0, f1);
            }
          case _con_BRANCH:
            {
                auto f0 = CFG_Prim::branch::read(is);
                auto f1 = read_exp_seq(is);
                auto f2 = read_probability(is);
                auto f3 = stm::read(is);
                auto f4 = stm::read(is);
                return BRANCH::make(f0, f1, f2, f3, f4);
            }
          case _con_ARITH:
            {
                auto f0 = CFG_Prim::arith::read(is);
                auto f1 = read_exp_seq(is);
                auto f2 = param::read(is);
                auto f3 = stm::read(is);
                return ARITH::make(f0, f1, f2, f3);
            }
          case _con_SETTER:
            {
                auto f0 = CFG_Prim::setter::read(is);
                auto f1 = read_exp_seq(is);
                auto f2 = stm::read(is);
                return SETTER::make(f0, f1, f2);
            }
          case _con_CALLGC:
            {
                auto f0 = read_exp_seq(is);
                auto f1 = LambdaVar::read_lvar_seq(is);
                auto f2 = stm::read(is);
                return CALLGC::make(f0, f1, f2);
            }
          case _con_RCC:
            {
                auto freentrant = asdl::read_bool(is);
                auto flinkage = asdl::read_string(is);
                auto fproto = CTypes::c_proto::read(is);
                auto fargs = read_exp_seq(is);
                auto fresults = read_param_seq(is);
                auto flive = read_param_seq(is);
                auto fk = stm::read(is);
                return RCC::make(freentrant, flinkage, fproto, fargs, fresults, flive,
                    fk);
            }
          default:
            is.invalidTag(static_cast<unsigned int>(tag), "stm");
        }
    }
    stm::~stm () { }
    LET::~LET ()
    {
        delete this->_v0;
        delete this->_v1;
        delete this->_v2;
    }
    ALLOC::~ALLOC ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
        delete this->_v3;
    }
    APPLY::~APPLY ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
        for (auto item : this->_v2) {
            delete item;
        }
    }
    THROW::~THROW ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
        for (auto item : this->_v2) {
            delete item;
        }
    }
    GOTO::~GOTO ()
    {
        for (auto item : this->_v1) {
            delete item;
        }
    }
    SWITCH::~SWITCH ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
    }
    BRANCH::~BRANCH ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
        delete this->_v3;
        delete this->_v4;
    }
    ARITH::~ARITH ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
        delete this->_v2;
        delete this->_v3;
    }
    SETTER::~SETTER ()
    {
        delete this->_v0;
        for (auto item : this->_v1) {
            delete item;
        }
        delete this->_v2;
    }
    CALLGC::~CALLGC ()
    {
        for (auto item : this->_v0) {
            delete item;
        }
        delete this->_v2;
    }
    RCC::~RCC ()
    {
        delete this->_v_proto;
        for (auto item : this->_v_args) {
            delete item;
        }
        for (auto item : this->_v_results) {
            delete item;
        }
        for (auto item : this->_v_live) {
            delete item;
        }
        delete this->_v_k;
    }
    // stm_seq pickler suppressed
    std::vector<stm *> read_stm_seq (asdl::instream & is)
    {
        return asdl::read_seq<stm>(is);
    }
    // pickler suppressed for frag_kind
    frag_kind read_frag_kind (asdl::instream & is)
    {
        return static_cast<frag_kind>(asdl::read_tag8(is));
    }
    frag * frag::read (asdl::instream & is)
    {
        auto fkind = read_frag_kind(is);
        auto flab = LambdaVar::read_lvar(is);
        auto fparams = read_param_seq(is);
        auto fbody = stm::read(is);
        return frag::make(fkind, flab, fparams, fbody);
    }
    frag::~frag ()
    {
        for (auto item : this->_v_params) {
            delete item;
        }
        delete this->_v_body;
    }
    // frag_seq pickler suppressed
    std::vector<frag *> read_frag_seq (asdl::instream & is)
    {
        return asdl::read_seq<frag>(is);
    }
    attrs * attrs::read (asdl::instream & is)
    {
        auto falignHP = asdl::read_int(is);
        auto fneedsBasePtr = asdl::read_bool(is);
        auto fhasTrapArith = asdl::read_bool(is);
        auto fhasRCC = asdl::read_bool(is);
        return attrs::make(falignHP, fneedsBasePtr, fhasTrapArith, fhasRCC);
    }
    attrs::~attrs () { }
    cluster * cluster::read (asdl::instream & is)
    {
        auto fattrs = attrs::read(is);
        auto ffrags = read_frag_seq(is);
        return cluster::make(fattrs, ffrags);
    }
    cluster::~cluster ()
    {
        delete this->_v_attrs;
        for (auto item : this->_v_frags) {
            delete item;
        }
    }
    // cluster_seq pickler suppressed
    std::vector<cluster *> read_cluster_seq (asdl::instream & is)
    {
        return asdl::read_seq<cluster>(is);
    }
    comp_unit * comp_unit::read (asdl::instream & is)
    {
        auto fsrcFile = asdl::read_string(is);
        auto fentry = cluster::read(is);
        auto ffns = read_cluster_seq(is);
        return comp_unit::make(fsrcFile, fentry, ffns);
    }
    comp_unit::~comp_unit ()
    {
        delete this->_v_entry;
        for (auto item : this->_v_fns) {
            delete item;
        }
    }
} // namespace CFG
