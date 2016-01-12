#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_buffer.h"

// Destructors
//
static
void
clean_reader(t_rdfl_reader *r) {
  t_rdfl_buffer		*tmp;
  while (r) {
    tmp = r;
    r = r->next;
    free(tmp);
  }
}

void
rdfl_buffer_clean(t_rdfl_buffer *b) {
  if (!b) return ;
  while (b->first) // HERE
  if (b->consummer) {
    clean_reader(b->consummer);
    free(b->consummer);
  }
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
  void		*tmp = b->buffer;

  if (b->list != NULL) {
    tmp = &b->buffer;
  } // TODO
  if (!(b->raw.last = _add_new(tmp, sizeof(t_rdfl_b))))
    goto bfailure;
  if (!(b->raw.last->data = malloc(amount)))
    goto bfailure;
  b->raw.last->size = amount;
  b->raw.last->nb_used = 0;
  b->raw.last->next = NULL;
  return (EXIT_SUCCESS);
bfailure:
  rdfl_buffer_clean(b);
  return (EXIT_FAILURE);
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
