#include		<string.h>
#include		<stdlib.h>
#include		"rdfl_local.h"
#include		"rdfl_buffer_access.h"

static
ssize_t // TODO how to return errors there? glstatic?
_iterate_readdata(t_rdfl *obj) {
  if (RDFL_OPT_ISSET(obj->settings, RDFL_NO_EXTEND))
    return (_read_noextend(obj, 0));
  return (_read_singlestep(obj));
}

inline
static
void *
_rdfl_b_rewind_localptr(t_rdfl_buffer *buffobj, size_t count) {
  // fprintf(stderr, "%zu - %zu\n", buffobj->buffer.ndx, count);
  return (buffobj->buffer.raw->data + ((buffobj->buffer.ndx == 0) ? buffobj->buffer.raw->size :
       buffobj->buffer.ndx) - count);
}

static
void
_iterate_taildata_auto(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data) {
  ssize_t	newdata_size;

  do {
    if ((newdata_size = _iterate_readdata(obj)) <= 0)
      return ;
  } while (callback(_rdfl_b_rewind_localptr(&obj->data, newdata_size), newdata_size, data) == BACC_CB_NEEDDATA);
}

static
void
_iterate_chunk(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data) {
  t_rdfl_b_list	*it;
  void		*ptr;
  int		proceed;
  size_t	chunk_data_size;

  ptr = rdfl_b_next_chunk(&obj->data, &chunk_data_size);
  if (!chunk_data_size)
    goto auto_it;
  if (!(proceed = callback(ptr, chunk_data_size, data)))
    return ;
  if (chunk_data_size < obj->data.consumer.l_total)
    if (!(proceed = callback(obj->data.consumer.raw->data,
	    obj->data.consumer.l_total - chunk_data_size, data)))
      return ;
  if ((it = obj->data.consumer.raw->next) != NULL) {
    while (it->next) {
      if (!(proceed = callback(it->data, it->size, data)))
	return ;
      it = it->next;
    }
    if (obj->data.buffer.ndx && (proceed = callback(it->data, obj->data.buffer.ndx, data)) <= BACC_CB_CONTINUE)
      return ;
  }
  if (proceed == BACC_CB_NEEDDATA) {
auto_it:
    if (RDFL_OPT_ISSET(obj->settings, RDFL_AUTOREAD))
      _iterate_taildata_auto(obj, callback, data);
  }
}

#define		PTYPE(p)	((struct s_bacc_getallcontent_data *)p)
struct		s_bacc_getallcontent_data {
  void		*ptr;
  size_t	offset;
};

static
int
_cb__bacc_getallcontent(void *ptr, size_t s, void *data) { // rdfl_bacc_getallcontent
  memcpy(PTYPE(data)->ptr + PTYPE(data)->offset, ptr, s);
  PTYPE(data)->offset += s;
  return (BACC_CB_CONTINUE);
}
#undef		PTYPE

#define		PTYPE(p)	((struct s_bacc_cmp_data *)p)
struct		s_bacc_cmp_data {
  void		*ptr;
  size_t	offset;
  size_t	total;
  int		return_value;
  int		cbret;
};

static
int
_cb__bacc_cmp(void *ptr, size_t s, void *data) { // rdfl_bacc_cmp
  size_t	local = ((s < PTYPE(data)->total) ? s : PTYPE(data)->total);

  if (memcmp(ptr, PTYPE(data)->ptr + PTYPE(data)->offset, local))
    return (BACC_CB_STOP);
  if (!(PTYPE(data)->total -= local)) {
    PTYPE(data)->return_value = 1;
    return (BACC_CB_STOP);
  }
  PTYPE(data)->offset += local;
  return (PTYPE(data)->cbret);
}
#undef		PTYPE

#define		PTYPE(p)	((struct s_bacc_ndx_data *)p)
struct		s_bacc_ndx_data {
  size_t	ndx;
  int		return_value;
};

static
int
_cb__bacc_ndx(void *ptr, size_t s, void *data) { // rdfl_bacc_ndx
  if (PTYPE(data)->ndx >= s) {
    PTYPE(data)->ndx -= s;
    return (BACC_CB_CONTINUE);
  }
  PTYPE(data)->return_value = ((char *)ptr)[PTYPE(data)->ndx];
  return (BACC_CB_STOP);
}
#undef		PTYPE

// Public Access
void *
rdfl_bacc_getallcontent(t_rdfl *obj, size_t *s, e_bacc_options e) {
  struct s_bacc_getallcontent_data	data;

  *s = obj->data.consumer.total + RDFL_OPT_ISSET(e, RBA_NULLTERMINATED);
  if (!(data.ptr = malloc(*s)))
    return (NULL);
  data.offset = 0;
  _iterate_chunk(obj, &_cb__bacc_getallcontent, &data);
  if (RDFL_OPT_ISSET(e, RBA_NULLTERMINATED))
    ((char *)data.ptr)[data.offset] = 0;
  return (data.ptr);
}

static
int
_rdfl_bacc_cycle_cmp(t_rdfl *obj, void *ptr, size_t s, int cbret) {
  struct s_bacc_cmp_data	data;

  if ((data.cbret = cbret) != BACC_CB_NEEDDATA && s > obj->data.consumer.total)
    return (0);
  data.total = s;
  data.ptr = ptr;
  data.offset = 0;
  data.return_value = 0;
  _iterate_chunk(obj, &_cb__bacc_cmp, &data);
  return (data.return_value);
}

int
rdfl_bacc_cmp(t_rdfl *obj, void *ptr, size_t s) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_CONTINUE));
}

int
rdfl_bacc_readptr(t_rdfl *obj, void *ptr, size_t s) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_NEEDDATA));
}

size_t
rdfl_bacc_info_total(t_rdfl *obj) {
  return (obj->data.consumer.total);
}

int	// corresponds to 1 byte
rdfl_bacc_ndx(t_rdfl *obj, size_t n) {
  struct s_bacc_ndx_data	data;
  data.ndx = n;
  data.return_value = 0;
  _iterate_chunk(obj, &_cb__bacc_ndx, &data);
  return (data.return_value);
}


