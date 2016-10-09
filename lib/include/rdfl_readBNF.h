#ifndef		__RDFL_READBNF_H_
# define	__RDFL_READBNF_H_

typedef			enum {
  FACT_RULE		= 0,
  FACT_RULE_LINKED,
  FACT_LITERAL,
  // help with contexts?
  FACT_EXPR_OPT,
  FACT_EXPR_REP,
  FACT_EXPR_GROUP
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

typedef			struct sl_orexpr {
  tl_fact		*factors;
  struct sl_orexpr	*next;
}			tl_orexpr;

typedef struct 		sl_prod {
  char			*identifier;
  struct sl_prod	*next;
  tl_orexpr		*exprs;
}			t_rdfl_bnf;

// BNF
t_rdfl_bnf	*rdfl_readBNF(t_rdfl *);
void		rdfl_freeBNF(t_rdfl_bnf *);
// #ifdef		DEVEL
void		rdfl_readBNF_dump(t_rdfl_bnf *);
//#endif
ssize_t		rdfl_applyBNF(t_rdfl_bnf *, t_rdfl *, char *);
t_rdfl_bnf	*_seek_target(t_rdfl_bnf *, char *);

#endif		/* !__RDFL_READBNF_H_ */
