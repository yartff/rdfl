#include	<unistd.h>
#include	<sys/select.h>
#include	<limits.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_buffer.h"
#include	"rdfl_local.h"

// read must check for following errors:
// EAGAIN E_WOULDBLOCK
static
ssize_t
push_read(t_rdfl_buffer *b, int fd, void *ptr, size_t s) {
  ssize_t	nb;
  if (s > SSIZE_MAX)
    return (B_ERR_SIZETOOBIG);
  if ((nb = read(fd, ptr, s)) == -1) {
    return (B_ERR_READ);
  }
  if ((b->buffer.ndx += nb) == b->buffer.raw->size)
    b->buffer.ndx = 0;
  b->consumer.total += nb;
  return (nb);
}

// Calculations
inline static void	*rdfl_b_sraw_regular_getconsume_chunk(t_rdfl_buffer *b, size_t *s)
{ *s = b->consumer.l_total; return (b->consumer.raw->data + b->consumer.ndx); }
inline static size_t	rdfl_b_sraw_regular_getconsume_all(t_rdfl_buffer *b)
{ return (b->consumer.l_total); }
inline static void	*rdfl_b_sraw_regular_buffer_chunk(t_rdfl_buffer *b, size_t *s)
{ *s = b->buffer.raw->size - b->buffer.ndx; return (b->buffer.raw->data + b->buffer.ndx); }
inline static size_t	rdfl_b_sraw_regular_buffer_allsize(t_rdfl_buffer *b)
{ return (b->buffer.raw->size - b->consumer.l_total); }

inline static void	*rdfl_b_sraw_circular_getconsume_chunk(t_rdfl_buffer *b, size_t *s)
{ *s = b->consumer.raw->size - b->consumer.ndx; return (b->consumer.raw->data + b->consumer.ndx); }
inline static size_t	rdfl_b_sraw_circular_getconsume_all(t_rdfl_buffer *b)
{ return (b->consumer.l_total); }
inline static size_t	rdfl_b_sraw_circular_buffer_allsize(t_rdfl_buffer *b)
{ return (b->buffer.raw->size - b->consumer.l_total); }
inline static void	*rdfl_b_sraw_circular_buffer_chunk(t_rdfl_buffer *b, size_t *s)
{ *s = rdfl_b_sraw_circular_buffer_allsize(b); return (b->buffer.raw->data + b->buffer.ndx); }

inline static void	*rdfl_b_mraw_getconsume_chunk(t_rdfl_buffer *b, size_t *s)
{ return (((b->consumer.ndx + b->consumer.l_total) > b->consumer.raw->size)
    ? rdfl_b_sraw_circular_getconsume_chunk(b, s)
    : rdfl_b_sraw_regular_getconsume_chunk(b, s)); }
inline static size_t	rdfl_b_mraw_getconsume_all(t_rdfl_buffer *b)
{ return (b->consumer.total); }
inline static void	*rdfl_b_mraw_buffer(t_rdfl_buffer *b, size_t *s)
{ return (rdfl_b_sraw_regular_buffer_chunk(b, s)); }

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
  b->consumer.l_total = ((b->consumer.raw->next)
      ? b->consumer.raw->size
      : b->buffer.ndx);
  b->consumer.ndx = 0;
}

