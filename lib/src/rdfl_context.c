#include	<stdlib.h>
#include	"rdfl_context.h"

static
void	*
_create_context(size_t ndx, t_ctxstack *nptr) {
  t_ctxstack		*new;

  if (!(new = malloc(sizeof(*new))))
    return (NULL);
  new->next = nptr;
  new->value = ndx;
  return (new);
}

int
rdfl_pushcontext(t_rdfl *obj, size_t ndx) {
#ifdef		DEVEL
  if (!RDFL_OPT_ISSET(obj->settings, RDFL_CONTEXT))
    return (ERR_BADFLAGS);
#endif
  if ((ndx += obj->data.consumer.skip) >= obj->data.consumer.total)
    return (ERR_OUTOFBOUND);
  if (!(obj->data.consumer.ctx->stack = _create_context(ndx - obj->data.consumer.ctx->total,
	  obj->data.consumer.ctx->stack)))
    return (ERR_MEMORY);
  obj->data.consumer.ctx->total = ndx;
  return (rdfl_b_set_skip(&obj->data, ndx));
}

int
rdfl_pushcontext_from_skip(t_rdfl *obj) {
  return (rdfl_pushcontext(obj, 0));
}

int
rdfl_context_init(t_rdfl_buffer *buffer) {
  if (!(buffer->consumer.ctx = malloc(sizeof(*(buffer->consumer.ctx)))))
    return (ERR_MEMORY);
  buffer->consumer.ctx->total = 0;
  buffer->consumer.ctx->stack = NULL;
  return (ERR_NONE);
}

int
rdfl_shiftcontext(t_rdfl *obj, size_t n) {
  if (!(obj->data.consumer.ctx->stack))
    return (rdfl_pushcontext(obj, n));
  if (!(obj->data.consumer.ctx->stack->next)) {
    obj->data.consumer.skip += n;
    obj->data.consumer.ctx->total = obj->data.consumer.skip;
    obj->data.consumer.ctx->stack->value = obj->data.consumer.skip;
  }
  else
    obj->data.consumer.ctx->stack->value -= n;
  return (ERR_NONE);
}

void
rdfl_popcontext(t_rdfl *obj) {
  t_ctxstack	*top = obj->data.consumer.ctx->stack;

  if (!(top))
    return ;
  obj->data.consumer.ctx->total -= top->value;
  obj->data.consumer.ctx->stack = top->next;
  free(top);
}

void
rdfl_shiftall(t_rdfl *obj, size_t n) {
  t_ctxstack	*ctxs = obj->data.consumer.ctx->stack;

  while (ctxs) {
    if (n >= ctxs->value) {
      n -= ctxs->value;
      ctxs = ctxs->next;
      (void)rdfl_popcontext(obj);
    }
    else {
      ctxs->value -= n;
      return ;
    }
  }
}

void
rdfl_restaurecontext(t_rdfl *obj) {
#ifdef	DEVEL
  if (!RDFL_OPT_ISSET(obj->settings, RDFL_CONTEXT))
    return (ERR_BADFLAGS);
#endif
  obj->data.consumer.skip = obj->data.consumer.ctx->total;
}
