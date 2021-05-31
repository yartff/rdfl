#include	<stdlib.h>
#include	<stdio.h>
#include	"unit_lib.h"

int	T_b_buffer_ptr_extend__1(void) {
  size_t s;
  t_rdfl *obj = get_mock();
  b_buffer_ptr_extend(&obj->data, &s, 60);
  l__rdflDevel_printbufferstate(obj);
}
