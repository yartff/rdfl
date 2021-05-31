#include	<stdio.h>
#include	"unit_file.h"
#include	"unit_opt.h"

#include	"tree/unit_tree.h"

typedef struct {
  const char *name;
  // category
  int		(*f)(void);
}		t_func;

typedef struct	{
  const char	*name;
  t_file	files[];
}		t_dir;

typedef struct	{
  const char	*name;
  t_func	funcs[];
}		t_file;


#define CUR	ALL
t_func		

t_dir		tree = {
  "unit_tests",
  t_func[] = {
  },
}

t_file		g_files[] = {
  {"buffer/buffer.d/b_buffer_ptr_extend"}
};



void
_exec(const char *file) {
  fprintf(stdout, "[EXE]     file.%s\n", file);
  T_b_buffer_ptr_extend__1();
}

void
execute_files(void) {
  unsigned int len;
  const char **f = opt_arg('f', &len);
  if (!f) return ;
  for (unsigned int i = 0; i < len; ++i) {
    _exec(f[i]);
  }
}
