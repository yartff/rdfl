#include	"buffer.h"

int
b_set_skip(t_rdfl_buffer *b, size_t value) {
#ifdef		DEVEL
  if (value >= b->consumer.total)
    return (ERRDEV_OUTOFBOUND);
#endif
  b->consumer.skip = value;
  return (ERR_NONE);
}
