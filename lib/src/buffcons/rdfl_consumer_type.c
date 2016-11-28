#include		<unistd.h>
#include		<string.h>
#include		"rdfl_local.h"
#include		"rdfl_consumer.h"
#include		"rdfl_buffer_access.h"

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
_cb__ct_readString_cstyle(void *ptr, size_t s, void *data) {
  (void)ptr, (void)s, (void)data;
  // TODO
  // Use RDFL_P_IGNORE_PREDATA
  // with readString(0) || readString(RDFL_PSTR_SIMPLE_QUOTE_STR)
  return (BACC_CB_NEEDDATA);
}
#undef		PTYPE

inline static void _readString_init(struct s_ct_readString *d)
{ d->total = 0; d->resultdata = 0; d->return_value = 0; }

ssize_t
rdfl_ct_readString(t_rdfl *obj, void **extract, e_bacc_options bopt) {
  struct s_ct_readString	data;
  int				ret;

  data.quote = (RDFL_OPT_ISSET(bopt, RDFL_PSTR_SIMPLE_QUOTE_STR) ? '\'' : '"');
  _readString_init(&data);
  if ((ret = _iterate_chunk(obj, &_cb__ct_readString_legacy, &data, bopt)) < 0) {
    if (ret == VCSM_REACHED_EOF)
      return (data.return_value);
    return (VCSM_INCOMPLETE_TOKEN);
  }
  if (_iterate_extract(obj, extract, data.return_value, bopt) == ERR_MEMORY)
    return (ERR_MEMORY);
  return (data.return_value);
}

#define		PTYPE(p)	((struct s_ct_readAllContained *)p)
struct		s_ct_readAllContained {
  void		*content;
  size_t	ctsize;
  ssize_t	return_value;
};

static
int
_cb__ct_readAllContained(void *ptr, size_t s, void *data) { // rdfl_ct_readAllContained
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
rdfl_ct_readAllContained(t_rdfl *obj, void **extract, const char *content, e_bacc_options bopt) {
  struct s_ct_readAllContained	data;
  int		ret;

  data.return_value = 0;
  data.content = (void *)content;
  data.ctsize = strlen(content);
  if ((ret = _iterate_chunk(obj, &_cb__ct_readAllContained, &data, bopt)) < 0
      && ret != VCSM_REACHED_EOF)
    return (ret);
  if (_iterate_extract(obj, extract, data.return_value, bopt) == ERR_MEMORY)
    return (ERR_MEMORY);
  return (data.return_value);
}

#define		PTYPE(p)	((struct s_ct_readUntil *)p)
struct		s_ct_readUntil {
  void		*ptr;
  size_t	s;
  ssize_t	return_value;
  size_t	rebase_skip;
  t_rdfl	*obj;
};

int
_cb__ct_readUntil(void *ptr, size_t s, void *data) { // rdfl_ct_readUntil
  size_t	i = 0;
  (void)ptr;

  // while (str[i]) strstr(str + i)
  PTYPE(data)->obj->data.consumer.skip = PTYPE(data)->rebase_skip
    + PTYPE(data)->return_value;
  while (i < s) {
    if (rdfl_bacc_readptr(PTYPE(data)->obj, PTYPE(data)->ptr,
	  PTYPE(data)->s, RDFL_P_IGNORE_PREDATA)) {
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

// TODO redo this and its callback
ssize_t
rdfl_ct_readUntil(t_rdfl *obj, void **extract, void *ptr, size_t s, e_bacc_options opt) {
  struct s_ct_readUntil		data;
  int				ret;

  // TODO: do an optimisation for 1 byte ptr
  // readUntilChar();
  data.return_value = 0;
  data.ptr = ptr;
  data.s = s;
  data.obj = obj;
  data.rebase_skip = obj->data.consumer.skip;
  ret = _iterate_chunk(obj, &_cb__ct_readUntil, &data, opt);
  if (ret < 0) {
    if (ret == VCSM_REACHED_EOF)
      return (0);
    return (ret);
  }
  if (_iterate_extract(obj, extract, data.return_value, opt) == ERR_MEMORY)
    return (ERR_MEMORY);
  return (data.return_value);
}

#define		PTYPE(p)	((struct s_ct_readIdentifier *)p)
struct		s_ct_readIdentifier {
  ssize_t	return_value;
};

#define		IS_CHAR_LOWER(c)	((c >= 'a' && c <= 'z'))
#define		IS_CHAR_UPPER(c)	((c >= 'A' && c <= 'Z'))
#define		IS_DIGIT(c)		((c >= '0' && c <= '9'))
#define		IS_CHAR(c)		((IS_CHAR_LOWER(c) || IS_CHAR_UPPER(c)))
#define		IDENTIFIER_ST_CHAR(c)	((IS_CHAR(c) || (c == '_')))
#define		IDENTIFIER_N_CHAR(c)	((IS_CHAR(c)) || IS_DIGIT(c) || (c == '_'))

int
_cb__ct_readIdentifier(void *ptr, size_t s, void *data) { // rdfl_ct_readIdentifier
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

// TODO
ssize_t
rdfl_ct_readIdentifier(t_rdfl *obj, void **extract, e_bacc_options opt) {
  struct s_ct_readIdentifier	data;
  int				ret;

  data.return_value = 0;
  if ((ret = _iterate_chunk(obj, &_cb__ct_readIdentifier, &data, opt)) < 0
      && ret != VCSM_REACHED_EOF)
    return (ret);
  if (_iterate_extract(obj, extract, data.return_value, opt) == ERR_MEMORY)
    return (ERR_MEMORY);
  return (data.return_value);
}
