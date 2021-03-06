#include	<unistd.h>
#include	"buffer.h"

// TODO make public and .h
void *
rdfl_flush_all_alloc(t_rdfl *obj, ssize_t *count_value) {
  void		*ptr;

  if (!(ptr = b_consume_all_alloc(&obj->data, count_value))) {
    return (NULL);
  }
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    b_fullclean_if_empty(&obj->data);
  return (ptr);
}

void *
rdfl_flush_firstbuffer_alloc(t_rdfl *obj, ssize_t *count_value) {
  void		*ptr;

  if (!(ptr = b_consume_firstbuffer_alloc(&obj->data, count_value)))
    return (NULL);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    b_fullclean_if_empty(&obj->data);
  return (ptr);
}
