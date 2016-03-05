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
    return (-1);
  }
}

ssize_t
rdfl_b_push_extend(t_rdfl_buffer *b, int fd, size_t s) {
  (void)b, (void)fd, (void)s;
}

ssize_t
rdfl_b_push(t_rdfl_buffer *b, int fd, size_t s) {
  (void)b, (void)fd, (void)s;
}

void *
rdfl_b_getdataptr(t_rdfl_buffer *b, size_t *s) {
  *s = b->consummer.raw->size - b->consummer.ndx;
  return (b->consummer.raw);
}

void *
rdfl_b_getfreechunkptr(t_rdfl_buffer *b, size_t *s) {
  return (b->buffer.raw);
}

int
rdfl_b_consummechunk_size(t_rdfl_buffer *b, size_t s) {
}

size_t
rdfl_b_datasize(t_rdfl_buffer *b) {
}

// Destructors
//
void
rdfl_b_del(t_rdfl_buffer *b) {
  t_rdfl_b_list	*freed = b->buffer.raw;
  b->buffer.raw = b->buffer.raw->next;
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
rdfl_b_add(t_rdfl_buffer *b, size_t amount) {
  t_rdfl_b_list		*tmp;

  if (b->buffer.raw != NULL) {
    tmp = b->consummer.raw;
  }
  if (!(b->buffer.raw->next = malloc(sizeof(*(b->buffer.raw)))))
    return (EXIT_FAILURE);
  b->buffer.raw = b->buffer.raw->next;
  if (!(b->buffer.raw->data = malloc(amount))) {
    free(tmp->next);
    tmp->next = NULL;
    b->buffer.raw = tmp;
    return (EXIT_FAILURE);
  }
  b->buffer.ndx = 0;
  b->buffer.raw->size = amount;
  b->buffer.raw->next = NULL;
  return (EXIT_SUCCESS);
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
