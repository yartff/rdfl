#ifndef		__RDFL_CONTEXT_H_
# define	__RDFL_CONTEXT_H_

# include	"rdfl.h"

// Contexts
void		rdfl_dropallcontexts(t_rdfl *);
int		rdfl_pushcontext(t_rdfl *, size_t ndx);
int		rdfl_dropcontexts(t_rdfl *, size_t ndx);
// returns the new last context
void		rdfl_popcontext(t_rdfl *);
void		rdfl_restaurecontext(t_rdfl *);
void		rdfl_shiftall(t_rdfl *, size_t);

int		rdfl_context_init(t_rdfl_buffer *);
int		rdfl_shiftcontext(t_rdfl *obj, size_t n);

# endif		/* !__RDFL_CONTEXT_H_ */
