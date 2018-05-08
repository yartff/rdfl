#ifndef			__RDFL__DATA_CONTAINER_H_
# define		__RDFL__DATA_CONTAINER_H_

# include		"rdfl_buffer.h"
# include		"network.h"

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

typedef struct		{
  t_rdfl_net		*nw;
  t_rdfl_values		v;
  e_rdflsettings	settings;
  int			fd;
  t_rdfl_buffer		data;
}			t_rdfl;

void			_close_fd(t_rdfl *);
void			_comments_clean(t_comments *);

#endif			/* !__RDFL__DATA_CONTAINER_H_ */
