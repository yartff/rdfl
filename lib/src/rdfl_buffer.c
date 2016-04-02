#include	<unistd.h>
#include	<sys/select.h>
#include	<limits.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_buffer.h"
#include	"rdfl_local.h"

// Calculations
//
inline static void	*rdfl_b_consumer_ptr(t_rdfl_buffer *b, size_t *s) {
  if (!b->consumer.raw) { *s = 0; return (NULL); }
  *s = (((b->consumer.ndx + b->consumer.l_total) > b->consumer.raw->size) ?
      (b->consumer.raw->size - b->consumer.ndx) : (b->consumer.l_total));
  return (b->consumer.raw->data + b->consumer.ndx);
}
inline static void	*rdfl_b_buffer_ptr(t_rdfl_buffer *b, size_t *s) {
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

// Assumes there's one unique raw with data ranged from 0 to buffer.ndx
inline static void rdfl_b_sraw_restat(t_rdfl_buffer *b) {
  b->consumer.l_total = b->buffer.ndx;
  b->consumer.total = b->buffer.ndx;
  b->consumer.ndx = 0;
}

#if 0 // UNUSED ATM
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

void
rdfl_buffer_clean(t_rdfl_buffer *b) {
  while (b->consumer.raw)
    rdfl_b_del(b);
  rdfl_buffer_init(b, 0);
}

void
rdfl_b_fullclean_if_empty(t_rdfl_buffer *b) {
  if (!(b->consumer.total))
    rdfl_buffer_clean(b);
}

// Constructors
//
static
int
rdfl_b_add_first(t_rdfl_buffer *b, size_t amount) {
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
rdfl_b_add_last(t_rdfl_buffer *b, size_t amount) {
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

int
rdfl_b_add(t_rdfl_buffer *b, size_t amount) {
  if (b->buffer.raw == NULL) {
    return (rdfl_b_add_first(b, amount));
  }
  return (rdfl_b_add_last(b, amount));
}

int
rdfl_buffer_init(t_rdfl_buffer *b, size_t amount) {
  memset(b, 0, sizeof(*b));
  if (amount != 0) {
    if (rdfl_b_add(b, amount) == EXIT_FAILURE)
      return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// Public consumers
//

void *
rdfl_b_next_chunk(t_rdfl_buffer *b, size_t *s) {
  return (rdfl_b_consumer_ptr(b, s));
}

// if consumer.raw joins buffer.raw and buffer.ndx is on the end,
// buffer.ndx goes back to 0 << TODO to reconsider
void
rdfl_b_consume_size(t_rdfl_buffer *b, size_t value) {
  if (value > b->consumer.total) {
    value = b->consumer.total;
  }
  while (value) {
    if (value >= b->consumer.l_total) {
      value -= b->consumer.l_total;
      rdfl_b_del_restat(b);
    }
    else {
      b->consumer.l_total -= value;
      b->consumer.total -= value;
      b->consumer.ndx = (b->consumer.ndx + value) % b->consumer.raw->size;
      value = 0;
      return ;
    }
  }
}

static
ssize_t
rdfl_b_consume_next_chunk(t_rdfl_buffer *b, void *ptr) {
  size_t	s;
  void		*copy;

  copy = rdfl_b_consumer_ptr(b, &s);
  memcpy(ptr, copy, s);
  rdfl_b_consume_size(b, s);
  return (s);
}

void *
rdfl_b_consume_all_alloc(t_rdfl_buffer *b, ssize_t *count_value) {
  void		*ptr;
  size_t	ndx = 0;

  if (count_value) *count_value = b->consumer.total;
  if (!b->consumer.total) return (NULL);
  if (!(ptr = malloc(b->consumer.total))) {
    if (count_value) *count_value = -1;
    return (NULL);
  }
  while (b->consumer.total) {
    ndx += rdfl_b_consume_next_chunk(b, ptr + ndx);
  }
  return (ptr);
}

void *
rdfl_b_consume_firstbuffer_alloc(t_rdfl_buffer *b, ssize_t *count_value) {
  void		*ptr;
  size_t	ndx = 0;
  t_rdfl_b_list	*raw_tmp;

  if (count_value) *count_value = b->consumer.l_total;
  if (!b->consumer.l_total) return (NULL);
  if (!(ptr = malloc(b->consumer.l_total))) {
    if (count_value) *count_value = -1;
    return (NULL);
  }
  raw_tmp = b->consumer.raw;
  while (b->consumer.raw == raw_tmp) {
    ndx += rdfl_b_consume_next_chunk(b, ptr + ndx);
  }
  return (ptr);
}

// Public buffering
//
inline
void *
rdfl_b_buffer_getchunk_extend(t_rdfl_buffer *b, size_t *s, size_t amount) {
  void		*ptr = rdfl_b_buffer_ptr(b, s);

  if (!(*s)) {
    if (rdfl_b_add(b, amount) == EXIT_FAILURE) {
      *s = 0;
      return (NULL); // TODO err
    }
    ptr = rdfl_b_buffer_ptr(b, s);
  }
  return (ptr);
}

inline
void *
rdfl_b_buffer_getchunk(t_rdfl_buffer *b, size_t *s) {
  return (rdfl_b_buffer_ptr(b, s));
}

// buffer.ndx should not got back to 0 when not on the consumer buffer
void
rdfl_b_buffer_size(t_rdfl_buffer *b, size_t value) {
  if (!(b->consumer.raw->next))
    b->consumer.l_total += value;
  b->consumer.total += value;
  if ((b->buffer.ndx += value) == b->buffer.raw->size && !b->consumer.raw->next)
    b->buffer.ndx = 0;
}

// just reads s bytes from buffer pointer
// Does NOT check for overflows. Be careful to what you send;
ssize_t
rdfl_b_push_read(t_rdfl_buffer *b, int fd, void *ptr, size_t s) {
  ssize_t	nb;

  /*
     printf("c:%zu _ b:%zu\n", b->consumer.ndx, b->buffer.ndx);
     fflush(stdout);
     getchar();
     */
  if (s > SSIZE_MAX)
    return (-1);
  if ((nb = read(fd, ptr, s)) == -1) {
    // TODO read must check for following errors:
    // EAGAIN E_WOULDBLOCK
    return (-1);
  }
  rdfl_b_buffer_size(b, (size_t)nb);
  return (nb);
}


// Debugging
//
//
void
rdfl_b_print_buffers(t_rdfl_buffer *b) {
  (void)b;
}
