#include		"buffer.h"

// TODO method with extract?
size_t
rdfl_acc_info_total(t_rdfl *obj) {
  return (obj->data.consumer.total);
}
