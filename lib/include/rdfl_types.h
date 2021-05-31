#ifndef		__RDFL_TYPES_H_
# define	__RDFL_TYPES_H_

# include	<sys/select.h>
# include	"rdfl_types_buffer.h"
# include	"rdfl_status_codes.h"

typedef struct		s_comments {
  char			*beg;
  char			*end;
  struct s_comments	*next;
}			t_comments;

typedef struct		{
  ssize_t		timeout;
  ssize_t		buffsize;
  t_comments		*cmts;
}			t_rdfl_values;

typedef			struct {
  struct timeval	*timeout;
  fd_set		fds;
  int			fd_select;
}			t_rdfl_net;

typedef struct		{
  t_rdfl_net		*nw;
  t_rdfl_values		v;
  e_rdflsettings	settings;
  int			fd;
  t_rdfl_buffer		data;
}			t_rdfl;

typedef	ssize_t		nwret_t;

# ifdef			DEVEL
#  define rdfl_retval	e_rdflerrors
# else
#  define rdfl_retval	void
# endif

#endif		/* !__RDFL_TYPES_H_ */
