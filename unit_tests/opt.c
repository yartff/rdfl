#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<getopt.h>
#include	"unit_opt.h"

static struct		{
  int		g_argc;
  char		**g_argv;
  int		result;
}		g_opt = {
  0, NULL, EXIT_SUCCESS
};

t_opt		opts[] = {
  {'h', NULL, "Prints this usage", 0, DEF_RT_VAL},
  {'c', "category", "Tests category.", 1, DEF_RT_VAL},
  {'l', NULL, "Lists files.", 1, DEF_RT_VAL},
  {'a', NULL, "Tests all files.", 0, DEF_RT_VAL},
  // {'e', NULL, "Only output errors.", 0, DEF_RT_VAL},
  {'m', "debug|devel|both [" OPT_M_ADD_OPTI "]", "Execute tests with compile type presets."
    " Append '" OPT_M_ADD_OPTI "' for optimisation mode", 0, DEF_RT_VAL},
};

static void
foreach_opt(int (*f)(t_opt *, void *), void *data) {
  for (unsigned int i = 0; i < (sizeof(opts) / sizeof(*opts)); ++i) {
    if (f(&opts[i], data) == EXIT_FAILURE)
      return ;
  }
}

typedef struct	{
  char		opt;
  t_opt		*ret;
}		t__get_opt;

static
int
get_opt_f(t_opt *ctn, void *tdata) {
  t__get_opt	*data = (t__get_opt *)tdata;
  if (ctn->opt == data->opt) {
    data->ret = ctn;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

static
t_opt
*get_opt(char opt) {
  t__get_opt	data;
  data.opt = opt;
  data.ret = NULL;
  foreach_opt(get_opt_f, &data);
  return (data.ret);
}

static
t_runtime
*get_rt(char opt) {
  t_opt		*data = get_opt(opt);

  if (data)
    return (&data->rt);
  return (NULL);
}

static
int
help_f(t_opt *ctn, void *data) {
  if (!data || (data && ctn->opt == *(char *)data)) {
    fprintf(stderr, "  -%c", ctn->opt);
    if (ctn->arg != NULL)
      fprintf(stderr, " [%s]", ctn->arg);
    if (ctn->desc != NULL)
      fprintf(stderr, ": %s", ctn->desc);
    fprintf(stderr, "\n");
    if (data) return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

static
int
setopt_f(t_opt *ctn, void *data) {
  if (*(char *)data != ctn->opt)
    return (EXIT_SUCCESS);
  ctn->rt.on = 1;
  if (ctn->arg) {
    if (ctn->cumulative) {
      if (!ctn->rt.arg)
	ctn->rt.arg = (char **)malloc((g_opt.g_argc - (optind - 1)) * sizeof(char *));
      ((char **)ctn->rt.arg)[ctn->rt.len] = g_opt.g_argv[optind - 1];
      ++ctn->rt.len;
    }
    else
      ctn->rt.arg = optarg;
  }
  return (EXIT_FAILURE);
}

static
void
unit_setopt(char opt) {
  foreach_opt(setopt_f, &opt);
}

typedef struct	{
  int	i;
  char	*optstring;
}		t__optstring;

static
int
help_optstring_f(t_opt *ctn, void *tdata) {
  t__optstring	*data = (t__optstring *)tdata;
  data->optstring[data->i++] = ctn->opt; 
  if (ctn->arg)
    data->optstring[data->i++] = ':';
  return (EXIT_SUCCESS);
}

static
const char	*
help_optstring() {
  static char	optstring[(sizeof(opts) / sizeof(*opts)) * 2 + 1];
  t__optstring	optbuild = {
    0, optstring
  };
  foreach_opt(help_optstring_f, &optbuild);
  optbuild.optstring[optbuild.i] = 0;
  return (optbuild.optstring);
}

/*
 ** CLEAN
 */
static
int
clean_opt_f(t_opt *ctn, void *data) {
  (void)data;
  if (ctn->cumulative) {
    free(ctn->rt.arg);
  }
  return (EXIT_SUCCESS);
}

void
clean_opt(void) {
  foreach_opt(clean_opt_f, NULL);
}

/*
 ** PRINT
 */

static
int
print_opt_f(t_opt *ctn, void *data) {
  (void)data;
  if (ctn->rt.on) {
    fprintf(stdout, "-%c", ctn->opt);
    if (ctn->arg) {
      if (ctn->cumulative)
	for (int i = 0; i < ctn->rt.len; ++i)
	  fprintf(stdout, " \"%s\"", ((char **)ctn->rt.arg)[i]);
      else
	fprintf(stdout, " \"%s\"", ctn->rt.arg);
    }
    fprintf(stdout, "\n");
  }
  return (EXIT_SUCCESS);
}

void
print_opt(void) {
  foreach_opt(print_opt_f, NULL);
  /*
  fprintf(stdout, "===\n");
  unsigned int i = 0;
  while (i < g_opt.g_argc) {
    if (i == optind)
      fprintf(stdout, "---\n");
    fprintf(stdout, "-> %s\n", g_opt.g_argv[i]);
    ++i;
  }
  */
  unsigned int i = optind;
  fprintf(stdout, "===\n");
  while (i < g_opt.g_argc) {
    fprintf(stdout, "-> %s\n", g_opt.g_argv[i]);
    ++i;
  }
}

/*
 ** Get
 */

int
opt_isset(char opt) {
  t_runtime	*rt = get_rt(opt);
  return (rt ? rt->on : 0);
}

const char	**
opt_arg(char opt, unsigned int *len) {
  t_opt		*obj = get_opt(opt);
  if (!obj || !obj->rt.on) {
    if (len) *len = 0;
    return (NULL);
  }
  if (obj->cumulative) {
    if (len) *len = obj->rt.len;
    return (const char **)obj->rt.arg;
  }
  if (len) *len = 1;
  return (((const char **)&obj->rt.arg));
}

const char **
opt_args(void) {
  return ((const char **)(g_opt.g_argv + optind));
}

void
opt_helpopt(char opt) {
  fprintf(stderr, "Usage for -%c:\n", opt);
  foreach_opt(help_f, &opt);
  // exit(EXIT_FAILURE);
}

void
help(void) {
  fprintf(stderr, "Usage: unit [OPTION]... [FILE]...\n");
  foreach_opt(help_f, NULL);
}

int
init_opt(int l_argc, char **l_argv) {
  int		i;
  const char	*optstring = help_optstring();

  opterr = 1;
  optind = 1;
  g_opt.g_argc = l_argc;
  g_opt.g_argv = l_argv;
  while ((i = getopt(l_argc, l_argv, optstring)) != -1) {
    if (i == '?')
      return (EXIT_FAILURE);
    unit_setopt(i);
  }
  if (opt_isset('h')) {
    help();
    exit(EXIT_SUCCESS);
  }
  return (EXIT_SUCCESS);
}
