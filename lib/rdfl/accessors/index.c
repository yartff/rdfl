#include		"buffer.h"
#include		"iterate.h"

#define		PTYPE(p)	((struct s_acc_ndx_data *)p)
struct		s_acc_ndx_data {
  size_t	ndx;
  int		return_value;
};

static
int
_cb__acc_ndx(void *ptr, size_t s, void *data) { // rdfl_acc_ndx
  if (PTYPE(data)->ndx >= s) {
    PTYPE(data)->ndx -= s;
    return (BACC_CB_NEEDDATA);
  }
  PTYPE(data)->return_value = ((char *)ptr)[PTYPE(data)->ndx];
  return (BACC_CB_STOP);
}
#undef		PTYPE

int	// corresponds to 1 byte
rdfl_acc_ndx(t_rdfl *obj, size_t n) {
  struct s_acc_ndx_data	data;
  int				ret;

  data.ndx = n;
  // data.return_value = 0;
  if ((ret = _iterate_chunk(obj, &_cb__acc_ndx, &data, 0)) < 0)
    return (ret);
  /* TODO: VCSM_EOF, VCSM_INCOMPLETE_TOKEN to be handle seperately
  if ((ret = _iterate_chunk(obj, &_cb__acc_ndx, &data, 0)) < 0
      && ret != VCSM_EOF && ret != VCSM_INCOMPLETE_TOKEN)
    return (ret);
  if (ret == VCSM_EOF || ret == VCSM_INCOMPLETE_TOKEN)
    return (ret);
    */
  return (data.return_value);
}
