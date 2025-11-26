// cfg.hpp
//
// Generated from cfg.asdl by asdlgen.
//

#include "asdl/asdl.hpp"

#include "context.hpp"
#include "lambda-var.hpp"


namespace CTypes {
    class c_type;
    enum class c_int;
    class c_proto;
    class c_type {
      public:
        virtual ~c_type ();
        // pickler method suppressed
        static c_type * read (asdl::instream & is);
      protected:
        enum _tag_t {
            _con_C_void = 1,
            _con_C_float,
            _con_C_double,
            _con_C_long_double,
            _con_C_unsigned,
            _con_C_signed,
            _con_C_PTR,
            _con_C_ARRAY,
            _con_C_STRUCT,
            _con_C_UNION
        };
        c_type (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    struct C_void : public c_type {
        C_void ()
          : c_type(c_type::_con_C_void)
        { }
        ~C_void ();
        static C_void * make ()
        {
            return new C_void;
        }
        // pickler method suppressed
    };
    struct C_float : public c_type {
        C_float ()
          : c_type(c_type::_con_C_float)
        { }
        ~C_float ();
        static C_float * make ()
        {
            return new C_float;
        }
        // pickler method suppressed
    };
    struct C_double : public c_type {
        C_double ()
          : c_type(c_type::_con_C_double)
        { }
        ~C_double ();
        static C_double * make ()
        {
            return new C_double;
        }
        // pickler method suppressed
    };
    struct C_long_double : public c_type {
        C_long_double ()
          : c_type(c_type::_con_C_long_double)
        { }
        ~C_long_double ();
        static C_long_double * make ()
        {
            return new C_long_double;
        }
        // pickler method suppressed
    };
    class C_unsigned : public c_type {
      public:
        C_unsigned (c_int p0)
          : c_type(c_type::_con_C_unsigned), _v0(p0)
        { }
        ~C_unsigned ();
        static C_unsigned * make (c_int p0)
        {
            return new C_unsigned(p0);
        }
        // pickler method suppressed
        c_int get_0 () const
        {
            return this->_v0;
        }
        void set_0 (c_int v)
        {
            this->_v0 = v;
        }
      private:
        c_int _v0;
    };
    class C_signed : public c_type {
      public:
        C_signed (c_int p0)
          : c_type(c_type::_con_C_signed), _v0(p0)
        { }
        ~C_signed ();
        static C_signed * make (c_int p0)
        {
            return new C_signed(p0);
        }
        // pickler method suppressed
        c_int get_0 () const
        {
            return this->_v0;
        }
        void set_0 (c_int v)
        {
            this->_v0 = v;
        }
      private:
        c_int _v0;
    };
    struct C_PTR : public c_type {
        C_PTR ()
          : c_type(c_type::_con_C_PTR)
        { }
        ~C_PTR ();
        static C_PTR * make ()
        {
            return new C_PTR;
        }
        // pickler method suppressed
    };
    class C_ARRAY : public c_type {
      public:
        C_ARRAY (c_type * p0, int p1)
          : c_type(c_type::_con_C_ARRAY), _v0(p0), _v1(p1)
        { }
        ~C_ARRAY ();
        static C_ARRAY * make (c_type * p0, int p1)
        {
            return new C_ARRAY(p0, p1);
        }
        // pickler method suppressed
        c_type * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (c_type * v)
        {
            this->_v0 = v;
        }
        int get_1 () const
        {
            return this->_v1;
        }
        void set_1 (int v)
        {
            this->_v1 = v;
        }
      private:
        c_type * _v0;
        int _v1;
    };
    class C_STRUCT : public c_type {
      public:
        C_STRUCT (std::vector<c_type *> const & p0)
          : c_type(c_type::_con_C_STRUCT), _v0(p0)
        { }
        ~C_STRUCT ();
        static C_STRUCT * make (std::vector<c_type *> const & p0)
        {
            return new C_STRUCT(p0);
        }
        // pickler method suppressed
        std::vector<c_type *> get_0 () const
        {
            return this->_v0;
        }
        void set_0 (std::vector<c_type *> const & v)
        {
            this->_v0 = v;
        }
      private:
        std::vector<c_type *> _v0;
    };
    class C_UNION : public c_type {
      public:
        C_UNION (std::vector<c_type *> const & p0)
          : c_type(c_type::_con_C_UNION), _v0(p0)
        { }
        ~C_UNION ();
        static C_UNION * make (std::vector<c_type *> const & p0)
        {
            return new C_UNION(p0);
        }
        // pickler method suppressed
        std::vector<c_type *> get_0 () const
        {
            return this->_v0;
        }
        void set_0 (std::vector<c_type *> const & v)
        {
            this->_v0 = v;
        }
      private:
        std::vector<c_type *> _v0;
    };
    // c_type_seq pickler suppressed
    std::vector<c_type *> read_c_type_seq (asdl::instream & is);
    enum class c_int {I_char = 1, I_short, I_int, I_long, I_long_long};
    // pickler suppressed for c_int
    c_int read_c_int (asdl::instream & is);
    using calling_convention = std::string;
    class c_proto {
      public:
        c_proto (calling_convention p_conv, c_type * p_retTy, std::vector<c_type *> const & p_paramTys)
          : _v_conv(p_conv), _v_retTy(p_retTy), _v_paramTys(p_paramTys)
        { }
        ~c_proto ();
        static c_proto * make (calling_convention p_conv, c_type * p_retTy, std::vector<c_type *> const & p_paramTys)
        {
            return new c_proto(p_conv, p_retTy, p_paramTys);
        }
        // pickler method suppressed
        static c_proto * read (asdl::instream & is);
        calling_convention get_conv () const
        {
            return this->_v_conv;
        }
        void set_conv (calling_convention v)
        {
            this->_v_conv = v;
        }
        c_type * get_retTy () const
        {
            return this->_v_retTy;
        }
        void set_retTy (c_type * v)
        {
            this->_v_retTy = v;
        }
        std::vector<c_type *> get_paramTys () const
        {
            return this->_v_paramTys;
        }
        void set_paramTys (std::vector<c_type *> const & v)
        {
            this->_v_paramTys = v;
        }
      private:
        calling_convention _v_conv;
        c_type * _v_retTy;
        std::vector<c_type *> _v_paramTys;
    };
} // namespace CTypes
namespace CFG_Prim {
    enum class numkind;
    enum class rounding_mode;
    class raw_ty;
    class alloc;
    enum class arithop;
    class arith;
    enum class pureop;
    class pure;
    class looker;
    class setter;
    enum class cmpop;
    enum class fcmpop;
    class branch;
    enum class numkind {INT = 1, FLT};
    // pickler suppressed for numkind
    numkind read_numkind (asdl::instream & is);
    enum class rounding_mode {TO_NEAREST = 1, TO_NEGINF, TO_POSINF, TO_ZERO};
    // pickler suppressed for rounding_mode
    rounding_mode read_rounding_mode (asdl::instream & is);
    class raw_ty {
      public:
        raw_ty (numkind p_kind, int p_sz)
          : _v_kind(p_kind), _v_sz(p_sz)
        { }
        ~raw_ty ();
        static raw_ty * make (numkind p_kind, int p_sz)
        {
            return new raw_ty(p_kind, p_sz);
        }
        // pickler method suppressed
        static raw_ty * read (asdl::instream & is);
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
      private:
        numkind _v_kind;
        int _v_sz;
    };
    // raw_ty_seq pickler suppressed
    std::vector<raw_ty *> read_raw_ty_seq (asdl::instream & is);
    class alloc {
      public:
        virtual ~alloc ();
        // pickler method suppressed
        static alloc * read (asdl::instream & is);
        virtual llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args) = 0;


