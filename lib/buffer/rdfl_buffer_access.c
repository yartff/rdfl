#include		<string.h>
#include		<stdlib.h>
#include		"values.h"
#include		"rdfl_buffer_access.h"
#include		"iterate.h"
#include		"rdfl_consumer.h"

// TODO method with extract?
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
    return (BACC_CB_NEEDDATA);
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
  // data.return_value = 0;
  if ((ret = _iterate_chunk(obj, &_cb__bacc_ndx, &data, 0)) < 0)
    return (ret);
  /* TODO: VCSM_REACHED_EOF, VCSM_INCOMPLETE_TOKEN to be handle seperately
  if ((ret = _iterate_chunk(obj, &_cb__bacc_ndx, &data, 0)) < 0
      && ret != VCSM_REACHED_EOF && ret != VCSM_INCOMPLETE_TOKEN)
    return (ret);
  if (ret == VCSM_REACHED_EOF || ret == VCSM_INCOMPLETE_TOKEN)
    return (ret);
    */
  return (data.return_value);
}
