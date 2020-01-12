#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_types.h"
#include	"buffer.h"

static
ssize_t
consume_next_chunk(t_rdfl_buffer *b, void *ptr) {
  size_t	s;
  void		*copy;

  copy = b_consumer_ptr(b, &s);
  memcpy(ptr, copy, s);
  b_consume_size(b, s);
  return (s);
}

void *
b_consume_all_alloc(t_rdfl_buffer *b, ssize_t *count_value) {
  void		*ptr;
  size_t	ndx = 0;

  if (count_value) *count_value = b->consumer.total;
  if (!b->consumer.total) return (NULL);
  if (!(ptr = malloc(b->consumer.total))) {
    if (count_value) *count_value = ERR_MEMORY_ALLOC;
    return (NULL);
  }
  while (b->consumer.total) {
    ndx += consume_next_chunk(b, ptr + ndx);
  }
  return (ptr);
}

void *
b_consume_firstbuffer_alloc(t_rdfl_buffer *b, ssize_t *count_value) {
  void		*ptr;
  size_t	ndx = 0;
  t_rdfl_blist	*raw_tmp;

  if (count_value) *count_value = b->consumer.l_total;
  if (!b->consumer.l_total) return (NULL);
  if (!(ptr = malloc(b->consumer.l_total))) {
    if (count_value) *count_value = ERR_MEMORY_ALLOC;
    return (NULL);
  }
  raw_tmp = b->consumer.raw;
  while (b->consumer.raw == raw_tmp) {
    ndx += consume_next_chunk(b, ptr + ndx);
  }
  return (ptr);
}

// if consumer.raw joins buffer.raw and buffer.ndx is on the end,
// both consumer.ndx and buffer.ndx goes back to 0 << TODO to reconsider
void
b_consume_size(t_rdfl_buffer *b, size_t value) {
  value += b->consumer.skip;

  // TODO: ctx
  if (value > b->consumer.total) {
    value = b->consumer.total;
  }
  while (value) {
    if (value >= b->consumer.l_total) {
      value -= b->consumer.l_total;
      b->consumer.skip = ((b->consumer.skip >= b->consumer.l_total)
	  ? b->consumer.skip - b->consumer.l_total : 0);
      leave_unique_consumer_blist(b);
    }
    else {
      b->consumer.l_total -= value;
      b->consumer.total -= value;
      b->consumer.skip = 0;
      b->consumer.ndx = (b->consumer.ndx + value) % b->consumer.raw->size;
      return ;
    }
  }
}
