#include		<unistd.h>
#include		<string.h>
#include		"rdfl_local.h"
#include		"rdfl_consumer.h"
#include		"rdfl_buffer_access.h"

#define		PTYPE(p)	((struct s_csm_readUntil *)p)
struct		s_csm_readUntil {
  void		*ptr;
  size_t	s;
  ssize_t	return_value;
  size_t	rebase_skip;
  t_rdfl	*obj;
};

int
_cb__csm_readUntil(void *ptr, size_t s, void *data) { // rdfl_csm_readUntil
  size_t	i = 0;
  (void)ptr;

  // while (str[i]) strstr(str + i)
  PTYPE(data)->obj->data.consumer.skip = PTYPE(data)->rebase_skip
    + PTYPE(data)->return_value;
  while (i < s) {
    if (rdfl_bacc_cmp_needdata(PTYPE(data)->obj, PTYPE(data)->ptr,
	  PTYPE(data)->s, RDFL_P_IGNORE_PREDATASKIP)) {
      PTYPE(data)->obj->data.consumer.skip = PTYPE(data)->rebase_skip;
      PTYPE(data)->return_value += PTYPE(data)->s;
      return (BACC_CB_STOP);
    }
    ++PTYPE(data)->obj->data.consumer.skip;
    ++PTYPE(data)->return_value;
    ++i;
  }
  PTYPE(data)->obj->data.consumer.skip = PTYPE(data)->rebase_skip;
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

// TODO redo this and its callback with contexts
ssize_t
rdfl_csm_readUntil(t_rdfl *obj, void **extract, void *ptr, size_t s, e_bacc_options opt) {
  struct s_csm_readUntil		data;
  int				ret;

  // TODO: do an optimisation for 1 byte ptr
  // readUntilChar();
  data.return_value = 0;
  data.ptr = ptr;
  data.s = s;
  data.obj = obj;
  data.rebase_skip = obj->data.consumer.skip;
  ret = _iterate_chunk(obj, &_cb__csm_readUntil, &data, opt);
  if (ret < 0) {
    if (ret == VCSM_REACHED_EOF)
      return (0);
    return (ret);
  }
  if (_iterate_extract(obj, extract, data.return_value, opt) == ERR_MEMORY)
    return (ERR_MEMORY);
  return (data.return_value);
}
