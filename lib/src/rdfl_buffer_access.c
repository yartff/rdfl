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
int
_iterate_taildata_auto(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data) {
  ssize_t	newdata_size;

  do {
    if ((newdata_size = _iterate_readdata(obj)) <= 0) {
      return (rdfl_eofreached(obj) ? VCSM_REACHED_EOF : ERR_NONE);
    }
  } while (callback(_rdfl_b_rewind_localptr(&obj->data, newdata_size), newdata_size, data) == BACC_CB_NEEDDATA);
  return (ERR_NONE);
}

// TODO: return int, if neg, error
//  return if EOF or socket or wahtev
// TODO: VCSM_INCOMPLETE_TOKEN & VCSM_UNMARKED_TOKEN from callbacks
// both TODOes will cover all errors
int
_iterate_chunk(t_rdfl *obj, int (*callback)(void *, size_t, void *),
    void *data, size_t skip) {
  t_rdfl_b_list	*it;
  void		*ptr;
  int		proceed;
  size_t	chunk_data_size;
  (void)skip; // TODO in t_rdfl_csm
  // TODO check for return in routines that sends NEEDDATA

  // returns either EOF reached or 0
  ptr = rdfl_b_next_chunk(&obj->data, &chunk_data_size);
  if (!chunk_data_size)
    goto auto_it;
  if (!(proceed = callback(ptr, chunk_data_size, data))) {
    return (ERR_NONE);
  }
  if (chunk_data_size < obj->data.consumer.l_total)
    if (!(proceed = callback(obj->data.consumer.raw->data,
	    obj->data.consumer.l_total - chunk_data_size, data)))
      return (ERR_NONE);
  if ((it = obj->data.consumer.raw->next) != NULL) {
    while (it->next) {
      // vvvvvvvvvvv TODO: this check might be useless
      proceed = callback(it->data, it->size, data);
      if (it->size && !(proceed)) {
	return (ERR_NONE);
      }
      it = it->next;
    }
    if (obj->data.buffer.ndx &&
	(proceed = callback(it->data, obj->data.buffer.ndx, data)) <= BACC_CB_CONTINUE)
      return (ERR_NONE);
  }
  if (proceed == BACC_CB_NEEDDATA) {
auto_it:
    if (RDFL_OPT_ISSET(obj->settings, RDFL_AUTOREAD))
      return (_iterate_taildata_auto(obj, callback, data));
    return (VCSM_UNMARKED_TOKEN);
  }
  return (ERR_NONE);
}

#define		PTYPE(p)	((struct s_bacc_getallcontent_data *)p)
struct		s_bacc_getallcontent_data {
  void		*ptr;
  size_t	offset;
  size_t	c;
};

static
int
_cb__bacc_getallcontent(void *ptr, size_t s, void *data) { // rdfl_bacc_getcontent
  if (s > (PTYPE(data)->c - PTYPE(data)->offset))
    s = PTYPE(data)->c - PTYPE(data)->offset;
  memcpy(PTYPE(data)->ptr + PTYPE(data)->offset, ptr, s);
  if ((PTYPE(data)->offset += s) == PTYPE(data)->c)
    return (BACC_CB_STOP);
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
_cb__bacc_cmp(void *ptr, size_t s, void *data) { // _rdfl_bacc_cycle_cmp
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
rdfl_bacc_getcontent(t_rdfl *obj, size_t *s_p, size_t c, e_bacc_options e) {
  struct s_bacc_getallcontent_data	data;
  int					opt;

  opt = (RDFL_OPT_ISSET(e, RDFL_P_NULLTERMINATED) != 0);
  if (!obj->data.consumer.total) return (NULL);
  data.c = ((c != 0 && c < obj->data.consumer.total) ? c : obj->data.consumer.total) ;
  if (!(data.ptr = malloc(data.c + opt)))
    return (NULL);
  data.offset = 0;
  _iterate_chunk(obj, &_cb__bacc_getallcontent, &data, 0);
  if (opt) ((char *)data.ptr)[data.offset] = 0;
  if (RDFL_OPT_ISSET(e, RDFL_P_CONSUME))
    rdfl_b_consume_size(&obj->data, data.c);
  if (s_p) *s_p = data.c + opt;
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
  if (_iterate_chunk(obj, &_cb__bacc_cmp, &data, 0) == VCSM_REACHED_EOF) {
    return (data.return_value ? VCSM_INCOMPLETE_TOKEN : ERR_NONE);
  }
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
  _iterate_chunk(obj, &_cb__bacc_ndx, &data, 0);
  return (data.return_value);
}
