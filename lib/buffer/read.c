#include	"values.h"
#include	"buffer_read.h"

#ifdef DEVEL
# include	<limits.h>
#endif

// buffer.ndx should not got back to 0 when not on the consumer buffer
static
void
addsize(t_rdfl_buffer *b, size_t value) {
  if (!(b->consumer.raw->next))
    b->consumer.l_total += value;
  b->consumer.total += value;
  if ((b->buffer.ndx += value) == b->buffer.raw->size && !b->consumer.raw->next)
    b->buffer.ndx = 0;
}

ssize_t
b_push_read(t_rdfl_buffer *b, int fd, void *ptr, size_t s) {
  ssize_t	nb;

#ifdef		DEVEL
  if (s > SSIZE_MAX) return (ERRDEV_SIZETOOBIG);
#endif
  if ((nb = read(fd, ptr, s)) == -1) {
    // TODO read must check for following errors:
    // EAGAIN E_WOULDBLOCK
    return (ERR_READ);
  }
  addsize(b, (size_t)nb);
  return (nb);
}

// Read wrappers utils
ssize_t
push_read(t_rdfl *obj, void *ptr, size_t available) {
  ssize_t	ret;

  if (RDFL_OPT_ISSET(obj->settings, LOCAL_REACHED_EOF))
    return (0);
  ret = b_push_read(&obj->data, obj->fd, ptr, available);
  if (ret <= 0)
    _close_fd(obj);
  return (ret);
}

ssize_t
read_into_chunk_extend(void *obj) {
  void		*ptr;
  size_t	s;

  if (!(ptr = b_buffer_getchunk_extend(&((t_rdfl *)obj)->data, &s,
	  ((t_rdfl *)obj)->v.buffsize)))
    return (ERR_MEMORY_ALLOC);
  return (push_read(obj, ptr, s));
}

ssize_t
read_into_chunk(void *obj) {
  void		*ptr;
  size_t	s;

  ptr = b_buffer_getchunk(&((t_rdfl *)obj)->data, &s);
  if (!s)
    return (ERR_NOSPACELEFT);
  return (push_read(obj, ptr, s));
}

