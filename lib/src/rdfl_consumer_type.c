#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"

#define		PTYPE(p)	((struct s_ct_readString *)p)
struct		s_ct_readString {
  ssize_t	total;
  // does not include any escaped sequence or enclosing quotes
  size_t	resultdata;
  ssize_t	return_value;
  char		quote;
};

static
int
_cb__ct_readString_legacy(void *ptr, size_t s, void *data) { // rdfl_ct_readString
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
      // fprintf(stderr, "[CT STRING]{%zd (total: %zu)(return: %zu)}\n", i, PTYPE(data)->total,
      //   PTYPE(data)->return_value);
      return (BACC_CB_STOP);
    }
    ++i;
  }
  i -= diff;
  PTYPE(data)->total += i;
  PTYPE(data)->resultdata += i;
  return (BACC_CB_NEEDDATA);
}

inline static void _readString_init(struct s_ct_readString *d)
{ d->total = 0; d->resultdata = 0; d->return_value = 0; }

int
_cb__ct_readString_cstyle(void *ptr, size_t s, void *data) {
  (void)ptr, (void)s;
  _readString_init(PTYPE(data));
  // TODO
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

ssize_t
rdfl_ct_readString(t_rdfl *obj, void **extract, e_bacc_options bopt) {
  struct s_ct_readString	data;

  data.quote = (RDFL_OPT_ISSET(bopt, RDFL_PSTR_SIMPLE_QUOTE_STR) ? '\'' : '"');
  _readString_init(&data);
  if (_iterate_chunk(obj, &_cb__ct_readString_legacy, &data, 0) <= 0)
    return (VCSM_INCOMPLETE_TOKEN);
  if (!data.total) return (0);
  if (extract) *extract = rdfl_bacc_getcontent(obj, NULL, data.return_value, bopt);
  else if (RDFL_OPT_ISSET(bopt, RDFL_P_CONSUME))
    rdfl_b_consume_size(&obj->data, data.return_value);
  return (data.return_value);
}

ssize_t
rdfl_ct_readAllContained(t_rdfl *obj, void **extract, const char *content, e_bacc_options bopt) {
  (void)obj, (void)extract, (void)content, (void)bopt;
  return (0);
}