      protected:
        enum _tag_t {_con_SPECIAL = 1, _con_RECORD, _con_RAW_RECORD, _con_RAW_ALLOC};
        alloc (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    struct SPECIAL : public alloc {
        SPECIAL ()
          : alloc(alloc::_con_SPECIAL)
        { }
        ~SPECIAL ();
        static SPECIAL * make ()
        {
            return new SPECIAL;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    class RECORD : public alloc {
      public:
        RECORD (asdl::integer p_desc, bool p_mut)
          : alloc(alloc::_con_RECORD), _v_desc(p_desc), _v_mut(p_mut)
        { }
        ~RECORD ();
        static RECORD * make (asdl::integer p_desc, bool p_mut)
        {
            return new RECORD(p_desc, p_mut);
        }
        // pickler method suppressed
        asdl::integer get_desc () const
        {
            return this->_v_desc;
        }
        void set_desc (asdl::integer v)
        {
            this->_v_desc = v;
        }
        bool get_mut () const
        {
            return this->_v_mut;
        }
        void set_mut (bool v)
        {
            this->_v_mut = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        asdl::integer _v_desc;
        bool _v_mut;
    };
    class RAW_RECORD : public alloc {
      public:
        RAW_RECORD (asdl::integer p_desc, int p_align, std::vector<raw_ty *> const & p_fields)
          : alloc(alloc::_con_RAW_RECORD), _v_desc(p_desc), _v_align(p_align),
              _v_fields(p_fields)
        { }
        ~RAW_RECORD ();
        static RAW_RECORD * make (asdl::integer p_desc, int p_align, std::vector<raw_ty *> const & p_fields)
        {
            return new RAW_RECORD(p_desc, p_align, p_fields);
        }
        // pickler method suppressed
        asdl::integer get_desc () const
        {
            return this->_v_desc;
        }
        void set_desc (asdl::integer v)
        {
            this->_v_desc = v;
        }
        int get_align () const
        {
            return this->_v_align;
        }
        void set_align (int v)
        {
            this->_v_align = v;
        }
        std::vector<raw_ty *> get_fields () const
        {
            return this->_v_fields;
        }
        void set_fields (std::vector<raw_ty *> const & v)
        {
            this->_v_fields = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        asdl::integer _v_desc;
        int _v_align;
        std::vector<raw_ty *> _v_fields;
    };
    class RAW_ALLOC : public alloc {
      public:
        RAW_ALLOC (std::optional<asdl::integer> p_desc, int p_align, int p_len)
          : alloc(alloc::_con_RAW_ALLOC), _v_desc(p_desc), _v_align(p_align),
              _v_len(p_len)
        { }
        ~RAW_ALLOC ();
        static RAW_ALLOC * make (std::optional<asdl::integer> p_desc, int p_align, int p_len)
        {
            return new RAW_ALLOC(p_desc, p_align, p_len);
        }
        // pickler method suppressed
        std::optional<asdl::integer> get_desc () const
        {
            return this->_v_desc;
        }
        void set_desc (std::optional<asdl::integer> v)
        {
            this->_v_desc = v;
        }
        int get_align () const
        {
            return this->_v_align;
        }
        void set_align (int v)
        {
            this->_v_align = v;
        }
        int get_len () const
        {
            return this->_v_len;
        }
        void set_len (int v)
        {
            this->_v_len = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        std::optional<asdl::integer> _v_desc;
        int _v_align;
        int _v_len;
    };
    enum class arithop {IADD = 1, ISUB, IMUL, IDIV, IREM};
    // pickler suppressed for arithop
    arithop read_arithop (asdl::instream & is);
    class arith {
      public:
        virtual ~arith ();
        // pickler method suppressed
        static arith * read (asdl::instream & is);
        virtual llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args) = 0;


      protected:
        enum _tag_t {_con_ARITH = 1, _con_FLOAT_TO_INT};
        arith (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    class ARITH : public arith {
      public:
        ARITH (arithop p_oper, int p_sz)
          : arith(arith::_con_ARITH), _v_oper(p_oper), _v_sz(p_sz)
        { }
        ~ARITH ();
        static ARITH * make (arithop p_oper, int p_sz)
        {
            return new ARITH(p_oper, p_sz);
        }
        // pickler method suppressed
        arithop get_oper () const
        {
            return this->_v_oper;
        }
        void set_oper (arithop v)
        {
            this->_v_oper = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        arithop _v_oper;
        int _v_sz;
    };
    class FLOAT_TO_INT : public arith {
      public:
        FLOAT_TO_INT (rounding_mode p_mode, int p_from, int p_to)
          : arith(arith::_con_FLOAT_TO_INT), _v_mode(p_mode), _v_from(p_from),
              _v_to(p_to)
        { }
        ~FLOAT_TO_INT ();
        static FLOAT_TO_INT * make (rounding_mode p_mode, int p_from, int p_to)
        {
            return new FLOAT_TO_INT(p_mode, p_from, p_to);
        }
        // pickler method suppressed
        rounding_mode get_mode () const
        {
            return this->_v_mode;
        }
        void set_mode (rounding_mode v)
        {
            this->_v_mode = v;
        }
        int get_from () const
        {
            return this->_v_from;
        }
        void set_from (int v)
        {
            this->_v_from = v;
        }
        int get_to () const
        {
            return this->_v_to;
        }
        void set_to (int v)
        {
            this->_v_to = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        rounding_mode _v_mode;
        int _v_from;
        int _v_to;
    };
    enum class pureop {
        ADD = 1,
        SUB,
        MUL,
        SDIV,
        SREM,
        UDIV,
        UREM,
        SHL,
        ASHR,
        LSHR,
        ORB,
        XORB,
        ANDB,
        CNTPOP,
        CNTLZ,
        CNTTZ,
        ROTL,
        ROTR,
        FADD,
        FSUB,
        FMUL,
        FDIV,
        FREM,
        FMADD,
        FNEG,
        FABS,
        FCOPYSIGN,
        FSQRT
    };
    // pickler suppressed for pureop
    pureop read_pureop (asdl::instream & is);
    class pure {
      public:
        virtual ~pure ();
        // pickler method suppressed
        static pure * read (asdl::instream & is);
        virtual llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args) = 0;


      protected:
        enum _tag_t {
            _con_PURE_ARITH = 1,
            _con_EXTEND,
            _con_TRUNC,
            _con_INT_TO_FLOAT,
            _con_FLOAT_TO_BITS,
            _con_BITS_TO_FLOAT,
            _con_PURE_SUBSCRIPT,
            _con_PURE_RAW_SUBSCRIPT,
            _con_RAW_SELECT
        };
        pure (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    class PURE_ARITH : public pure {
      public:
        PURE_ARITH (pureop p_oper, int p_sz)
          : pure(pure::_con_PURE_ARITH), _v_oper(p_oper), _v_sz(p_sz)
        { }
        ~PURE_ARITH ();
        static PURE_ARITH * make (pureop p_oper, int p_sz)
        {
            return new PURE_ARITH(p_oper, p_sz);
        }
        // pickler method suppressed
        pureop get_oper () const
        {
            return this->_v_oper;
        }
        void set_oper (pureop v)
        {
            this->_v_oper = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        pureop _v_oper;
        int _v_sz;
    };
    class EXTEND : public pure {
      public:
        EXTEND (bool p_signed, int p_from, int p_to)
          : pure(pure::_con_EXTEND), _v_signed(p_signed), _v_from(p_from), _v_to(p_to)
        { }
        ~EXTEND ();
        static EXTEND * make (bool p_signed, int p_from, int p_to)
        {
            return new EXTEND(p_signed, p_from, p_to);
        }
        // pickler method suppressed
        bool get_signed () const
        {
            return this->_v_signed;
        }
        void set_signed (bool v)
        {
            this->_v_signed = v;
        }
        int get_from () const
        {
            return this->_v_from;
        }
        void set_from (int v)
        {
            this->_v_from = v;
        }
        int get_to () const
        {
            return this->_v_to;
        }
        void set_to (int v)
        {
            this->_v_to = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        bool _v_signed;
        int _v_from;
        int _v_to;
    };
    class TRUNC : public pure {
      public:
        TRUNC (int p_from, int p_to)
          : pure(pure::_con_TRUNC), _v_from(p_from), _v_to(p_to)
        { }
        ~TRUNC ();
        static TRUNC * make (int p_from, int p_to)
        {
            return new TRUNC(p_from, p_to);
        }
        // pickler method suppressed
        int get_from () const
        {
            return this->_v_from;
        }
        void set_from (int v)
        {
            this->_v_from = v;
        }
        int get_to () const
        {
            return this->_v_to;
        }
        void set_to (int v)
        {
            this->_v_to = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        int _v_from;
        int _v_to;
    };
    class INT_TO_FLOAT : public pure {
      public:
        INT_TO_FLOAT (int p_from, int p_to)
          : pure(pure::_con_INT_TO_FLOAT), _v_from(p_from), _v_to(p_to)
        { }
        ~INT_TO_FLOAT ();
        static INT_TO_FLOAT * make (int p_from, int p_to)
        {
            return new INT_TO_FLOAT(p_from, p_to);
        }
        // pickler method suppressed
        int get_from () const
        {
            return this->_v_from;
        }
        void set_from (int v)
        {
            this->_v_from = v;
        }
        int get_to () const
        {
            return this->_v_to;
        }
        void set_to (int v)
        {
            this->_v_to = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        int _v_from;
        int _v_to;
    };
    class FLOAT_TO_BITS : public pure {
      public:
        FLOAT_TO_BITS (int p_sz)
          : pure(pure::_con_FLOAT_TO_BITS), _v_sz(p_sz)
        { }
        ~FLOAT_TO_BITS ();
        static FLOAT_TO_BITS * make (int p_sz)
        {
            return new FLOAT_TO_BITS(p_sz);
        }
        // pickler method suppressed
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        int _v_sz;
    };
    class BITS_TO_FLOAT : public pure {
      public:
        BITS_TO_FLOAT (int p_sz)
          : pure(pure::_con_BITS_TO_FLOAT), _v_sz(p_sz)
        { }
        ~BITS_TO_FLOAT ();
        static BITS_TO_FLOAT * make (int p_sz)
        {
            return new BITS_TO_FLOAT(p_sz);
        }
        // pickler method suppressed
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        int _v_sz;
    };
    struct PURE_SUBSCRIPT : public pure {
        PURE_SUBSCRIPT ()
          : pure(pure::_con_PURE_SUBSCRIPT)
        { }
        ~PURE_SUBSCRIPT ();
        static PURE_SUBSCRIPT * make ()
        {
            return new PURE_SUBSCRIPT;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    class PURE_RAW_SUBSCRIPT : public pure {
      public:
        PURE_RAW_SUBSCRIPT (numkind p_kind, int p_sz)
          : pure(pure::_con_PURE_RAW_SUBSCRIPT), _v_kind(p_kind), _v_sz(p_sz)
        { }
        ~PURE_RAW_SUBSCRIPT ();
        static PURE_RAW_SUBSCRIPT * make (numkind p_kind, int p_sz)
        {
            return new PURE_RAW_SUBSCRIPT(p_kind, p_sz);
        }
        // pickler method suppressed
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        numkind _v_kind;
        int _v_sz;
    };
    class RAW_SELECT : public pure {
      public:
        RAW_SELECT (numkind p_kind, int p_sz, int p_offset)
          : pure(pure::_con_RAW_SELECT), _v_kind(p_kind), _v_sz(p_sz),
              _v_offset(p_offset)
        { }
        ~RAW_SELECT ();
        static RAW_SELECT * make (numkind p_kind, int p_sz, int p_offset)
        {
            return new RAW_SELECT(p_kind, p_sz, p_offset);
        }
        // pickler method suppressed
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        int get_offset () const
        {
            return this->_v_offset;
        }
        void set_offset (int v)
        {
            this->_v_offset = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        numkind _v_kind;
        int _v_sz;
        int _v_offset;
    };
    class looker {
      public:
        virtual ~looker ();
        // pickler method suppressed
        static looker * read (asdl::instream & is);
        virtual llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args) = 0;


      protected:
        enum _tag_t {
            _con_DEREF = 1,
            _con_SUBSCRIPT,
            _con_RAW_SUBSCRIPT,
            _con_RAW_LOAD,
            _con_GET_HDLR,
            _con_GET_VAR
        };
        looker (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    struct DEREF : public looker {
        DEREF ()
          : looker(looker::_con_DEREF)
        { }
        ~DEREF ();
        static DEREF * make ()
        {
            return new DEREF;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct SUBSCRIPT : public looker {
        SUBSCRIPT ()
          : looker(looker::_con_SUBSCRIPT)
        { }
        ~SUBSCRIPT ();
        static SUBSCRIPT * make ()
        {
            return new SUBSCRIPT;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    class RAW_SUBSCRIPT : public looker {
      public:
        RAW_SUBSCRIPT (numkind p_kind, int p_sz)
          : looker(looker::_con_RAW_SUBSCRIPT), _v_kind(p_kind), _v_sz(p_sz)
        { }
        ~RAW_SUBSCRIPT ();
        static RAW_SUBSCRIPT * make (numkind p_kind, int p_sz)
        {
            return new RAW_SUBSCRIPT(p_kind, p_sz);
        }
        // pickler method suppressed
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        numkind _v_kind;
        int _v_sz;
    };
    class RAW_LOAD : public looker {
      public:
        RAW_LOAD (numkind p_kind, int p_sz)
          : looker(looker::_con_RAW_LOAD), _v_kind(p_kind), _v_sz(p_sz)
        { }
        ~RAW_LOAD ();
        static RAW_LOAD * make (numkind p_kind, int p_sz)
        {
            return new RAW_LOAD(p_kind, p_sz);
        }
        // pickler method suppressed
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        numkind _v_kind;
        int _v_sz;
    };
    struct GET_HDLR : public looker {
        GET_HDLR ()
          : looker(looker::_con_GET_HDLR)
        { }
        ~GET_HDLR ();
        static GET_HDLR * make ()
        {
            return new GET_HDLR;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct GET_VAR : public looker {
        GET_VAR ()
          : looker(looker::_con_GET_VAR)
        { }
        ~GET_VAR ();
        static GET_VAR * make ()
        {
            return new GET_VAR;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    class setter {
      public:
        virtual ~setter ();
        // pickler method suppressed
        static setter * read (asdl::instream & is);
        virtual void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args) = 0;

      protected:
        enum _tag_t {
            _con_UNBOXED_UPDATE = 1,
            _con_UPDATE,
            _con_UNBOXED_ASSIGN,
            _con_ASSIGN,
            _con_RAW_UPDATE,
            _con_RAW_STORE,
            _con_SET_HDLR,
            _con_SET_VAR
        };
        setter (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    struct UNBOXED_UPDATE : public setter {
        UNBOXED_UPDATE ()
          : setter(setter::_con_UNBOXED_UPDATE)
        { }
        ~UNBOXED_UPDATE ();
        static UNBOXED_UPDATE * make ()
        {
            return new UNBOXED_UPDATE;
        }
        // pickler method suppressed
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct UPDATE : public setter {
        UPDATE ()
          : setter(setter::_con_UPDATE)
        { }
        ~UPDATE ();
        static UPDATE * make ()
        {
            return new UPDATE;
        }
        // pickler method suppressed
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct UNBOXED_ASSIGN : public setter {
        UNBOXED_ASSIGN ()
          : setter(setter::_con_UNBOXED_ASSIGN)
        { }
        ~UNBOXED_ASSIGN ();
        static UNBOXED_ASSIGN * make ()
        {
            return new UNBOXED_ASSIGN;
        }
        // pickler method suppressed
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct ASSIGN : public setter {
        ASSIGN ()
          : setter(setter::_con_ASSIGN)
        { }
        ~ASSIGN ();
        static ASSIGN * make ()
        {
            return new ASSIGN;
        }
        // pickler method suppressed
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    class RAW_UPDATE : public setter {
      public:
        RAW_UPDATE (numkind p_kind, int p_sz)
          : setter(setter::_con_RAW_UPDATE), _v_kind(p_kind), _v_sz(p_sz)
        { }
        ~RAW_UPDATE ();
        static RAW_UPDATE * make (numkind p_kind, int p_sz)
        {
            return new RAW_UPDATE(p_kind, p_sz);
        }
        // pickler method suppressed
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        numkind _v_kind;
        int _v_sz;
    };
    class RAW_STORE : public setter {
      public:
        RAW_STORE (numkind p_kind, int p_sz)
          : setter(setter::_con_RAW_STORE), _v_kind(p_kind), _v_sz(p_sz)
        { }
        ~RAW_STORE ();
        static RAW_STORE * make (numkind p_kind, int p_sz)
        {
            return new RAW_STORE(p_kind, p_sz);
        }
        // pickler method suppressed
        numkind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (numkind v)
        {
            this->_v_kind = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        numkind _v_kind;
        int _v_sz;
    };
    struct SET_HDLR : public setter {
        SET_HDLR ()
          : setter(setter::_con_SET_HDLR)
        { }
        ~SET_HDLR ();
        static SET_HDLR * make ()
        {
            return new SET_HDLR;
        }
        // pickler method suppressed
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct SET_VAR : public setter {
        SET_VAR ()
          : setter(setter::_con_SET_VAR)
        { }
        ~SET_VAR ();
        static SET_VAR * make ()
        {
            return new SET_VAR;
        }
        // pickler method suppressed
        void codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    enum class cmpop {GT = 1, GTE, LT, LTE, EQL, NEQ};
    // pickler suppressed for cmpop
    cmpop read_cmpop (asdl::instream & is);
    enum class fcmpop {
        F_EQ = 1,
        F_ULG,
        F_UN,
        F_LEG,
        F_GT,
        F_GE,
        F_UGT,
        F_UGE,
        F_LT,
        F_LE,
        F_ULT,
        F_ULE,
        F_LG,
        F_UE
    };
    // pickler suppressed for fcmpop
    fcmpop read_fcmpop (asdl::instream & is);
    class branch {
      public:
        virtual ~branch ();
        // pickler method suppressed
        static branch * read (asdl::instream & is);
        virtual llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args) = 0;


      protected:
        enum _tag_t {_con_CMP = 1, _con_FCMP, _con_FSGN, _con_PEQL, _con_PNEQ, _con_LIMIT
        };
        branch (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    class CMP : public branch {
      public:
        CMP (cmpop p_oper, bool p_signed, int p_sz)
          : branch(branch::_con_CMP), _v_oper(p_oper), _v_signed(p_signed), _v_sz(p_sz)
        { }
        ~CMP ();
        static CMP * make (cmpop p_oper, bool p_signed, int p_sz)
        {
            return new CMP(p_oper, p_signed, p_sz);
        }
        // pickler method suppressed
        cmpop get_oper () const
        {
            return this->_v_oper;
        }
        void set_oper (cmpop v)
        {
            this->_v_oper = v;
        }
        bool get_signed () const
        {
            return this->_v_signed;
        }
        void set_signed (bool v)
        {
            this->_v_signed = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        cmpop _v_oper;
        bool _v_signed;
        int _v_sz;
    };
    class FCMP : public branch {
      public:
        FCMP (fcmpop p_oper, int p_sz)
          : branch(branch::_con_FCMP), _v_oper(p_oper), _v_sz(p_sz)
        { }
        ~FCMP ();
        static FCMP * make (fcmpop p_oper, int p_sz)
        {
            return new FCMP(p_oper, p_sz);
        }
        // pickler method suppressed
        fcmpop get_oper () const
        {
            return this->_v_oper;
        }
        void set_oper (fcmpop v)
        {
            this->_v_oper = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        fcmpop _v_oper;
        int _v_sz;
    };
    class FSGN : public branch {
      public:
        FSGN (int p0)
          : branch(branch::_con_FSGN), _v0(p0)
        { }
        ~FSGN ();
        static FSGN * make (int p0)
        {
            return new FSGN(p0);
        }
        // pickler method suppressed
        int get_0 () const
        {
            return this->_v0;
        }
        void set_0 (int v)
        {
            this->_v0 = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        int _v0;
    };
    struct PEQL : public branch {
        PEQL ()
          : branch(branch::_con_PEQL)
        { }
        ~PEQL ();
        static PEQL * make ()
        {
            return new PEQL;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    struct PNEQ : public branch {
        PNEQ ()
          : branch(branch::_con_PNEQ)
        { }
        ~PNEQ ();
        static PNEQ * make ()
        {
            return new PNEQ;
        }
        // pickler method suppressed
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

    };
    class LIMIT : public branch {
      public:
        LIMIT (unsigned int p0)
          : branch(branch::_con_LIMIT), _v0(p0)
        { }
        ~LIMIT ();
        static LIMIT * make (unsigned int p0)
        {
            return new LIMIT(p0);
        }
        // pickler method suppressed
        unsigned int get_0 () const
        {
            return this->_v0;
        }
        void set_0 (unsigned int v)
        {
            this->_v0 = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt, Args_t const &args);

      private:
        unsigned int _v0;
    };
} // namespace CFG_Prim
namespace CFG {
    class ty;
    class exp;
    class param;
    class stm;
    enum class frag_kind;
    class frag;
    class attrs;
    class cluster;
    class comp_unit;
    class ty {
      public:
        virtual ~ty ();
        // pickler method suppressed
        static ty * read (asdl::instream & is);
        virtual llvm::Type *codegen (smlnj::cfgcg::Context *cxt) = 0;
        bool isNUMt () { return this->_tag == _con_NUMt; }
        bool isFLTt () { return this->_tag == _con_FLTt; }


      protected:
        enum _tag_t {_con_LABt = 1, _con_PTRt, _con_TAGt, _con_NUMt, _con_FLTt};
        ty (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    struct LABt : public ty {
        LABt ()
          : ty(ty::_con_LABt)
        { }
        ~LABt ();
        static LABt * make ()
        {
            return new LABt;
        }
        // pickler method suppressed
        llvm::Type *codegen (smlnj::cfgcg::Context *cxt);

    };
    struct PTRt : public ty {
        PTRt ()
          : ty(ty::_con_PTRt)
        { }
        ~PTRt ();
        static PTRt * make ()
        {
            return new PTRt;
        }
        // pickler method suppressed
        llvm::Type *codegen (smlnj::cfgcg::Context *cxt);

    };
    struct TAGt : public ty {
        TAGt ()
          : ty(ty::_con_TAGt)
        { }
        ~TAGt ();
        static TAGt * make ()
        {
            return new TAGt;
        }
        // pickler method suppressed
        llvm::Type *codegen (smlnj::cfgcg::Context *cxt);

    };
    class NUMt : public ty {
      public:
        NUMt (int p_sz)
          : ty(ty::_con_NUMt), _v_sz(p_sz)
        { }
        ~NUMt ();
        static NUMt * make (int p_sz)
        {
            return new NUMt(p_sz);
        }
        // pickler method suppressed
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Type *codegen (smlnj::cfgcg::Context *cxt);

      private:
        int _v_sz;
    };
    class FLTt : public ty {
      public:
        FLTt (int p_sz)
          : ty(ty::_con_FLTt), _v_sz(p_sz)
        { }
        ~FLTt ();
        static FLTt * make (int p_sz)
        {
            return new FLTt(p_sz);
        }
        // pickler method suppressed
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Type *codegen (smlnj::cfgcg::Context *cxt);

      private:
        int _v_sz;
    };
    // ty_seq pickler suppressed
    std::vector<ty *> read_ty_seq (asdl::instream & is);
    class exp {
      public:
        virtual ~exp ();
        // pickler method suppressed
        static exp * read (asdl::instream & is);
        virtual llvm::Value *codegen (smlnj::cfgcg::Context *cxt) = 0;
        bool isLABEL () { return (this->_tag == _con_LABEL); }


      protected:
        enum _tag_t {
            _con_VAR = 1,
            _con_LABEL,
            _con_NUM,
            _con_LOOKER,
            _con_PURE,
            _con_SELECT
        };
        exp (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
    };
    class VAR : public exp {
      public:
        VAR (LambdaVar::lvar p_name)
          : exp(exp::_con_VAR), _v_name(p_name)
        { }
        ~VAR ();
        static VAR * make (LambdaVar::lvar p_name)
        {
            return new VAR(p_name);
        }
        // pickler method suppressed
        LambdaVar::lvar get_name () const
        {
            return this->_v_name;
        }
        void set_name (LambdaVar::lvar v)
        {
            this->_v_name = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt);

      private:
        LambdaVar::lvar _v_name;
    };
    class LABEL : public exp {
      public:
        LABEL (LambdaVar::lvar p_name)
          : exp(exp::_con_LABEL), _v_name(p_name)
        { }
        ~LABEL ();
        static LABEL * make (LambdaVar::lvar p_name)
        {
            return new LABEL(p_name);
        }
        // pickler method suppressed
        LambdaVar::lvar get_name () const
        {
            return this->_v_name;
        }
        void set_name (LambdaVar::lvar v)
        {
            this->_v_name = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt);

      private:
        LambdaVar::lvar _v_name;
    };
    class NUM : public exp {
      public:
        NUM (asdl::integer p_iv, int p_sz)
          : exp(exp::_con_NUM), _v_iv(p_iv), _v_sz(p_sz)
        { }
        ~NUM ();
        static NUM * make (asdl::integer p_iv, int p_sz)
        {
            return new NUM(p_iv, p_sz);
        }
        // pickler method suppressed
        asdl::integer get_iv () const
        {
            return this->_v_iv;
        }
        void set_iv (asdl::integer v)
        {
            this->_v_iv = v;
        }
        int get_sz () const
        {
            return this->_v_sz;
        }
        void set_sz (int v)
        {
            this->_v_sz = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt);

      private:
        asdl::integer _v_iv;
        int _v_sz;
    };
    class LOOKER : public exp {
      public:
        LOOKER (CFG_Prim::looker * p_oper, std::vector<exp *> const & p_args)
          : exp(exp::_con_LOOKER), _v_oper(p_oper), _v_args(p_args)
        { }
        ~LOOKER ();
        static LOOKER * make (CFG_Prim::looker * p_oper, std::vector<exp *> const & p_args)
        {
            return new LOOKER(p_oper, p_args);
        }
        // pickler method suppressed
        CFG_Prim::looker * get_oper () const
        {
            return this->_v_oper;
        }
        void set_oper (CFG_Prim::looker * v)
        {
            this->_v_oper = v;
        }
        std::vector<exp *> get_args () const
        {
            return this->_v_args;
        }
        void set_args (std::vector<exp *> const & v)
        {
            this->_v_args = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt);

      private:
        CFG_Prim::looker * _v_oper;
        std::vector<exp *> _v_args;
    };
    class PURE : public exp {
      public:
        PURE (CFG_Prim::pure * p_oper, std::vector<exp *> const & p_args)
          : exp(exp::_con_PURE), _v_oper(p_oper), _v_args(p_args)
        { }
        ~PURE ();
        static PURE * make (CFG_Prim::pure * p_oper, std::vector<exp *> const & p_args)
        {
            return new PURE(p_oper, p_args);
        }
        // pickler method suppressed
        CFG_Prim::pure * get_oper () const
        {
            return this->_v_oper;
        }
        void set_oper (CFG_Prim::pure * v)
        {
            this->_v_oper = v;
        }
        std::vector<exp *> get_args () const
        {
            return this->_v_args;
        }
        void set_args (std::vector<exp *> const & v)
        {
            this->_v_args = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt);

      private:
        CFG_Prim::pure * _v_oper;
        std::vector<exp *> _v_args;
    };
    class SELECT : public exp {
      public:
        SELECT (int p_idx, exp * p_arg)
          : exp(exp::_con_SELECT), _v_idx(p_idx), _v_arg(p_arg)
        { }
        ~SELECT ();
        static SELECT * make (int p_idx, exp * p_arg)
        {
            return new SELECT(p_idx, p_arg);
        }
        // pickler method suppressed
        int get_idx () const
        {
            return this->_v_idx;
        }
        void set_idx (int v)
        {
            this->_v_idx = v;
        }
        exp * get_arg () const
        {
            return this->_v_arg;
        }
        void set_arg (exp * v)
        {
            this->_v_arg = v;
        }
        llvm::Value *codegen (smlnj::cfgcg::Context *cxt);

      private:
        int _v_idx;
        exp * _v_arg;
    };
    // exp_seq pickler suppressed
    std::vector<exp *> read_exp_seq (asdl::instream & is);
    class param {
      public:
        param (LambdaVar::lvar p_name, ty * p_ty)
          : _v_name(p_name), _v_ty(p_ty)
        { }
        ~param ();
        static param * make (LambdaVar::lvar p_name, ty * p_ty)
        {
            return new param(p_name, p_ty);
        }
        // pickler method suppressed
        static param * read (asdl::instream & is);
        LambdaVar::lvar get_name () const
        {
            return this->_v_name;
        }
        void set_name (LambdaVar::lvar v)
        {
            this->_v_name = v;
        }
        ty * get_ty () const
        {
            return this->_v_ty;
        }
        void set_ty (ty * v)
        {
            this->_v_ty = v;
        }
        void bind (smlnj::cfgcg::Context *cxt, llvm::Value *v) { cxt->insertVal (this->_v_name, v); }


      private:
        LambdaVar::lvar _v_name;
        ty * _v_ty;
    };
    // param_seq pickler suppressed
    std::vector<param *> read_param_seq (asdl::instream & is);
    using probability = int;
    class stm {
      public:
        virtual ~stm ();
        // pickler method suppressed
        static stm * read (asdl::instream & is);
        virtual void init (smlnj::cfgcg::Context *cxt, bool blkEntry) = 0;
        virtual void codegen (smlnj::cfgcg::Context *cxt) = 0;
        llvm::BasicBlock *bb () { return this->_bb; }


      protected:
        enum _tag_t {
            _con_LET = 1,
            _con_ALLOC,
            _con_APPLY,
            _con_THROW,
            _con_GOTO,
            _con_SWITCH,
            _con_BRANCH,
            _con_ARITH,
            _con_SETTER,
            _con_CALLGC,
            _con_RCC
        };
        stm (_tag_t tag)
          : _tag(tag)
        { }
        _tag_t _tag;
        llvm::BasicBlock *_bb;  // for the first stm in a block
        void _initBB (smlnj::cfgcg::Context *cxt, bool blkEntry);

    };
    class LET : public stm {
      public:
        LET (exp * p0, param * p1, stm * p2)
          : stm(stm::_con_LET), _v0(p0), _v1(p1), _v2(p2)
        { }
        ~LET ();
        static LET * make (exp * p0, param * p1, stm * p2)
        {
            return new LET(p0, p1, p2);
        }
        // pickler method suppressed
        exp * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (exp * v)
        {
            this->_v0 = v;
        }
        param * get_1 () const
        {
            return this->_v1;
        }
        void set_1 (param * v)
        {
            this->_v1 = v;
        }
        stm * get_2 () const
        {
            return this->_v2;
        }
        void set_2 (stm * v)
        {
            this->_v2 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        exp * _v0;
        param * _v1;
        stm * _v2;
    };
    class ALLOC : public stm {
      public:
        ALLOC (CFG_Prim::alloc * p0, std::vector<exp *> const & p1, LambdaVar::lvar p2, stm * p3)
          : stm(stm::_con_ALLOC), _v0(p0), _v1(p1), _v2(p2), _v3(p3)
        { }
        ~ALLOC ();
        static ALLOC * make (CFG_Prim::alloc * p0, std::vector<exp *> const & p1, LambdaVar::lvar p2, stm * p3)
        {
            return new ALLOC(p0, p1, p2, p3);
        }
        // pickler method suppressed
        CFG_Prim::alloc * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (CFG_Prim::alloc * v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        LambdaVar::lvar get_2 () const
        {
            return this->_v2;
        }
        void set_2 (LambdaVar::lvar v)
        {
            this->_v2 = v;
        }
        stm * get_3 () const
        {
            return this->_v3;
        }
        void set_3 (stm * v)
        {
            this->_v3 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        CFG_Prim::alloc * _v0;
        std::vector<exp *> _v1;
        LambdaVar::lvar _v2;
        stm * _v3;
    };
    class APPLY : public stm {
      public:
        APPLY (exp * p0, std::vector<exp *> const & p1, std::vector<ty *> const & p2)
          : stm(stm::_con_APPLY), _v0(p0), _v1(p1), _v2(p2)
        { }
        ~APPLY ();
        static APPLY * make (exp * p0, std::vector<exp *> const & p1, std::vector<ty *> const & p2)
        {
            return new APPLY(p0, p1, p2);
        }
        // pickler method suppressed
        exp * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (exp * v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        std::vector<ty *> get_2 () const
        {
            return this->_v2;
        }
        void set_2 (std::vector<ty *> const & v)
        {
            this->_v2 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        exp * _v0;
        std::vector<exp *> _v1;
        std::vector<ty *> _v2;
    };
    class THROW : public stm {
      public:
        THROW (exp * p0, std::vector<exp *> const & p1, std::vector<ty *> const & p2)
          : stm(stm::_con_THROW), _v0(p0), _v1(p1), _v2(p2)
        { }
        ~THROW ();
        static THROW * make (exp * p0, std::vector<exp *> const & p1, std::vector<ty *> const & p2)
        {
            return new THROW(p0, p1, p2);
        }
        // pickler method suppressed
        exp * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (exp * v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        std::vector<ty *> get_2 () const
        {
            return this->_v2;
        }
        void set_2 (std::vector<ty *> const & v)
        {
            this->_v2 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        exp * _v0;
        std::vector<exp *> _v1;
        std::vector<ty *> _v2;
    };
    class GOTO : public stm {
      public:
        GOTO (LambdaVar::lvar p0, std::vector<exp *> const & p1)
          : stm(stm::_con_GOTO), _v0(p0), _v1(p1)
        { }
        ~GOTO ();
        static GOTO * make (LambdaVar::lvar p0, std::vector<exp *> const & p1)
        {
            return new GOTO(p0, p1);
        }
        // pickler method suppressed
        LambdaVar::lvar get_0 () const
        {
            return this->_v0;
        }
        void set_0 (LambdaVar::lvar v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        LambdaVar::lvar _v0;
        std::vector<exp *> _v1;
    };
    class SWITCH : public stm {
      public:
        SWITCH (exp * p0, std::vector<stm *> const & p1)
          : stm(stm::_con_SWITCH), _v0(p0), _v1(p1)
        { }
        ~SWITCH ();
        static SWITCH * make (exp * p0, std::vector<stm *> const & p1)
        {
            return new SWITCH(p0, p1);
        }
        // pickler method suppressed
        exp * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (exp * v)
        {
            this->_v0 = v;
        }
        std::vector<stm *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<stm *> const & v)
        {
            this->_v1 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        exp * _v0;
        std::vector<stm *> _v1;
    };
    class BRANCH : public stm {
      public:
        BRANCH (CFG_Prim::branch * p0, std::vector<exp *> const & p1, probability p2, stm * p3, stm * p4)
          : stm(stm::_con_BRANCH), _v0(p0), _v1(p1), _v2(p2), _v3(p3), _v4(p4)
        { }
        ~BRANCH ();
        static BRANCH * make (CFG_Prim::branch * p0, std::vector<exp *> const & p1, probability p2, stm * p3, stm * p4)
        {
            return new BRANCH(p0, p1, p2, p3, p4);
        }
        // pickler method suppressed
        CFG_Prim::branch * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (CFG_Prim::branch * v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        probability get_2 () const
        {
            return this->_v2;
        }
        void set_2 (probability v)
        {
            this->_v2 = v;
        }
        stm * get_3 () const
        {
            return this->_v3;
        }
        void set_3 (stm * v)
        {
            this->_v3 = v;
        }
        stm * get_4 () const
        {
            return this->_v4;
        }
        void set_4 (stm * v)
        {
            this->_v4 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        CFG_Prim::branch * _v0;
        std::vector<exp *> _v1;
        probability _v2;
        stm * _v3;
        stm * _v4;
    };
    class ARITH : public stm {
      public:
        ARITH (CFG_Prim::arith * p0, std::vector<exp *> const & p1, param * p2, stm * p3)
          : stm(stm::_con_ARITH), _v0(p0), _v1(p1), _v2(p2), _v3(p3)
        { }
        ~ARITH ();
        static ARITH * make (CFG_Prim::arith * p0, std::vector<exp *> const & p1, param * p2, stm * p3)
        {
            return new ARITH(p0, p1, p2, p3);
        }
        // pickler method suppressed
        CFG_Prim::arith * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (CFG_Prim::arith * v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        param * get_2 () const
        {
            return this->_v2;
        }
        void set_2 (param * v)
        {
            this->_v2 = v;
        }
        stm * get_3 () const
        {
            return this->_v3;
        }
        void set_3 (stm * v)
        {
            this->_v3 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        CFG_Prim::arith * _v0;
        std::vector<exp *> _v1;
        param * _v2;
        stm * _v3;
    };
    class SETTER : public stm {
      public:
        SETTER (CFG_Prim::setter * p0, std::vector<exp *> const & p1, stm * p2)
          : stm(stm::_con_SETTER), _v0(p0), _v1(p1), _v2(p2)
        { }
        ~SETTER ();
        static SETTER * make (CFG_Prim::setter * p0, std::vector<exp *> const & p1, stm * p2)
        {
            return new SETTER(p0, p1, p2);
        }
        // pickler method suppressed
        CFG_Prim::setter * get_0 () const
        {
            return this->_v0;
        }
        void set_0 (CFG_Prim::setter * v)
        {
            this->_v0 = v;
        }
        std::vector<exp *> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<exp *> const & v)
        {
            this->_v1 = v;
        }
        stm * get_2 () const
        {
            return this->_v2;
        }
        void set_2 (stm * v)
        {
            this->_v2 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        CFG_Prim::setter * _v0;
        std::vector<exp *> _v1;
        stm * _v2;
    };
    class CALLGC : public stm {
      public:
        CALLGC (std::vector<exp *> const & p0, std::vector<LambdaVar::lvar> const & p1, stm * p2)
          : stm(stm::_con_CALLGC), _v0(p0), _v1(p1), _v2(p2)
        { }
        ~CALLGC ();
        static CALLGC * make (std::vector<exp *> const & p0, std::vector<LambdaVar::lvar> const & p1, stm * p2)
        {
            return new CALLGC(p0, p1, p2);
        }
        // pickler method suppressed
        std::vector<exp *> get_0 () const
        {
            return this->_v0;
        }
        void set_0 (std::vector<exp *> const & v)
        {
            this->_v0 = v;
        }
        std::vector<LambdaVar::lvar> get_1 () const
        {
            return this->_v1;
        }
        void set_1 (std::vector<LambdaVar::lvar> const & v)
        {
            this->_v1 = v;
        }
        stm * get_2 () const
        {
            return this->_v2;
        }
        void set_2 (stm * v)
        {
            this->_v2 = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        std::vector<exp *> _v0;
        std::vector<LambdaVar::lvar> _v1;
        stm * _v2;
    };
    class RCC : public stm {
      public:
        RCC (bool p_reentrant, std::string const & p_linkage, CTypes::c_proto * p_proto, std::vector<exp *> const & p_args, std::vector<param *> const & p_results, std::vector<param *> const & p_live, stm * p_k)
          : stm(stm::_con_RCC), _v_reentrant(p_reentrant), _v_linkage(p_linkage),
              _v_proto(p_proto), _v_args(p_args), _v_results(p_results), _v_live(p_live),
              _v_k(p_k)
        { }
        ~RCC ();
        static RCC * make (bool p_reentrant, std::string const & p_linkage, CTypes::c_proto * p_proto, std::vector<exp *> const & p_args, std::vector<param *> const & p_results, std::vector<param *> const & p_live, stm * p_k)
        {
            return new RCC(p_reentrant, p_linkage, p_proto, p_args, p_results, p_live,
                p_k);
        }
        // pickler method suppressed
        bool get_reentrant () const
        {
            return this->_v_reentrant;
        }
        void set_reentrant (bool v)
        {
            this->_v_reentrant = v;
        }
        std::string get_linkage () const
        {
            return this->_v_linkage;
        }
        void set_linkage (std::string const & v)
        {
            this->_v_linkage = v;
        }
        CTypes::c_proto * get_proto () const
        {
            return this->_v_proto;
        }
        void set_proto (CTypes::c_proto * v)
        {
            this->_v_proto = v;
        }
        std::vector<exp *> get_args () const
        {
            return this->_v_args;
        }
        void set_args (std::vector<exp *> const & v)
        {
            this->_v_args = v;
        }
        std::vector<param *> get_results () const
        {
            return this->_v_results;
        }
        void set_results (std::vector<param *> const & v)
        {
            this->_v_results = v;
        }
        std::vector<param *> get_live () const
        {
            return this->_v_live;
        }
        void set_live (std::vector<param *> const & v)
        {
            this->_v_live = v;
        }
        stm * get_k () const
        {
            return this->_v_k;
        }
        void set_k (stm * v)
        {
            this->_v_k = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool blkEntry);
        void codegen (smlnj::cfgcg::Context *cxt);


      private:
        bool _v_reentrant;
        std::string _v_linkage;
        CTypes::c_proto * _v_proto;
        std::vector<exp *> _v_args;
        std::vector<param *> _v_results;
        std::vector<param *> _v_live;
        stm * _v_k;
    };
    // stm_seq pickler suppressed
    std::vector<stm *> read_stm_seq (asdl::instream & is);
    enum class frag_kind {STD_FUN = 1, STD_CONT, KNOWN_FUN, INTERNAL};
    // pickler suppressed for frag_kind
    frag_kind read_frag_kind (asdl::instream & is);
    class frag {
      public:
        frag (frag_kind p_kind, LambdaVar::lvar p_lab, std::vector<param *> const & p_params, stm * p_body)
          : _v_kind(p_kind), _v_lab(p_lab), _v_params(p_params), _v_body(p_body)
        { }
        ~frag ();
        static frag * make (frag_kind p_kind, LambdaVar::lvar p_lab, std::vector<param *> const & p_params, stm * p_body)
        {
            return new frag(p_kind, p_lab, p_params, p_body);
        }
        // pickler method suppressed
        static frag * read (asdl::instream & is);
        frag_kind get_kind () const
        {
            return this->_v_kind;
        }
        void set_kind (frag_kind v)
        {
            this->_v_kind = v;
        }
        LambdaVar::lvar get_lab () const
        {
            return this->_v_lab;
        }
        void set_lab (LambdaVar::lvar v)
        {
            this->_v_lab = v;
        }
        std::vector<param *> get_params () const
        {
            return this->_v_params;
        }
        void set_params (std::vector<param *> const & v)
        {
            this->_v_params = v;
        }
        stm * get_body () const
        {
            return this->_v_body;
        }
        void set_body (stm * v)
        {
            this->_v_body = v;
        }
        void init (smlnj::cfgcg::Context *cxt);
        void codegen (smlnj::cfgcg::Context *cxt, cluster *cluster);
        llvm::BasicBlock *bb() const { return this->_v_body->bb(); }
        llvm::Type *paramTy (int i) const { return this->_phiNodes[i]->getType(); }
        void addIncoming (int i, llvm::Value *v, llvm::BasicBlock *bblk)
        {
            this->_phiNodes[i]->addIncoming(v, bblk);
        }


      private:
        frag_kind _v_kind;
        LambdaVar::lvar _v_lab;
        std::vector<param *> _v_params;
        stm * _v_body;
        std::vector<llvm::PHINode *> _phiNodes;

    };
    // frag_seq pickler suppressed
    std::vector<frag *> read_frag_seq (asdl::instream & is);
    class attrs {
      public:
        attrs (int p_alignHP, bool p_needsBasePtr, bool p_hasTrapArith, bool p_hasRCC)
          : _v_alignHP(p_alignHP), _v_needsBasePtr(p_needsBasePtr),
              _v_hasTrapArith(p_hasTrapArith), _v_hasRCC(p_hasRCC)
        { }
        ~attrs ();
        static attrs * make (int p_alignHP, bool p_needsBasePtr, bool p_hasTrapArith, bool p_hasRCC)
        {
            return new attrs(p_alignHP, p_needsBasePtr, p_hasTrapArith, p_hasRCC);
        }
        // pickler method suppressed
        static attrs * read (asdl::instream & is);
        int get_alignHP () const
        {
            return this->_v_alignHP;
        }
        void set_alignHP (int v)
        {
            this->_v_alignHP = v;
        }
        bool get_needsBasePtr () const
        {
            return this->_v_needsBasePtr;
        }
        void set_needsBasePtr (bool v)
        {
            this->_v_needsBasePtr = v;
        }
        bool get_hasTrapArith () const
        {
            return this->_v_hasTrapArith;
        }
        void set_hasTrapArith (bool v)
        {
            this->_v_hasTrapArith = v;
        }
        bool get_hasRCC () const
        {
            return this->_v_hasRCC;
        }
        void set_hasRCC (bool v)
        {
            this->_v_hasRCC = v;
        }
      private:
        int _v_alignHP;
        bool _v_needsBasePtr;
        bool _v_hasTrapArith;
        bool _v_hasRCC;
    };
    class cluster {
      public:
        cluster (attrs * p_attrs, std::vector<frag *> const & p_frags)
          : _v_attrs(p_attrs), _v_frags(p_frags)
        { }
        ~cluster ();
        static cluster * make (attrs * p_attrs, std::vector<frag *> const & p_frags)
        {
            return new cluster(p_attrs, p_frags);
        }
        // pickler method suppressed
        static cluster * read (asdl::instream & is);
        attrs * get_attrs () const
        {
            return this->_v_attrs;
        }
        void set_attrs (attrs * v)
        {
            this->_v_attrs = v;
        }
        std::vector<frag *> get_frags () const
        {
            return this->_v_frags;
        }
        void set_frags (std::vector<frag *> const & v)
        {
            this->_v_frags = v;
        }
        void init (smlnj::cfgcg::Context *cxt, bool isFirst);
        void codegen (smlnj::cfgcg::Context *cxt, bool isFirst);
        llvm::Function *fn () const { return this->_fn; }
        frag *entry () const { return this->_v_frags[0]; }


      private:
        attrs * _v_attrs;
        std::vector<frag *> _v_frags;
        llvm::Function *_fn;

    };
    // cluster_seq pickler suppressed
    std::vector<cluster *> read_cluster_seq (asdl::instream & is);
    class comp_unit {
      public:
        comp_unit (std::string const & p_srcFile, cluster * p_entry, std::vector<cluster *> const & p_fns)
          : _v_srcFile(p_srcFile), _v_entry(p_entry), _v_fns(p_fns)
        { }
        ~comp_unit ();
        static comp_unit * make (std::string const & p_srcFile, cluster * p_entry, std::vector<cluster *> const & p_fns)
        {
            return new comp_unit(p_srcFile, p_entry, p_fns);
        }
        // pickler method suppressed
        static comp_unit * read (asdl::instream & is);
        std::string get_srcFile () const
        {
            return this->_v_srcFile;
        }
        void set_srcFile (std::string const & v)
        {
            this->_v_srcFile = v;
        }
        cluster * get_entry () const
        {
            return this->_v_entry;
        }
        void set_entry (cluster * v)
        {
            this->_v_entry = v;
        }
        std::vector<cluster *> get_fns () const
        {
            return this->_v_fns;
        }
        void set_fns (std::vector<cluster *> const & v)
        {
            this->_v_fns = v;
        }
        void codegen (smlnj::cfgcg::Context *cxt);

      private:
        std::string _v_srcFile;
        cluster * _v_entry;
        std::vector<cluster *> _v_fns;
    };
} // namespace CFG
