#include	"rdfl.h"
#include	"buffer.h"
#include	"network.h"

// Readers
//

ssize_t
rdflReader_size(t_rdfl *obj, size_t count) {
  void		*ptr;
  ssize_t	s;
  size_t	available, total = count;

  while (count) {
    if (!(ptr = b_buffer_ptr_extend(&obj->data, &available,
	    (RDFL_OPT_ISSET(obj->settings, RDFL_ADJUST_BUFFSIZE)
	     ? count : (size_t)obj->v.buffsize))))
      return (ERR_MEMORY_ALLOC);
    if (available > count) available = count;
    if ((s = push_read(obj, ptr, available)) < 0)
      return (s);
    if (!s || s != ((ssize_t)available))
      return (total - (count - s));
    count -= (size_t)s;
  }
  return (total);
}

ssize_t
rdflReader_allavail(t_rdfl *obj, e_rdflerrors *err) {
  size_t	available;
  ssize_t	full = 1, total = 0;
  void		*ptr;

  while (full) {
    if (!(ptr = b_buffer_ptr_extend(&obj->data, &available, obj->v.buffsize))) {
      if (err) {
	*err = ERR_MEMORY_ALLOC;
	return (total);
      }
      return (ERR_MEMORY_ALLOC);
    }
    if ((full = push_read(obj, ptr, available)) <= 0) {
      if (err) {
	*err = (!full ? VAL_LAST_READ_0 : full);
	return (total);
      }
      if (!full || full < VAL || full == ERR_CONNECTION_CLOSED)
	return (total);
      return (full);
    }
    total += full;
    full = ((size_t)full == available);
  }
  return (total);
}

ssize_t
rdflReader_noextend(t_rdfl *obj, size_t consume) {
  size_t	s;
  ssize_t	total;
  void		*ptr;

  b_consume_size(&obj->data, consume);
  ptr = b_buffer_ptr(&obj->data, &s);
  if (!s) { return (ERR_NOSPACELEFT); }
  if ((total = push_read(obj, ptr, s)) < 0)
    return (total);

// TODO
#if 0
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FILLFREESPACE) && total == s) {
    ptr = b_buffer_ptr(&obj->data, &s);
    if (!s) return ((size_t)ret);
    if ((ret = push_read(&obj, ptr, s)) < 0) {
      if (err) *err = ret;
      return (total);
    }
  }
#endif

  return (total);
}

ssize_t
rdflReader_legacy(t_rdfl *obj, void *buf, size_t count) {
  (void)obj, (void)buf, (void)count;
  return (0);
}

ssize_t
rdflReader_monitoring(t_rdfl *obj) {
  return (nw_monitoring(obj->nw, &read_into_chunk_extend, obj));
}

ssize_t
rdflReader_monitoring_noextend(t_rdfl *obj) {
  return (nw_monitoring(obj->nw, &read_into_chunk, obj));
}

ssize_t
rdflReader_monitoring_allavail(t_rdfl *obj, e_rdflerrors *err) {
  ssize_t	total = 0;
  nwret_t	nw_ret;

  while ((nw_ret = nw_monitoring(obj->nw, &read_into_chunk_extend, obj)) > 0)
    total += nw_ret;
  if (nw_ret <= 0) {
    if (err) {
      *err = (nw_ret ? nw_ret : VAL_LAST_READ_0);
      return (total);
    }
    return (nw_ret > VAL ? nw_ret : total);
  }
  return (total);
}

ssize_t
rdflReader_singlestep(t_rdfl *obj) {
  void		*ptr;
  size_t	available;

  if (!(ptr = b_buffer_ptr_extend(&obj->data, &available,
	  (size_t)obj->v.buffsize)))
    return (ERR_MEMORY_ALLOC);
  return (push_read(obj, ptr, available));
}
