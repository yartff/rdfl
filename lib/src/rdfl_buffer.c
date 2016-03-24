#include	<unistd.h>
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
push_read(int fd, void *ptr, size_t s) {
  ssize_t	nb;
  if (s > SSIZE_MAX)
    return (B_ERR_SIZETOOBIG);
  if ((nb = read(fd, ptr, s)) == -1) {
    return (B_ERR_READ);
  }
  return (nb);
}

// Methods
//
ssize_t
rdfl_b_push_extend(t_rdfl_buffer *b, int fd, size_t s) {
  (void)b, (void)fd, (void)s;
  return (0); // TODO
}

ssize_t
rdfl_b_push(t_rdfl_buffer *b, int fd, size_t s) {
  (void)b, (void)fd, (void)s; // TODO
  return (0);
}

void *
rdfl_b_getfreechunk(t_rdfl_buffer *b, size_t *s) {
  *s = b->buffer.raw->size - b->buffer.ndx;
  return (b->buffer.raw->data + b->buffer.ndx);
}

void *
rdfl_b_getfreewritable_size(t_rdfl_buffer *b, size_t *s) {
  (void)s; // TODO
  return (b->buffer.raw);
}

size_t
rdfl_b_datachunk_size(t_rdfl_buffer *b) {
  return (b->consummer.l_total);
}

size_t
rdfl_b_data_size(t_rdfl_buffer *b) {
  return (b->consummer.total);
}

// Destructors
//
static
void
rdfl_b_del(t_rdfl_buffer *b) {
  t_rdfl_b_list	*freed = b->consummer.raw;
  b->consummer.raw = b->consummer.raw->next;
  if (b->consummer.raw == NULL)
    b->buffer.raw = NULL;
  free(freed->data);
  free(freed);
}

void
rdfl_buffer_clean(t_rdfl_buffer *b) {
  if (!b) return ;
  while (b->buffer.raw) // HERE
    rdfl_b_del(b);
}

// Constructors
//
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
  b->consummer.raw = b->buffer.raw;
  b->consummer.ndx = 0;
  b->consummer.total = 0;
  b->consummer.l_total = 0;
  return (EXIT_SUCCESS);
}

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
