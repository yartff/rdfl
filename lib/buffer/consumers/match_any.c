#include		<unistd.h>
#include		<string.h>
#include		"iterate.h"
#include		"values.h"
#include		"rdfl_consumer.h"
#include		"rdfl_buffer_access.h"


#define		PTYPE(p)	((struct s_csm_readMatchAny *)p)
struct		s_csm_readMatchAny {
  void		*content;
  size_t	ctsize;
  ssize_t	return_value;
};

static
int
_cb__csm_readMatchAny(void *ptr, size_t s, void *data) { // rdfl_csm_readMatchAny
  size_t	to_s = 0, n;

  while (to_s < s) {
    n = 0;
    while (n < PTYPE(data)->ctsize) {
      if (((char *)ptr)[to_s] == ((char *)PTYPE(data)->content)[n]) {
	++PTYPE(data)->return_value;
	break ;
      }
      ++n;
    }
    if (n == PTYPE(data)->ctsize) {
      return (BACC_CB_STOP);
    }
    ++to_s;
  }
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

ssize_t
rdfl_csm_readMatchAny(t_rdfl *obj, void **extract, const char *content, e_bacc_options bopt) {
  struct s_csm_readMatchAny	data;
  int		ret;

  data.return_value = 0;
  data.content = (void *)content;
  data.ctsize = strlen(content);
  if ((ret = _iterate_chunk(obj, &_cb__csm_readMatchAny, &data, bopt)) < 0
      && ret != VCSM_REACHED_EOF)
    return (ret);
  if (_iterate_extract(obj, extract, data.return_value, bopt) == ERR_MEMORY_ALLOC)
    return (ERR_MEMORY_ALLOC);
  return (data.return_value);
}

