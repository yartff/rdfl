#include		<unistd.h>
#include		<string.h>
#include		"rdfl_local.h"
#include		"rdfl_consumer.h"
#include		"rdfl_buffer_access.h"

#define		PTYPE(p)	((struct s_csm_readString *)p)
struct		s_csm_readString {
  ssize_t	total;
  ssize_t	return_value;
  char		quote;
  // does not include any escaped sequence or enclosing quotes
  size_t	resultdata;
};

static
int
_cb__csm_readString_legacy(void *ptr, size_t s, void *data) { // rdfl_csm_readString
  size_t		i = 0;
  int			diff = 0;

  if (!PTYPE(data)->total) {
    if (((char *)ptr)[0] != PTYPE(data)->quote) {
      return (BACC_CB_STOP);
    }
    PTYPE(data)->total = 1;
    i = 1;
    diff = 1;
  }
  while (i < s) {
    if (((char *)ptr)[i] == PTYPE(data)->quote) {
      ++i;
      PTYPE(data)->return_value = PTYPE(data)->total + i - diff;
      PTYPE(data)->resultdata += (i - diff - 1);
      return (BACC_CB_STOP);
    }
    ++i;
  }
  i -= diff;
  PTYPE(data)->total += i;
  PTYPE(data)->resultdata += i;
  return (BACC_CB_NEEDDATA);
}

int
_cb__csm_readString_cstyle(void *ptr, size_t s, void *data) {
  (void)ptr, (void)s, (void)data;
  // TODO
  // Use RDFL_P_IGNORE_PREDATASKIP
  // with readString(0) || readString(RDFL_PSTR_SIMPLE_QUOTE_STR)
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

inline static void _readString_init(struct s_csm_readString *d)
{ d->total = 0; d->resultdata = 0; d->return_value = 0; }

ssize_t
rdfl_csm_readString(t_rdfl *obj, void **extract, e_bacc_options bopt) {
  struct s_csm_readString	data;
  int				ret;

  data.quote = (RDFL_OPT_ISSET(bopt, RDFL_PSTR_SIMPLE_QUOTE_STR) ? '\'' : '"');
  _readString_init(&data);
  if ((ret = _iterate_chunk(obj, &_cb__csm_readString_legacy, &data, bopt)) < 0) {
    if (ret == VCSM_REACHED_EOF)
      return (data.return_value);
    return (VCSM_INCOMPLETE_TOKEN);
  }
  if (_iterate_extract(obj, extract, data.return_value, bopt) == ERR_MEMORY)
    return (ERR_MEMORY);
  return (data.return_value);
}
