#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_buffer.h"
#include	"values.h"
// #include	"context.h"
#include	"rdfl_devel.h"

// Calculations
//
inline void	*b_consumer_ptr(t_rdfl_buffer *b, size_t *s) {
  if (!b->consumer.raw) { *s = 0; return (NULL); }
  *s = (((b->consumer.ndx + b->consumer.l_total) > b->consumer.raw->size)
      ? (b->consumer.raw->size - b->consumer.ndx)
      : (b->consumer.l_total));
  return (b->consumer.raw->data + b->consumer.ndx);
}
inline static void	*get_buffer_ptr(t_rdfl_buffer *b, size_t *s) {
  if (!b->buffer.raw) { *s = 0; return (NULL); }
  *s = (b->consumer.raw->next || (b->consumer.ndx < b->buffer.ndx) || b->consumer.l_total == 0)
    ? (b->buffer.raw->size - b->buffer.ndx)
    : ((b->consumer.ndx == b->buffer.ndx) ? 0 : b->consumer.ndx - b->buffer.ndx);
  return (b->buffer.raw->data + b->buffer.ndx);
}

// Destructors
//
static
void
rdfl_b_del(t_rdfl_buffer *b) {
  t_rdfl_b_list	*freed = b->consumer.raw;
  b->consumer.raw = b->consumer.raw->next;
  if (b->consumer.raw == NULL)
    b->buffer.raw = NULL;
  free(freed->data);
  free(freed);
}

static
void
rdfl_b_del_restat(t_rdfl_buffer *b) {
  b->consumer.total -= b->consumer.l_total;
  rdfl_b_del(b);
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

static
void
rdfl_b_del_restat_butfirst(t_rdfl_buffer *b) {
  if (b->consumer.raw->next) {
    rdfl_b_del_restat(b);
    return ;
  }
  b->consumer.ndx = 0;
  b->consumer.total = 0;
  b->consumer.l_total = 0;
  b->buffer.ndx = 0;
}

#if 0 // UNUSED ATM
// Assumes there's one unique raw with data ranged from 0 to buffer.ndx
inline static void rdfl_b_sraw_restat(t_rdfl_buffer *b) {
  b->consumer.l_total = b->buffer.ndx;
  b->consumer.total = b->buffer.ndx;
  b->consumer.ndx = 0;
}

static
void
rdfl_b_clean_to_last_buffer(t_rdfl_buffer *b) {
  if (!b->consumer.raw || !b->consumer.raw->next)
    return ;
  while (b->consumer.raw->next)
    rdfl_b_del(b);
  rdfl_b_sraw_restat(b);
}
#endif

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
    rdfl_b_del(b);
  b_init(b, 0);
#ifdef		DEVEL
  return (ERR_NONE);
#endif
}

void
b_fullclean_if_empty(t_rdfl_buffer *b) {
  if (!(b->consumer.total))
    (void)b_clean(b);
}

// if consumer.raw joins buffer.raw and buffer.ndx is on the end,
// both consumer.ndx and buffer.ndx goes back to 0 << TODO to reconsider
void
b_consume_size(t_rdfl_buffer *b, size_t value) {
  value += b->consumer.skip;

  if (value > b->consumer.total) {
    value = b->consumer.total;
  }
  // TODO: ctx
#if 0
  rdfl_context_consume(b, value);
  rdfl_dropallcontexts(b);
#endif
  while (value) {
    if (value >= b->consumer.l_total) {
      value -= b->consumer.l_total;
      b->consumer.skip = ((b->consumer.skip >= b->consumer.l_total)
	  ? b->consumer.skip - b->consumer.l_total : 0);
      rdfl_b_del_restat_butfirst(b);
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

/*
 **
 **
 ** REWRITE
 **
 */

// Constructors
//
static
int
add_buffer_first(t_rdfl_buffer *b, size_t amount) {
  register t_rdfl_b_list	*tmp;

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

//
inline
void *
b_buffer_getchunk_extend(t_rdfl_buffer *b, size_t *s, size_t amount) {
  void		*ptr = get_buffer_ptr(b, s);

  if (!(*s)) {
    if (add_buffer(b, amount) == EXIT_FAILURE) {
      *s = 0;
      return (NULL);
    }
    ptr = get_buffer_ptr(b, s);
  }
  return (ptr);
}

inline
void *
b_buffer_getchunk(t_rdfl_buffer *b, size_t *s) {
  return (get_buffer_ptr(b, s));
}

int
b_set_skip(t_rdfl_buffer *b, size_t value) {
#ifdef		DEVEL
  if (value >= b->consumer.total)
    return (ERRDEV_OUTOFBOUND);
#endif
  b->consumer.skip = value;
  return (ERR_NONE);
}
