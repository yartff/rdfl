#ifdef NOPE

public:
├── rdfl_consumer.h  rdfl.h               rdfl_types_buffer.h
└── rdfl_devel.h     rdfl_status_codes.h  rdfl_types.h

rdfl:
└── /* context.h */ clean.h  network.h  values.h
clean.c   devel.c  extract.c  network.c  rdfl.c

bnf:
└── bnf.h
rdfl_bnf.c  rdfl_bnf_dump.c  rdfl_bnf_params.c  rdfl_bnf_run.c

buffer:
└── buffer.h  buffer_access.h  iterate.h
buffer.c         consume.c	   read.c
buffer_access.c  buffer_iterate.c  /* rdfl_context.c */
accessors ───    compare.c	   get_content.c
consumers ───    identifier.c	   match_any.c    read_until.c  string.c

// TODO: buffer.h and buffer.c
// TODO: BACC_* in buffer.h


// grep
//
ssize_t			_handle_predata(opt);
// CANCEL(RDFL_P_CONSUME)
// rdfl_acc_cmp(opt);
// rdfl_csm_readUntil(opt | RDFL_P_IGNORE_PREDATASKIP)
// rdfl_csm_readMatchAny(opt);

int			_iterate_chunk(opt);
// RDFL_P_IGNORE_PREDATASKIP
// _handle_predata(opt)

int			_iterate_extract(opt);
// rdfl_acc_getcontent(opt | RDFL_P_IGNORE_PREDATASKIP)
// if (RDFL_P_CONSUME) b_consume_size();

-> RDFL_P_CONSUME, RDFL_P_IGNORE_PREDATASKIP

void			*rdfl_acc_getcontent(t_rdfl *, ssize_t *, size_t c, e_acc_options);
// -> if `c` == 0 -> all
// if (RDFL_P_NULLTERMINATED)
// iterate_chunk(e)

int			rdfl_acc_cmp(opt);
// _iterate_chunk(opt);
// _iterate_extract(opt);

size_t			rdfl_acc_info_total();

int			rdfl_acc_ndx();
// _iterate_chunk(0);

ssize_t			rdfl_csm_readString(opt);
// RDFL_PSTR_SIMPLE_QUOTE_STR
// _iterate_chunk(opt);
// _iterate_extract(opt);

ssize_t			rdfl_csm_readIdentifier(opt);
// _iterate_chunk(opt);
// _iterate_extract(opt);

ssize_t			rdfl_csm_readUntil(opt);
// _iterate_chunk(opt);
// _iterate_extract(opt);

ssize_t			rdfl_csm_readMatchAny(opt);
// _iterate_chunk(opt);
// _iterate_extract(opt);


#endif

#ifndef		__UNIT_FILE_H_
# define	__UNIT_FILE_H_

# include	"decl.h"

void		exec_file(const char *);

#endif		/* !__UNIT_FILE_H_ */
