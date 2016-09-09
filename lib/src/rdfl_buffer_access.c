#include		<string.h>
#include		<stdlib.h>
#include		"rdfl_local.h"
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"
#include		"rdfl_context.h"

static
ssize_t
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
ssize_t
_iterate_taildata_auto(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data) {
  ssize_t	newdata_size;

  do {
    if ((newdata_size = _iterate_readdata(obj)) <= 0) {
      return (rdfl_eofreached(obj) ? VCSM_REACHED_EOF : newdata_size);
    }
  } while (callback(_rdfl_b_rewind_localptr(&obj->data, newdata_size), newdata_size, data) == BACC_CB_NEEDDATA);
  return (ERR_NONE);
}

// skip will never be >total
inline
static
int
_iterate_autoskip(void *chunk, size_t s,
    int (*callback)(void *, size_t, void *), void *data,
    size_t *skipped, size_t skip) {
  size_t	to_skip = skip - *skipped;
  if (!to_skip) return (callback(chunk, s, data));
  if (s > to_skip) {
    *skipped += to_skip;
    return (callback(chunk + to_skip, s - to_skip, data));
  }
  if ((*skipped += s) == skip)
    return (BACC_CB_NEEDDATA);
  return (BACC_CB_NOACTION);
}

// Can't think of any easy better way to do
static
int
_iterate_chunk_routine(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data) {
  t_rdfl_b_list	*it;
  void		*ptr;
  int		proceed;
  size_t	chunk_data_size, skipped = 0, total_skip;

  total_skip = obj->data.consumer.skip;
  ptr = rdfl_b_next_chunk(&obj->data, &chunk_data_size);
  if (!chunk_data_size) goto auto_it;
  if (!(proceed = _iterate_autoskip(ptr, chunk_data_size, callback, data, &skipped, total_skip)))
    return (ERR_NONE);
  if (chunk_data_size < obj->data.consumer.l_total)
    if (!(proceed = _iterate_autoskip(obj->data.consumer.raw->data,
	    obj->data.consumer.l_total - chunk_data_size, callback, data, &skipped, total_skip)))
      return (ERR_NONE);
  if ((it = obj->data.consumer.raw->next) != NULL) {
    while (it->next) {
      // vvvvvvvvvvv TODO: this check might be useless
      if (it->size && !(proceed = _iterate_autoskip(it->data, it->size, callback,
	      data, &skipped, total_skip)))
	return (ERR_NONE);
      it = it->next;
    }
    if (obj->data.buffer.ndx &&
	(proceed = _iterate_autoskip(it->data, obj->data.buffer.ndx,
				     callback, data, &skipped,
				     total_skip)) <= BACC_CB_CONTINUE)
      return (ERR_NONE);
  }
  if (proceed == BACC_CB_NEEDDATA) {
auto_it:
    if (RDFL_OPT_ISSET(obj->settings, RDFL_AUTOREAD))
      return (_iterate_taildata_auto(obj, callback, data));
    return (rdfl_eofreached(obj) ? VCSM_INCOMPLETE_TOKEN : VCSM_UNMARKED_TOKEN);
  }
  return (ERR_NONE);
}

static
ssize_t
_handle_clear_blanks(t_rdfl *obj, e_bacc_options opt) {
  if (!RDFL_OPT_ISSET(obj->settings, RDFL_AUTOCLEAR_BLANKS))
    return (0);
  return (rdfl_ct_readAllContained(obj, NULL, " \t\n", opt));
}

static
ssize_t
_handle_comment_loop(t_rdfl *obj, e_bacc_options opt, t_comments *l) {
  ssize_t	ret;
  size_t	len = strlen(l->beg);

  if ((ret = rdfl_bacc_readptr(obj, l->beg, len, opt)) <= 0)
    return (ret);
  obj->data.consumer.skip += len;
  ret = rdfl_ct_readUntil(obj, l->end, strlen(l->end), opt | RDFL_P_IGNORE_PREDATA);
  obj->data.consumer.skip -= len;
  if (ret <= 0)
    return (ret);
  return (len + ret);
}

