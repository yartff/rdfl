#include		<string.h>
#include		<stdlib.h>
#include		"buffer.h"
#include		"iterate.h"

#define		PTYPE(p)	((struct s_bacc_getallcontent_data *)p)
struct		s_bacc_getallcontent_data {
  void		*ptr;
  size_t	offset;
  size_t	c;
};

static
int
_cb__bacc_getallcontent(void *ptr, size_t s, void *data) { // rdfl_bacc_getcontent
  if (s > (PTYPE(data)->c - PTYPE(data)->offset))
    s = PTYPE(data)->c - PTYPE(data)->offset;
  memcpy(PTYPE(data)->ptr + PTYPE(data)->offset, ptr, s);
  if ((PTYPE(data)->offset += s) == PTYPE(data)->c)
    return (BACC_CB_STOP);
  return (BACC_CB_CONTINUE);
}
#undef		PTYPE

// TODO: redo within single if with total instead of offset
void *
rdfl_bacc_getcontent(t_rdfl *obj, ssize_t *s_p, size_t c, e_bacc_options e) {
  struct s_bacc_getallcontent_data	data;
  int					extra_char, ret;

  if (s_p) *s_p = ERR_NONE;
  extra_char = (RDFL_OPT_ISSET(e, RDFL_P_NULLTERMINATED) != 0);
  if (!obj->data.consumer.total)
    return (NULL);
  data.c = ((c != 0 && c < obj->data.consumer.total) ? c : obj->data.consumer.total);
  if (!(data.ptr = malloc(data.c + extra_char))) {
    if (s_p) *s_p = ERR_MEMORY_ALLOC;
    return (NULL);
  }
  data.offset = 0;
  if ((ret = _iterate_chunk(obj, &_cb__bacc_getallcontent, &data, e)) < 0) {
    free(data.ptr);
    if (s_p) *s_p = ret;
    return (NULL);
  }
  if (extra_char) ((char *)data.ptr)[data.offset] = 0;
  if (s_p) *s_p = data.c + extra_char;
  return (data.ptr);
}
