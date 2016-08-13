#include		<string.h>
#include		<stdlib.h>
#include		"rdfl_buffer_access.h"

static
void
_iterate_chunk(t_rdfl *obj, int (*callback)(void *, size_t, void *), void *data) {
  size_t	chunk_data_size;
  t_rdfl_b_list	*it;
  void		*ptr = rdfl_b_next_chunk(&obj->data, &chunk_data_size);

  if (!ptr || callback(ptr, chunk_data_size, data))
    return ;
  if (chunk_data_size < obj->data.consumer.l_total)
    if (callback(obj->data.consumer.raw->data,
	  obj->data.consumer.l_total - chunk_data_size, data))
      return ;
  if (!(it = obj->data.consumer.raw->next))
    return ;
  while (it != obj->data.buffer.raw) {
    if (callback(it->data, it->size, data))
      return ;
    it = it->next;
  }
  callback(it->data, obj->data.buffer.ndx, data);
}

#define		PTYPE(p)	((struct s_bacc_getallcontent_data *)p)
struct		s_bacc_getallcontent_data {
  void		*ptr;
  size_t	offset;
};

static
int
_cb__bacc_getallcontent(void *ptr, size_t s, void *data) { // rdfl_bacc_getallcontent
  memcpy(PTYPE(data)->ptr + PTYPE(data)->offset, ptr, s);
  PTYPE(data)->offset += s;
  return (0);
}
#undef		PTYPE

#define		PTYPE(p)	((struct s_bacc_cmp_data *)p)
struct		s_bacc_cmp_data {
  void		*ptr;
  size_t	offset;
  size_t	total;
  int		return_value;
};

static
int
_cb__bacc_cmp(void *ptr, size_t s, void *data) { // rdfl_bacc_cmp
  size_t	local = s < PTYPE(data)->total ? s : PTYPE(data)->total;

  if (memcmp(ptr, PTYPE(data)->ptr + PTYPE(data)->offset, local))
    return (1);
  if (!(PTYPE(data)->total -= local)) {
    PTYPE(data)->return_value = 1;
    return (1);
  }
  PTYPE(data)->offset += local;
  return (0);
}
#undef		PTYPE

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
    return (0);
  }
  printf("%zu\n", PTYPE(data)->ndx);
  PTYPE(data)->return_value = ((char *)ptr)[PTYPE(data)->ndx];
  return (1);
}
#undef		PTYPE

// Public Access
void *
rdfl_bacc_getallcontent(t_rdfl *obj, size_t *s, e_bacc_options e) {
  struct s_bacc_getallcontent_data	data;

  *s = obj->data.consumer.total + ((e & RBA_NULLTERMINATED) != 0);
  if (!(data.ptr = malloc(*s)))
    return (NULL);
  data.offset = 0;
  _iterate_chunk(obj, &_cb__bacc_getallcontent, &data);
  if (e & RBA_NULLTERMINATED)
    ((char *)data.ptr)[data.offset] = 0;
  return (data.ptr);
}

int
rdfl_bacc_cmp(t_rdfl *obj, void *ptr, size_t s) {
  struct s_bacc_cmp_data	data;

  data.total = obj->data.consumer.total;
  if (s > data.total) return (0);
  if (s < data.total) data.total = s;
  data.ptr = ptr;
  data.offset = 0;
  data.return_value = 0;
  _iterate_chunk(obj, &_cb__bacc_cmp, &data);
  return (data.return_value);
}

size_t
rdfl_bacc_info_total(t_rdfl *obj) {
  return (obj->data.consumer.total);
}

int	// corresponds to 1 byte
rdfl_bacc_ndx(t_rdfl *obj, size_t n) {
  struct s_bacc_ndx_data	data;
  data.ndx = n;
  data.return_value = 0;
  _iterate_chunk(obj, &_cb__bacc_ndx, &data);
  return (data.return_value);
}
