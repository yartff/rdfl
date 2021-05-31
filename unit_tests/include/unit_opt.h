#ifndef		__UNIT_OPT_H_
# define	__UNIT_OPT_H_

# include	"unit_internal.h"

typedef struct	{
  int		g_argc;
  char		**g_argv;
  int		result;
}		t_opt_data;

typedef struct	{
  int		on;
  void		*arg;
  int		ndx;
  int		len;
}		t_runtime;

# define DEF_RT_VAL	{0, NULL, -1, 0}

typedef struct	{
  char		opt;
  const char	*arg;
  const char	*desc;
  int		cumulative;
  t_runtime	rt;
}		t_opt;

e_unit_return_code	init_opt(int l_argc, char **l_argv);
void			opt_print(void);
int			opt_isset(char opt);
const char		**opt_arg(char opt, unsigned int *);
const char		**opt_args(void);
void			opt_helpopt(char opt);
void			clean_opt(void);

# define	OPT_M_SET_DEB	"debug"
# define	OPT_M_SET_DEV	"devel"
# define	OPT_M_SET_BOTH	"both"
# define	OPT_M_ADD_OPTI	"+o"

#endif		/* !__UNIT_OPT_H_ */
