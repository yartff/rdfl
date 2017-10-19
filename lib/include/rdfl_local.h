#ifndef			__RDFL_LOCAL_H_
# define		__RDFL_LOCAL_H_

# include		"rdfl.h"
# include		"rdfl_status_codes.h"
# include		"rdfl_bnf.h"

// rdfl
# define		RDFL_DEFAULT_BUFFSIZE	4096 // (bytes)
# define		RDFL_DEFAULT_TIMEOUT	-1 // (blocking reads)
typedef			enum {
  RDFL_LOC_NONE		=	RDFL_LAST,
  RDFL_LOC_OPEN		=	RDFL_LOC_NONE << 1,
  RDFL_LOC_ALLOC	=	RDFL_LOC_NONE << 2,
  RDFL_LOC_REACHED_EOF	=	RDFL_LOC_NONE << 3,
}			e_rdflsettings_local;

// rdfl_buffer
enum {
  BACC_CB_STOP		= 0,
  BACC_CB_CONTINUE,
  BACC_CB_NEEDDATA,
  BACC_CB_NOACTION,
};
int			_iterate_chunk(t_rdfl *, int (*)(void *, size_t, void *), void *, e_bacc_options opt);
int			_iterate_extract(t_rdfl *, void **, ssize_t, e_bacc_options);
void			*rdfl_b_next_chunk(t_rdfl_buffer *, size_t *);
tl_param		*read_params(t_rdfl *, e_rdflerrors *);

// rdfl_bnf
#define			READ_OPE(obj, c, opt)	(rdfl_bacc_cmp_needdata(obj, c, sizeof(c) - 1, opt) > 0)
#define			OP_RULE			"::="
#define			OP_RULE_END		";"
#define			OP_OR			"|"
#define			OP_EXPR_BEG		"["
#define			OP_EXPR_END		"]"
#define			OP_GRP_BEG		"("
#define			OP_GRP_END		")"
static const e_bacc_options	OPTS = RDFL_P_NULLTERMINATED | RDFL_P_CONSUME;

#endif			/* !__RDFL_LOCAL_H_ */
