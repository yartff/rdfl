#ifndef		__RDFL_READBNF_H_
# define	__RDFL_READBNF_H_

# include		"rdfl.h"

typedef			enum {
  FACT_RULE		= 0,
  FACT_RULE_LINKED,
  FACT_RULE_BUILTIN,
  FACT_LITERAL,
  // help with contexts?
  // EXPRS
  FACT_EXPRS		= 30,
  FACT_EXPR_OPT,
  FACT_EXPR_NGT,
  FACT_EXPR_NGTE
}			e_fact_type;

// TODO when consuming the second stream
// ( 'a' 'b' | 'a' 'c' )
// what if consumes 'a'? do I really need to use contexts for these?
// even worse:
// ( ">>" ">" identifier | ">>>" integer)

typedef struct		sl_fact {
  e_fact_type		type;
  struct sl_fact	*next;
  // Either one of these:
  // - t_rdfl_bnf	*;
  // - char		*;
  // - tl_orexpr	*;
  void			*target;
}			tl_fact;

typedef struct		sl_orexpr {
  tl_fact		*factors;
  struct sl_orexpr	*next;
}			tl_orexpr;

typedef struct		sl_param {
  char			*type;
  char			*id;
  struct sl_param	*next;
}			tl_param;

typedef struct 		sl_prod {
  char			*identifier;
  tl_param		*params;
  struct sl_prod	*next;
  tl_orexpr		*exprs;
}			t_rdfl_bnf;

// BNF
t_rdfl_bnf		*rdfl_readBNF(t_rdfl *);
void			rdfl_freeBNF(t_rdfl_bnf *);
ssize_t			rdfl_applyBNF(t_rdfl_bnf *, t_rdfl *, char *);
t_rdfl_bnf		*_seek_target(t_rdfl_bnf *, char *);
void			_free_param_list(tl_param *);
tl_param		*read_params(t_rdfl *, e_rdflerrors *);

# ifdef			DEVEL
void			rdfl_readBNF_dump(t_rdfl_bnf *);
# endif

#define			READ_OPE(obj, c, opt)	(rdfl_acc_cmp(obj, c, sizeof(c) - 1, opt) > 0)
#define			OP_RULE			"::="
#define			OP_RULE_END		";"
#define			OP_OR			"|"
#define			OP_EXPR_BEG		"["
#define			OP_EXPR_END		"]"
#define			OP_GRP_BEG		"("
#define			OP_GRP_END		")"
static const e_acc_options	OPTS = RDFL_P_NULLTERMINATED | RDFL_P_CONSUME;

#endif		/* !__RDFL_READBNF_H_ */
