#include		<string.h>
#include		"rdfl_local.h"
#include		"rdfl.h"
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
    if ((newdata_size = _iterate_readdata(obj)) <= 0)
      return (rdfl_eofreached(obj) ? VCSM_REACHED_EOF : newdata_size);
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
      //  vvvvvvvv vv	TODO: this check might be useless
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
    if (RDFL_OPT_ISSET(obj->settings, RDFL_AUTOREAD)) {
      return (_iterate_taildata_auto(obj, callback, data));
    }
    return (rdfl_eofreached(obj) ? VCSM_REACHED_EOF : VCSM_INCOMPLETE_TOKEN);
    // TODO make sure this is handled in calling funcs
  }
  return (ERR_NONE);
}
// VCSM_INCOMPLETE_TOKEN means nothing in the stream discards the current routine, but
// we can't reach EOF either

// REACHED_EOF
// INCOMPLETE
// UNMARKED

static
ssize_t
_handle_clear_blanks(t_rdfl *obj, e_bacc_options opt) {
  if (!RDFL_OPT_ISSET(obj->settings, RDFL_AUTOCLEAR_BLANKS))
    return (0);
  return (rdfl_csm_readMatchAny(obj, NULL, " \t\n", opt));
}

static
ssize_t
_handle_comment_loop(t_rdfl *obj, e_bacc_options opt, t_comments *l) {
  ssize_t	ret;
  size_t	len = strlen(l->beg);

  if ((ret = rdfl_bacc_cmp_needdata(obj, l->beg, len, opt)) <= 0)
    return (ret);
  obj->data.consumer.skip += len;
  ret = rdfl_csm_readUntil(obj, NULL, l->end, strlen(l->end), opt | RDFL_P_IGNORE_PREDATASKIP);
  obj->data.consumer.skip -= len;
  if (ret == 0)
    return (ERRBNF_SYNTAX);
  if (ret < 0)
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
	      | RDFL_P_IGNORE_PREDATASKIP)) < 0) {
	obj->data.consumer.skip -= total;
	return (ret);
      }
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

// TODO: also expose method to handlepredata except last line (for '^')
static
ssize_t
_handle_predata(t_rdfl *obj, e_bacc_options opt) {
  return (_handle_clearable_data(obj, opt));
}

// TODO: callback[0] and callback[n] for all _cb funcs
int
_iterate_chunk(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data, e_bacc_options opt) {
  int		ret;
  ssize_t	save; // skip is incremented by save when predata handled

  if (RDFL_OPT_ISSET(opt, RDFL_P_IGNORE_PREDATASKIP))
    return (_iterate_chunk_routine(obj, callback, data));
  if ((save = _handle_predata(obj, opt)) < 0)
    return (save);
  // rdfl_force_consume_size(obj, save);
#ifdef		DEVEL
  if (RDFL_OPT_ISSET(obj->settings, RDFL_CONTEXT)) {
#endif
    if (RDFL_OPT_ISSET(opt, RDFL_P_SETCONTEXT)) {
      if ((ret = rdfl_pushcontext(obj)) < 0)
	return (ret);
    }
#ifdef		DEVEL
  } else if (RDFL_OPT_ISSET(opt, RDFL_P_SETCONTEXT)) {
    return (ERRDEV_BADFLAGS);
  }
#endif
  if (((ret = _iterate_chunk_routine(obj, callback, data)) == VCSM_INCOMPLETE_TOKEN)
      && rdfl_eofreached(obj))
    return (VCSM_REACHED_EOF);
  return (ret);
}

// HAS TO be called everytime after _iterate_chunk
// you can do things in between
// if you send NULL as extract, it can't fail
int
_iterate_extract(t_rdfl *obj, void **extract, ssize_t s, e_bacc_options opt) {
  if (s <= 0) {
    if (extract) *extract = NULL;
    return (ERR_NONE);
  }
  if ((extract != NULL) && (!(*extract = rdfl_bacc_getcontent(obj, NULL, s, opt | RDFL_P_IGNORE_PREDATASKIP))))
    return (ERR_MEMORY);
  if (RDFL_OPT_ISSET(opt, RDFL_P_CONSUME))
    rdfl_b_consume_size(&obj->data, s);
  return (ERR_NONE);
}
