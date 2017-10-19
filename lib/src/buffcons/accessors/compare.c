#include		<string.h>
#include		<stdlib.h>
#include		"rdfl_local.h"
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"

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
  if ((ret = _iterate_chunk(obj, &_cb__bacc_cmp, &data, opt)) < 0
      && ret != VCSM_REACHED_EOF)
    return (ret);
  _iterate_extract(obj, NULL, data.return_value * s, opt);
  return (data.return_value);
}

int
rdfl_bacc_cmp(t_rdfl *obj, void *ptr, size_t s) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_CONTINUE, 0));
}

// TODO: should be named cmp? this is the default behaviour
int
rdfl_bacc_cmp_needdata(t_rdfl *obj, void *ptr, size_t s, e_bacc_options opt) {
  return (_rdfl_bacc_cycle_cmp(obj, ptr, s, BACC_CB_NEEDDATA, opt));
}

