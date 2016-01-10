#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_buffer.h"

// Destructors
//
void
rdfl_buffer_clean(t_rdfl_buffer *b) {
  (void)b;
}

// Constructors
//
static
void *
_add_new(void **oldptr, size_t s) {
  *oldptr = malloc(s);
  memset(*oldptr, 0, s);
  return (*oldptr);
}

int
rdfl_b_create(t_rdfl_buffer *b, size_t amount) {
  void		*tmp;

  if (b->raw.last != NULL) {
    tmp = &b->raw.last->next;
  }
  if (!(b->raw.last = _add_new(tmp, sizeof(t_rdfl_b))))
    return (EXIT_FAILURE);
  if (!(b->raw.last->data = malloc(amount)))
    return (EXIT_FAILURE);
  b->raw.last->size = amount;
  b->raw.last->nb_used = 0;
  b->raw.last->next = NULL;
  return (EXIT_SUCCESS);
}

void
rdfl_buffer_init(t_rdfl_buffer *b, size_t amount) {
  memset(b, 0, sizeof(*b));
  if (amount != 0) {
    // TODO allocate 1 buffer
  }
  printf("rdfl_buffer: %zu\nrdfl_b: %zu\nrdfl_b_list: %zu\nrdfl_reader: %zu\n",
      sizeof(t_rdfl_buffer), sizeof(t_rdfl_b), sizeof(t_rdfl_b_list), sizeof(t_rdfl_reader));
}
