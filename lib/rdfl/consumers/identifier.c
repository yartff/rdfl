#include		"rdfl_consumer.h"
#include		"buffer.h"
#include		"iterate.h"

#define		PTYPE(p)	((struct s_csm_readIdentifier *)p)
struct		s_csm_readIdentifier {
  ssize_t	return_value;
};

#define		IS_CHAR_LOWER(c)	((c >= 'a' && c <= 'z'))
#define		IS_CHAR_UPPER(c)	((c >= 'A' && c <= 'Z'))
#define		IS_DIGIT(c)		((c >= '0' && c <= '9'))
#define		IS_CHAR(c)		((IS_CHAR_LOWER(c) || IS_CHAR_UPPER(c)))
#define		IDENTIFIER_ST_CHAR(c)	((IS_CHAR(c) || (c == '_')))
#define		IDENTIFIER_N_CHAR(c)	((IS_CHAR(c)) || IS_DIGIT(c) || (c == '_'))

int
_cb__csm_readIdentifier(void *ptr, size_t s, void *data) { // rdfl_csm_readIdentifier
  size_t	i = 0;

  if (!(PTYPE(data)->return_value)) {
    if (IDENTIFIER_ST_CHAR(((char *)ptr)[0])) {
      PTYPE(data)->return_value = 1;
      i = 1;
    }
    else
      return (BACC_CB_STOP);
  }
  while (i < s) {
    if (!IDENTIFIER_N_CHAR(((char *)ptr)[i])) {
      return (BACC_CB_STOP);
    }
    ++PTYPE(data)->return_value;
    ++i;
  }
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

// TODO several types of identifier
ssize_t
rdfl_csm_readIdentifier(t_rdfl *obj, void **extract, e_acc_options opt) {
  struct s_csm_readIdentifier	data;
  int				ret;

  data.return_value = 0;
  if ((ret = _iterate_chunk(obj, &_cb__csm_readIdentifier, &data, opt)) < 0
      && ret != VCSM_EOF)
    return (ret);
  if (_iterate_extract(obj, extract, data.return_value, opt) == ERR_MEMORY_ALLOC)
    return (ERR_MEMORY_ALLOC);
  return (data.return_value);
}
