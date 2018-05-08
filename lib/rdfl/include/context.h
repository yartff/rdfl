#ifndef			__RDFL_CONTEXT_H_
# define		__RDFL_CONTEXT_H_

# include		"rdfl.h"

typedef	ssize_t		ctxid_t;
typedef struct		s_ctxstack {
  ssize_t		value; // TODO ssize_t?
  ctxid_t		id;
  struct s_ctxstack	*next;
}			t_ctxstack;

typedef struct		{
  size_t		total;
  ctxid_t		id;
  t_ctxstack		*stack;
}			t_rdfl_contexts;

/* TODO: ctx
rdfl_retval		rdfl_ctx_popuntil(t_rdfl_buffer *, ctxid_t);
rdfl_retval		rdfl_dropallcontexts(t_rdfl_buffer *);
*/

/* TODO: ctx, distinct .h
int			rdfl_context_init(t_rdfl_buffer *);
void			rdfl_context_consume(t_rdfl_buffer *, size_t);
int			rdfl_restaurecontext(t_rdfl *, ssize_t);
ssize_t			rdfl_pushcontext(t_rdfl *);
*/

# endif			/* !__RDFL_CONTEXT_H_ */
