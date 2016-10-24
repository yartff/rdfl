#include	<stdlib.h>
#include	"rdfl_context.h"

static
t_ctxstack *
_get_context_byId(t_rdfl *obj, ssize_t id, size_t *total) {
  // TODO bool remove (adjust prev and next, values and total)
#ifdef		DEVEL
  if (!obj->data.consumer.ctx)
    return (NULL);
#endif
  size_t	tot = obj->data.consumer.ctx->total;
  t_ctxstack	*top = obj->data.consumer.ctx->stack;
  while (top) {
    tot += top->value;
    if (top->value == id) {
      if (total) *total = tot;
      return (top);
    }
    top = top->next;
  }
  return (NULL);
}

int
rdfl_restaurecontext(t_rdfl *obj, ssize_t id) {
  size_t	total = 0;
  if (id == -1) {
#ifdef		DEVEL
    if (!obj->data.consumer.ctx || !obj->data.consumer.ctx->stack)
      return (ERR_OUTOFBOUND);
#endif
    obj->data.consumer.skip = obj->data.consumer.ctx->total;
    return (ERR_NONE);
  }
#ifdef		DEVEL
  t_ctxstack	*ctx = _get_context_byId(obj, id, &total);
  if (!ctx) return (ERR_OUTOFBOUND);
#else
  (void)_get_context_byId(obj, id, &total);
#endif
  obj->data.consumer.skip = total;
  return (ERR_NONE);
}

static
void *
_create_context(size_t ndx, t_ctxstack *nptr, ssize_t id) {
  t_ctxstack		*new;

  if (!(new = malloc(sizeof(*new))))
    return (NULL);
  new->next = nptr;
  new->value = ndx;
  new->id = id;
  return (new);
}

ssize_t
rdfl_pushcontext(t_rdfl *obj) {
  size_t	diff;
  t_ctxstack	*n;

#ifdef		DEVEL
  if (!RDFL_OPT_ISSET(obj->settings, RDFL_CONTEXT))
    return (ERR_BADFLAGS);
  if ((obj->data.consumer.skip) >= obj->data.consumer.total)
    return (ERR_OUTOFBOUND);
#endif
  diff = obj->data.consumer.skip - obj->data.consumer.ctx->total;
  if (!(n = _create_context(diff, (obj->data.consumer.ctx ? obj->data.consumer.ctx->stack : NULL), obj->data.consumer.ctx->id))) {
    return (ERR_MEMORY);
  }
  obj->data.consumer.ctx->stack = n;
  ++obj->data.consumer.ctx->id;
  obj->data.consumer.ctx->total = obj->data.consumer.skip;
  return (ERR_NONE);
}

static
void
rdfl_ctx_pop(t_rdfl_buffer *b) {
  t_ctxstack		*l;

  if (!(l = b->consumer.ctx->stack))
    return ;
  b->consumer.ctx->stack = b->consumer.ctx->stack->next;
  b->consumer.ctx->total -= l->value;
  free(l);
}

void
rdfl_ctx_popuntil(t_rdfl_buffer *b, ctxid_t id) {
// #ifdef DEVEL
  while (b->consumer.ctx->stack) {
    if (b->consumer.ctx->stack->id == id)
      return ;
    rdfl_ctx_pop(b);
  }
}

void
rdfl_dropallcontexts(t_rdfl_buffer *b) {
  if (!b->consumer.ctx)
    return ;
  while (b->consumer.ctx->stack)
    rdfl_ctx_pop(b);
  b->consumer.ctx->id = 0;
}

int
rdfl_context_init(t_rdfl_buffer *buffer) {
  if (!(buffer->consumer.ctx = malloc(sizeof(*(buffer->consumer.ctx)))))
    return (ERR_MEMORY);
  buffer->consumer.ctx->total = 0;
  buffer->consumer.ctx->id = 0;
  buffer->consumer.ctx->stack = NULL;
  return (ERR_NONE);
}

void
rdfl_context_consume(t_rdfl_buffer *buffer, size_t s) {
  (void)buffer, (void)s;
}
