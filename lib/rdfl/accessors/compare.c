#include		<string.h>
#include		"buffer.h"
#include		"iterate.h"

#define			PTYPE(p)	((struct s_acc_cmp_data *)p)
struct			s_acc_cmp_data {
  void			*ptr;
  size_t		offset;
  size_t		total;
  int			return_value;
};

static
int
_cb__acc_cmp(void *ptr, size_t s, void *data) { // rdfl_acc_cmp
  size_t	local = ((s < PTYPE(data)->total) ? s : PTYPE(data)->total);

  if (memcmp(ptr, PTYPE(data)->ptr + PTYPE(data)->offset, local)) {
    return (BACC_CB_STOP);
  }
  if (!(PTYPE(data)->total -= local)) {
    PTYPE(data)->return_value = 1;
    return (BACC_CB_STOP);
  }
  PTYPE(data)->offset += local;
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

int
rdfl_acc_cmp(t_rdfl *obj, void *ptr, size_t s, e_acc_options opt) {
  struct s_acc_cmp_data		data;
  int				ret;

  if (s > obj->data.consumer.total)
    return (0);
  data.total = s;
  data.ptr = ptr;
  data.offset = 0;
  data.return_value = 0;
  if ((ret = _iterate_chunk(obj, &_cb__acc_cmp, &data, opt)) < 0
      && ret != VCSM_EOF)
    return (ret);
  _iterate_extract(obj, NULL, data.return_value * s, opt);
  return (data.return_value);
}
