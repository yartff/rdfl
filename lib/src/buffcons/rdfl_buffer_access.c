#include		<string.h>
#include		<stdlib.h>
#include		"rdfl_local.h"
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"
// Methods
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

static
int
_rdfl_bacc_cycle_cmp(t_rdfl *obj, void *ptr, size_t s, int cbret, e_bacc_options opt) {
  struct s_bacc_cmp_data	data;
  int				ret;

  if ((data.cbret = cbret) != BACC_CB_NEEDDATA && s > obj->data.consumer.total)
    return (0);
  data.total = s;
  data.ptr = ptr;
  data.offset = 0;
  data.return_value = 0;
  // rdfl_printbufferstate(obj);
  if ((ret = _iterate_chunk(obj, &_cb__bacc_cmp, &data, opt)) <= 0) {
    if (VCSM_REACHED_EOF) {
      _iterate_extract(obj, NULL, data.return_value * s, opt);
      return (data.return_value);
    }
    return (ret);
  }
  _iterate_extract(obj, NULL, data.return_value * s, opt);
  return (data.return_value);
}

int
rdfl_bacc_cmp(t_rdfl *obj, void *ptr, size_t s) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_CONTINUE, 0));
}

// TODO method with extract?
int
rdfl_bacc_readptr(t_rdfl *obj, void *ptr, size_t s, e_bacc_options opt) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_NEEDDATA, opt));
}

size_t
rdfl_bacc_info_total(t_rdfl *obj) {
  return (obj->data.consumer.total);
}

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

int	// corresponds to 1 byte
rdfl_bacc_ndx(t_rdfl *obj, size_t n) {
  struct s_bacc_ndx_data	data;
  int				ret;

  data.ndx = n;
  data.return_value = 0;
  // TODO ret
  ret = _iterate_chunk(obj, &_cb__bacc_ndx, &data, 0);
  (void)ret;
  return (data.return_value);
}
