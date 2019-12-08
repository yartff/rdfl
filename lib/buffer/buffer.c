#include	<string.h>
#include	<stdlib.h>
#include	"buffer.h"
// #include	"context.h"

static int add_buffer(t_rdfl_buffer *b, size_t amount);

// Chunk Getters
//
void *
b_buffer_ptr_extend(t_rdfl_buffer *b, size_t *s, size_t amount) {
  void		*ptr = b_buffer_ptr(b, s);

  if (!(*s)) {
    if (add_buffer(b, amount) == EXIT_FAILURE) {
      *s = 0;
      return (NULL);
    }
    ptr = b_buffer_ptr(b, s);
  }
  return (ptr);
}

void	*b_consumer_ptr(t_rdfl_buffer *b, size_t *s) {
  if (!b->consumer.raw) { *s = 0; return (NULL); }
  *s = (((b->consumer.ndx + b->consumer.l_total) > b->consumer.raw->size)
      ? (b->consumer.raw->size - b->consumer.ndx)
      : (b->consumer.l_total));
  return (b->consumer.raw->data + b->consumer.ndx);
}

void	*b_buffer_ptr(t_rdfl_buffer *b, size_t *s) {
  if (!b->buffer.raw) { *s = 0; return (NULL); }
  *s = (b->consumer.raw->next || (b->consumer.ndx < b->buffer.ndx) || b->consumer.l_total == 0)
    ? (b->buffer.raw->size - b->buffer.ndx)
    : ((b->consumer.ndx == b->buffer.ndx) ? 0 : b->consumer.ndx - b->buffer.ndx);
  return (b->buffer.raw->data + b->buffer.ndx);
}

// Constructors
//
static
int
add_buffer_first(t_rdfl_buffer *b, size_t amount) {
  register t_rdfl_blist	*tmp;

  if (!(tmp = malloc(sizeof(*(b->buffer.raw)))))
    return (EXIT_FAILURE);
  if (!(tmp->data = malloc(amount))) {
    free(tmp);
    return (EXIT_FAILURE);
  }
  tmp->size = amount;
  tmp->next = NULL;

  b->buffer.raw = tmp;
  b->consumer.raw = tmp;

  b->buffer.ndx = 0;
  b->consumer.ndx = 0;
  b->consumer.total = 0;
  b->consumer.l_total = 0;
  return (EXIT_SUCCESS);
}

static
int
add_buffer_last(t_rdfl_buffer *b, size_t amount) {
  if (!(b->buffer.raw->next = malloc(sizeof(*(b->buffer.raw->next)))))
    return (EXIT_FAILURE);
  if (!(b->buffer.raw->next->data = malloc(amount))) {
    free(b->buffer.raw->next);
    b->buffer.raw->next = NULL;
    return (EXIT_FAILURE);
  }
  b->buffer.raw = b->buffer.raw->next;
  b->buffer.raw->size = amount;
  b->buffer.raw->next = NULL;
  b->buffer.ndx = 0;
  return (EXIT_SUCCESS);
}

static
int
add_buffer(t_rdfl_buffer *b, size_t amount) {
  int ret = (b->buffer.raw == NULL)
    ? add_buffer_first(b, amount)
    : add_buffer_last(b, amount);
  // TODO: if ret == EXIT_FAILURE then ERR_MEMORY_ALLOC
  return (ret);
}

int
b_init(t_rdfl_buffer *b, size_t amount) {
  memset(b, 0, sizeof(*b));
  if (amount != 0) {
    if (add_buffer(b, amount) == EXIT_FAILURE)
      return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// Destructors
//
static
void
delete_consumer_blist(t_rdfl_buffer *b) {
  t_rdfl_blist	*freed = b->consumer.raw;
  b->consumer.raw = b->consumer.raw->next;
  if (b->consumer.raw == NULL)
    b->buffer.raw = NULL;
  free(freed->data);
  free(freed);
}

static
void
delete_consumer_blist_restat(t_rdfl_buffer *b) {
  b->consumer.total -= b->consumer.l_total;
  delete_consumer_blist(b);
  b->consumer.ndx = 0;
  if (b->consumer.raw) {
    if (b->consumer.raw->next)
      b->consumer.l_total = b->consumer.raw->size;
    else {
      b->consumer.l_total = b->buffer.ndx;
      if (b->buffer.ndx == b->buffer.raw->size)
	b->buffer.ndx = 0;
    }
  } else {
    b->consumer.l_total = 0;
    b->buffer.ndx = 0;
  }
}

void
leave_unique_consumer_blist(t_rdfl_buffer *b) {
  if (b->consumer.raw->next) {
    delete_consumer_blist_restat(b);
    return ;
  }
  b->consumer.ndx = 0;
  b->consumer.total = 0;
  b->consumer.l_total = 0;
  b->buffer.ndx = 0;
}

inline
void
b_fullclean_if_empty(t_rdfl_buffer *b) {
  if (!(b->consumer.total))
    (void)b_clean(b);
}

rdfl_retval
b_clean(t_rdfl_buffer *b) {
#ifdef		DEVEL
  if (!b) return (ERRDEV_NULLOBJECT);
#endif
// TODO: ctx
#if 0
  rdfl_dropallcontexts(b);
  free(b->consumer.ctx);
#endif
  while (b->consumer.raw)
    delete_consumer_blist(b);
  memset(b, 0, sizeof(*b));
#ifdef		DEVEL
  return (ERR_NONE);
#endif
}
