#ifndef		__OPT_H_
# define	__OPT_H_

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

void		print_opt(void);
int		init_opt(int l_argc, char **l_argv);
int		opt_isset(char opt);
char		*opt_arg(char opt);
const char	**opt_args(void);
void		opt_helpopt(char opt);
void		clean_opt(void);

# define	OPT_M_DEB	"debug"
# define	OPT_M_DEV	"devel"
# define	OPT_M_BOTH	"both"
# define	OPT_M_ADD_OPTI	"+o"

#endif		/* !__OPT_H_ */
