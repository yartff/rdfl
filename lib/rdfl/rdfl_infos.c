#include	"rdfl.h"

// Infos
//

inline
size_t
rdfl_get_total_datasize(t_rdfl *obj) {
  return (obj->data.consumer.total);
}
