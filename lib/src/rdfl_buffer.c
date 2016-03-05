#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"rdfl_buffer.h"
#include	"rdfl_local.h"

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
  } // TODO
  if (!(b->buffer.raw->next = malloc(sizeof(*(b->buffer.raw)))))
    return (EXIT_FAILURE);
  b->buffer.raw = b->buffer.raw->next;
  if (!(b->buffer.raw->data = malloc(amount))) {
    free(tmp->next);
    tmp->next = NULL;
    b->buffer.raw = tmp;
    return (EXIT_FAILURE);
  }
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
    // TODO allocate 1 buffer
  }
  return (EXIT_SUCCESS);
  printf("rdfl_buffer: %zu\nrdfl_b: %zu\nrdfl_b_list: %zu\n",
      sizeof(t_rdfl_buffer), sizeof(t_rdfl_b), sizeof(t_rdfl_b_list));
}
