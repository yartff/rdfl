#ifndef			__RDFL_CONSUMER_H_
# define		__RDFL_CONSUMER_H_

# include		<unistd.h>
# include		"rdfl_local.h"
# include		"rdfl.h"

// TODO implement a consumer engine
typedef		struct {
  // Captures
  // Context
  // csm.opt | opt ?
}		t_rdfl_csm;
typedef				enum {
  RDFL_CSM_NONE			= 0,
  RDFL_CSM_AUTOCLEAR_BLANKS	= 1 << 0,
  // either you put tokens in check order
  RDFL_CSM_ALLOW_CONTEXTS	= 1 << 1,
  // RDFL_P_BUILD_TREE		= 1 << 2,
  RDFL_CSM_LAST			= 1 << 3,
}				e_csm_options;

// READ_AS_TOKEN

// consumer

ssize_t		rdfl_csm_number(t_rdfl *, int);
ssize_t		rdfl_csm_str(t_rdfl *, const char *);
ssize_t		rdfl_csm_float(t_rdfl *, float);

// consumer_type
typedef		enum {
  // "\n \" \etc"
  RDFL_PSTR_INTERPRET_BS		= RDFL_P_LAST << 1,
  // "\xeb" "\xa" TODO
  RDFL_PSTR_INTERPRET_NONTYPABLE	= RDFL_P_LAST << 2,
  // "abcd" "efgh"
  RDFL_PSTR_CONCAT_CSTYLE		= RDFL_P_LAST << 3,
  // "ab"cd'ef'gh OR 'ab'"cd"ef'gh'
  RDFL_PSTR_CONCAT_SHSTYLE		= RDFL_P_LAST << 4,
  // 'abcd' -- ignored if SHSTYLE
  RDFL_PSTR_SIMPLE_QUOTE_STR		= RDFL_P_LAST << 5,
  RDFL_PSTR_EXTRACT_INNER		= RDFL_P_LAST << 6
}		e_bacc_str;
ssize_t		rdfl_ct_readString(t_rdfl *, void **extract, e_bacc_options);

// probably need opt too
ssize_t		rdfl_ct_readChar(t_rdfl *, void **extract, e_bacc_options);

long		rdfl_ct_readInteger(t_rdfl *, void **extract, e_bacc_options);

typedef		enum {
  // standard C limits its identifiers to 31 characters.
  // data outside that boundary will be discarded
  RDFL_PIDT_C_LIMIT			= RDFL_P_LAST << 1
}		e_bacc_idt;
ssize_t		rdfl_ct_readIdentifier(t_rdfl *, void **extract, e_bacc_options);

// BNF
int		rdfl_readBNF(t_rdfl *);

#endif