void
rdfl_buffer_clean(t_rdfl_buffer *b) {
  if (!b) return ;
  while (b->buffer.raw) // HERE
    rdfl_b_del(b);
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
  b->buffer.ndx = 0;
  b->consumer.raw = b->buffer.raw;
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

static
int
rdfl_b_add(t_rdfl_buffer *b, size_t amount) {
  if (b->buffer.raw == NULL)
    return (rdfl_b_add_first(b, amount));
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

// Methods
//

static
int
rdfl_b_isbufferfull(t_rdfl_buffer *b) {
  if (b->consumer.raw->next == NULL)
    return (b->buffer.raw->size == b->consumer.l_total);
  return (b->buffer.raw->size == b->buffer.ndx);
}

#if 0 // Unused atm
static
ssize_t
rdfl_b_push_circular(t_rdfl_buffer *b, int fd) {
  size_t	s;
  void		*ptr = rdfl_b_sraw_circular_buffer_chunk(b, &s);
  ssize_t	pushed;

  if (!s)
    return (-1);
  if ((pushed = push_read(b, fd, ptr, s)) < 0)
    return (-1);
  return (pushed);
}

static
ssize_t
rdfl_b_push_regular(t_rdfl_buffer *b, int fd) {
  size_t	s;
  void		*ptr = rdfl_b_sraw_regular_buffer_chunk(b, &s);
  ssize_t	pushed;

  if (!s)
    return (-1);
  if ((pushed = push_read(b, fd, ptr, s)) < 0)
    return (-1);
  return (pushed);
}

static
ssize_t
rdfl_b_push_circular_canread(t_rdfl_buffer *b, int fd, int *can_read) {
  size_t	s;
  void		*ptr = rdfl_b_sraw_circular_buffer_chunk(b, &s);
  ssize_t	pushed;

  if (!s)
    return (-1);
  if ((pushed = push_read(b, fd, ptr, s)) < 0) {
    return (-1);
  }
  if (can_read != NULL) *can_read = (pushed == (ssize_t)s);
  return (pushed);
}

static
ssize_t
rdfl_b_push_regular_canread(t_rdfl_buffer *b, int fd, int *can_read) {
  size_t	s;
  void		*ptr = rdfl_b_sraw_regular_buffer_chunk(b, &s);
  ssize_t	pushed;

  if (!s)
    return (-1);
  if ((pushed = push_read(b, fd, ptr, s)) < 0)
    return (-1);
  if (can_read != NULL) *can_read = (pushed == (ssize_t)s);
  return (pushed);
}

// won't extend // Untested
static
ssize_t
rdfl_b_push_sraw(t_rdfl_buffer *b, int fd) {
  ssize_t	pushed;
  if (b->consumer.ndx + b->consumer.l_total > b->consumer.raw->size) {
    if ((pushed = rdfl_b_push_circular(b, fd)) < 0)
      return (-1);
    b->consumer.l_total += pushed;
    return (pushed);
  }
  if ((pushed = rdfl_b_push_regular(b, fd)) < 0)
    return (-1);
  b->consumer.l_total += pushed;
  return (pushed);
}

static
ssize_t
rdfl_b_push_sraw_canread(t_rdfl_buffer *b, int fd, int *can_read) {
  ssize_t	pushed;
  if (b->consumer.ndx + b->consumer.l_total > b->consumer.raw->size) {
    if ((pushed = rdfl_b_push_circular_canread(b, fd, can_read)) < 0) {
      return (-1);
    }
    b->consumer.l_total += pushed;
    return (pushed);
  }
  if ((pushed = rdfl_b_push_regular_canread(b, fd, can_read)) < 0)
    return (-1);
  b->consumer.l_total += pushed;
  return (pushed);
}
#endif

//
static
ssize_t
rdfl_b_push_extend_pusher(t_rdfl_buffer *b, int fd, void *ptr, int available,
    int *can_read) {
  ssize_t	pushed;
  if ((pushed = push_read(b, fd, ptr, available)) < 0)
    return (-1);
  if (can_read != NULL) *can_read = (pushed == (ssize_t)available);
  return (pushed);
}

static
ssize_t
rdfl_b_push_extend_pusher_restatfirst(t_rdfl_buffer *b,
    int fd, void *ptr, int available, int *can_read) {
  ssize_t	pushed;
  if ((pushed = push_read(b, fd, ptr, available)) < 0)
    return (-1);
  if (can_read != NULL) *can_read = (pushed == (ssize_t)available);
  b->consumer.l_total += pushed;
  return (pushed);
}

static
ssize_t
rdfl_b_push_extend(t_rdfl_buffer *b, int fd, size_t buffersize, int *can_read) {
  size_t	available;
  void		*ptr;

  if (!b->consumer.raw) {
    if (rdfl_b_add_first(b, buffersize) == EXIT_FAILURE) {
      return (-1);
    }
    ptr = rdfl_b_sraw_regular_buffer_chunk(b, &available);
    return (rdfl_b_push_extend_pusher_restatfirst(b, fd, ptr, available, can_read));
  }
  if (rdfl_b_isbufferfull(b) || b->consumer.raw->next) {
    if (rdfl_b_add_last(b, buffersize) == EXIT_FAILURE) {
      return (-1);
    }
    ptr = rdfl_b_mraw_buffer(b, &available);
    return (rdfl_b_push_extend_pusher(b, fd, ptr, available, can_read));
  }
  if (b->consumer.ndx + b->consumer.l_total > b->consumer.raw->size)
    ptr = rdfl_b_sraw_regular_buffer_chunk(b, &available);
  else
    ptr = rdfl_b_sraw_circular_buffer_chunk(b, &available);
  return (rdfl_b_push_extend_pusher_restatfirst(b, fd, ptr, available, can_read));
}


// To use when no monitoring
static
ssize_t
rdfl_b_push_all_local_select(int fd, fd_set *fds, struct timeval *timeout_ptr) {
  fd_set	tmp;

  memcpy(&tmp, fds, sizeof(tmp));
  return (select(fd + 1, &tmp, NULL, NULL, timeout_ptr));
}
// Only free from the chunk.
// Be careful to what you send
static
void
rdfl_b_consume_size_chunk(t_rdfl_buffer *b, ssize_t s) {
  // Shouldn't have to check for these.
  // Especially to not double check obvious locally managed data
  // if (!b->consumer.raw || !b->consumer.total) return ;
  if (b->consumer.l_total == (size_t)s) {
    // Supports double chunks only if total of the block is consummed
    if (b->consumer.raw->next) {
      rdfl_b_del_restat(b);
    }
    else { // Reset the last remaining block // TODO RDFL_FREE_ALL
      b->consumer.l_total = 0;
      b->consumer.total = 0;
      b->consumer.ndx = 0;
    }
    return ;
  }
  if ((b->consumer.ndx += s) == b->consumer.raw->size)
    b->consumer.ndx = 0;
  b->consumer.l_total -= s;
  b->consumer.total -= s;
}

// Assumes ptr points to enough space
ssize_t
rdfl_b_consume_next_chunk(t_rdfl_buffer *b, void *ptr) {
  size_t	s;
  void		*copy;

  copy = rdfl_b_mraw_getconsume_chunk(b, &s);
  memcpy(ptr, copy, s);
  rdfl_b_consume_size_chunk(b, s);
  return (s);
}

//
// Public Extenders
ssize_t
rdfl_b_push_all_local_monitoring(t_rdfl_buffer *b,
    int fd, ssize_t buffersize, long timeout_value) {
  fd_set	readfds;
  struct timeval timeout;
  struct timeval timeout_tmp;
  int		ret;
  ssize_t	pushed = 0, save;

  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  if (timeout_value == -1) {
    for (;;) {
      if ((ret = rdfl_b_push_all_local_select(fd, &readfds, NULL)) == -1)
	return (-1);
      if (!ret) return (pushed);
      if ((save = rdfl_b_push_extend(b, fd, buffersize, NULL)) == -1)
	return (-1);
      pushed += save;
    }
  }
  timeout.tv_sec = timeout_value / 1000000;
  timeout.tv_usec = timeout_value % 1000000;
  for (;;) {
    memcpy(&timeout_tmp, &timeout, sizeof(timeout_tmp));
    if ((ret = rdfl_b_push_all_local_select(fd, &readfds, &timeout_tmp)) == -1)
      return (-1);
    if (!ret) return (pushed);
    if ((save = rdfl_b_push_extend(b, fd, buffersize, NULL)) == -1)
      return (-1);
    pushed += save;
  }
}

ssize_t
rdfl_b_push_all_local(t_rdfl_buffer *b, int fd, size_t buffersize) {
  ssize_t	pushed;
  ssize_t	total_pushed;
  int		can_read;

  if ((total_pushed = rdfl_b_push_extend(b, fd, buffersize, &can_read)) < 0)
    return (-1);
  while (can_read == 1) {
    if ((pushed = rdfl_b_push_extend(b, fd, buffersize, &can_read)) < 0) {
      return (-1);
    }
    total_pushed += pushed;
  }
  return (total_pushed);
}

//
// Public consumers
void *
rdfl_b_consume_all(t_rdfl_buffer *b, ssize_t *count_value) {
  void		*ptr;
  size_t	ndx = 0;

  if (count_value) *count_value = b->consumer.total;
  if (!(ptr = malloc(b->consumer.total))) {
    if (count_value) *count_value = -1;
    return (NULL);
  }
  while (b->consumer.total) {
    ndx += rdfl_b_consume_next_chunk(b, ptr + ndx);
  }
  return (ptr);
}

void
rdfl_b_fullclean_if_empty(t_rdfl_buffer *b) {
  if (!(b->consumer.total))
    rdfl_buffer_clean(b);
}
