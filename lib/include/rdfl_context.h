#ifndef		__RDFL_CONTEXT_H_
# define	__RDFL_CONTEXT_H_

# include	"rdfl.h"

int		rdfl_restaurecontext(t_rdfl *, ssize_t);
ssize_t		rdfl_pushcontext(t_rdfl *);
void		rdfl_ctx_popuntil(t_rdfl_buffer *, ctxid_t);
void		rdfl_dropallcontexts(t_rdfl_buffer *);

int		rdfl_context_init(t_rdfl_buffer *);
void		rdfl_context_consume(t_rdfl_buffer *, size_t);

# endif		/* !__RDFL_CONTEXT_H_ */
