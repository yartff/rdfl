#include		<string.h>
#include		"rdfl_buffer_access.h"

int
rdfl_bacc_cmp(t_rdfl *obj, void *ptr, size_t s) {
  size_t			total = obj->data.consumer.total, l_total, offset = 0, chunk_data_size;
  t_rdfl_b_list			*chunk = obj->data.consumer.raw;
  void				*chunk_data;

  if (!s) s = total;
  else if (s < total) total = s;
  else if (s > total) return (0);
  // No check, we rely on total value
  chunk_data = rdfl_b_next_chunk(&obj->data, &chunk_data_size);
  if (chunk_data_size >= total)
    return (!memcmp(chunk_data, ptr, total));
  if (memcmp(chunk_data, ptr, chunk_data_size))
    return (0);
  total -= chunk_data_size;
  if (chunk_data_size < obj->data.consumer.l_total) {
    offset = obj->data.consumer.l_total - chunk_data_size;
    if (offset > total) offset = total;
    if (memcmp(chunk->data, ptr + chunk_data_size, offset))
      return (0);
    total -= offset;
  }
  offset += chunk_data_size;
  chunk = chunk->next;
  while (total) {
    l_total = total < chunk->size ? total : chunk->size;
    if (memcmp(chunk->data, ptr + offset, l_total))
      return (0);
    total -= l_total;
    offset += l_total;
    chunk = chunk->next;
  }
  return (1);
}
