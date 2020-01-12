#include	"rdfl.h"
#include	"buffer.h"

// Utils
//

inline int	rdfl_eofreached(t_rdfl *obj)
{ return (RDFL_OPT_ISSET(obj->settings, LOCAL_EOF)); }

void
rdfl_consume_size(t_rdfl *obj, size_t s) {
  b_consume_size(&obj->data, s);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    b_fullclean_if_empty(&obj->data);
}

inline
void *
rdfl_getinplace_next_chunk(t_rdfl *obj, size_t *s, size_t *total_s) {
  void		*ptr;

  ptr = b_consumer_ptr(&obj->data, s);
  if (total_s) *total_s = obj->data.consumer.total;
  return (ptr);
}

#if 0
ssize_t
rdfl_read_ignore_size(t_rdfl *obj, size_t s) {
  (void)obj, (void)s; // if monitoring etc...
  return (-1);
}
#endif
