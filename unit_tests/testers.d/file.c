#include	<stdio.h>
#include	"unit_file.h"
#include	"unit_tree.h"

typedef struct {
  const char *name;
  // category
  int		(*f)(void);
}		t_func;

typedef struct	{
  const char	*name;
  t_func	funcs[];
}		t_file;

t_file		g_files[] = {
  {"buffer/buffer.d/b_buffer_ptr_extend"}
};

void
exec_file(const char *file) {
  T_b_buffer_ptr_extend__1();
}
