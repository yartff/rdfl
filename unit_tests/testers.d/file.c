#include	<stdio.h>
#include	"file.h"
#include	"tree.h"

typedef struct {
  const char *name;
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

/*
typedef struct	{
  char		*path;
  struct	{
    t_fct	fcts[];
  }		content;
}		t_file;
*/