static
ssize_t
_handle_comment(t_rdfl *obj, e_bacc_options opt) {
  t_comments	*l = obj->v.cmts;
  ssize_t	ret;

  while (l) {
    if ((ret = _handle_comment_loop(obj, opt, l)) != 0)
      return (ret);
    l = l->next;
  }
  return (0);
}

static
ssize_t
_handle_clearable_data(t_rdfl *obj, e_bacc_options opt) {
  ssize_t		total = 0, ret;
  int			loop_ret = 1;
  size_t		i;
  ssize_t		(*ft[])(t_rdfl *, e_bacc_options) = {
    &_handle_clear_blanks,
    &_handle_comment
  };

  while (loop_ret) {
    loop_ret = 0;
    i = 0;
    while (i < (sizeof(ft) / sizeof(*ft))) {
      if ((ret = ft[i](obj, RDFL_OPT_CANCEL(opt, RDFL_P_CONSUME)
	      | RDFL_P_IGNORE_PREDATA)) < 0)
	return (ret);
      if (ret > 0) {
	total += ret;
	loop_ret = 1;
	obj->data.consumer.skip += ret;
	break ;
      }
      ++i;
    }
  }
  return (total);
}

static
ssize_t
_handle_predata(t_rdfl *obj, e_bacc_options opt) {
  return (_handle_clearable_data(obj, opt));
}

int
_iterate_chunk(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data, e_bacc_options opt) {
  int		ret;
  ssize_t	save;

  if (RDFL_OPT_ISSET(opt, RDFL_P_IGNORE_PREDATA))
    return (_iterate_chunk_routine(obj, callback, data));
  if ((save = _handle_predata(obj, opt)) < 0) {
    return (save);
  }
  // obj->data.consumer.skip += save;
  if (RDFL_OPT_ISSET(obj->settings, RDFL_CONTEXT)) {
    if (RDFL_OPT_ISSET(opt, RDFL_P_SETCONTEXT)) {
      if ((ret = rdfl_shiftcontext(obj, 0)) < 0)
	return (ret);
      return (_iterate_chunk_routine(obj, callback, data));
    }
    ret = _iterate_chunk_routine(obj, callback, data);
  }
  else {
    ret = _iterate_chunk_routine(obj, callback, data);
  }
  return (ret);
}

// HAS TO be called everytime after _iterate_chunk
// you can do things in between
int
_iterate_extract(t_rdfl *obj, void **extract, ssize_t s, e_bacc_options opt) {
  if (s <= 0) {
    if (extract) *extract = NULL;
    return (ERR_NONE);
  }
  if (extract)
    if (!(*extract = rdfl_bacc_getcontent(obj, NULL, s, opt | RDFL_P_IGNORE_PREDATA)))
      return (ERR_MEMORY);
  if (RDFL_OPT_ISSET(opt, RDFL_P_CONSUME))
    rdfl_b_consume_size(&obj->data, s);
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

  if (memcmp(ptr, PTYPE(data)->ptr + PTYPE(data)->offset, local)) {
    return (BACC_CB_STOP);
  }
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
  _iterate_chunk(obj, &_cb__bacc_getallcontent, &data, e);
  if (opt) ((char *)data.ptr)[data.offset] = 0;
  if (s_p) *s_p = data.c + opt;
  return (data.ptr);
}

static
int
_rdfl_bacc_cycle_cmp(t_rdfl *obj, void *ptr, size_t s, int cbret, e_bacc_options opt) {
  struct s_bacc_cmp_data	data;

  if ((data.cbret = cbret) != BACC_CB_NEEDDATA && s > obj->data.consumer.total)
    return (0);
  data.total = s;
  data.ptr = ptr;
  data.offset = 0;
  data.return_value = 0;
  if (_iterate_chunk(obj, &_cb__bacc_cmp, &data, opt) == VCSM_REACHED_EOF) {
    return (data.return_value ? VCSM_INCOMPLETE_TOKEN : ERR_NONE);
  }
  return (data.return_value);
}

int
rdfl_bacc_cmp(t_rdfl *obj, void *ptr, size_t s) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_CONTINUE, 0));
}

int
rdfl_bacc_readptr(t_rdfl *obj, void *ptr, size_t s, e_bacc_options opt) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_NEEDDATA, opt));
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

