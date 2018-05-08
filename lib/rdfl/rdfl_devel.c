#ifdef		DEVEL

# include	<unistd.h>
# include	<limits.h>
# include	<stdio.h>
# include	<string.h>
# include	<stdlib.h>
# include	"values.h"
// # include	"context.h"
# include	"rdfl_devel.h"

// RDFL

// Helpers
//
static struct {
  void			*fct;
  const char		*name;
  const char		*func;
  e_rdflsettings	flag;
}	readersTable[] = {{},
  // Order matters. Starts at 1
  {&rdflReader_size, "readsize_handler_t", "rdflReader_size", RDFL_FORCEREADSIZE},
  {&rdflReader_monitoring_no_extend, "readmonitoringnoext_handler_t", "rdflReader_monitoring_no_extend", RDFL_MONITORING | RDFL_NO_EXTEND},
  {&rdflReader_monitoring_allavail, "readmonitoringall_handler_t", "rdflReader_monitoring_allavail", RDFL_MONITORING | RDFL_ALL_AVAILABLE},
  {&rdflReader_monitoring, "readmonitoring_handler_t", "rdflReader_monitoring", RDFL_MONITORING},
  {&rdflReader_noextend, "readnoextend_handler_t", "rdflReader_noextend", RDFL_NO_EXTEND},
  {&rdflReader_all_available, "readall_handler_t", "rdflReader_all_available", RDFL_ALL_AVAILABLE},
  {&rdflReader_legacy, "readlegacy_handler_t", "rdflReader_legacy", RDFL_LEGACY},
  {&rdflReader_singlestep, "readsinglestep_t", "rdflReader_singlestep", RDFL_NONE},
};

static
unsigned int
get_readersTable_ndx(void *ptr) {
  unsigned int	i = 0;

  while (++i < (sizeof(readersTable) / sizeof(*readersTable))) {
    if (ptr == readersTable[i].fct)
      return (i);
  }
  return (0);
}

const char *
rdflDevel_handler_typedef_declare(void *ptr) {
  unsigned int i = get_readersTable_ndx(ptr);
  return (i ? readersTable[i].name : NULL);
}

const char *
rdflDevel_handler_func_declare(void *ptr) {
  unsigned int i = get_readersTable_ndx(ptr);
  return (i ? readersTable[i].func : NULL);
}

void *
rdflDevel_get_func(e_rdflsettings settings) {
  unsigned int	i = 0;
  while (++i < ((sizeof(readersTable) / sizeof(*readersTable)) - 1)) {
    if (RDFL_OPT_CONTAINALL(settings, readersTable[i].flag))
      return (readersTable[i].fct);
  }
  return (&rdflReader_singlestep);
}

// Debugging
//
static
void
print_chars(char c, size_t count) {
  while (count--) {
    printf("%c", c);
  }
}

static
void
print_buffer(const char *str, size_t count) {
  size_t	i = 0;
  printf("\033[0;34m");
  while (i < count) {
    if (RDFL_IS_ASCII_PRINTABLECHAR(str[i])) {
      printf("%c",  str[i]);
    }
    else {
      printf("\033[1;32m%c\033[0;34m", RDFL_IS_ASCII_NODISPLAYCHAR(str[i]) ? '.' : '?');
    }
    ++i;
  }
  printf("\033[0m");
}

/* TODO: ctx
static
void
print_contexts(t_rdfl_buffer *b) {
  t_ctxstack	*tmp;
  if (!b->consumer.ctx || !b->consumer.ctx->stack) {
    printf("%s\n", "No contexts");
    return ;
  }
  tmp = b->consumer.ctx->stack;
  while (tmp) {
    printf("[%zd]=(%zd)\n", tmp->id, tmp->value);
    tmp = tmp->next;
  }
}
*/

// TODO display size of each buffer
// TODO display contexts
static
void
print_buffers(t_rdfl_buffer *b) {
  t_rdfl_b_list		*raw = b->consumer.raw;
  //t_ctxstack		*l;
  size_t		tmp;

  if (!raw) {
    printf("\033[0;33mTOT: \033[0m%zu\nNo buffers.\n" , b->consumer.total);
    return ;
  }
  /* TODO: ctx
  if (b->consumer.ctx) {
    l = b->consumer.ctx->stack;
    while (l != NULL) {
      printf("_%zd_\n", l->value);
      l = l->next;
    }
  }
  */
  printf("\033[0;33mTOT: \033[0m%zu \033[0;33m"
      "(First TOT: \033[0m%zu\033[0;33m/\033[0m%zu\033[0;33m) -> %zu"
      "\n\033[1;31m{\033[0m", b->consumer.total,
      b->consumer.l_total, b->consumer.raw->size, b->consumer.skip);
  while (raw) {
    if (raw == b->consumer.raw) {
      printf("\033[1;31m[\033[0m");
      if (b->consumer.ndx + b->consumer.l_total > raw->size) {
	//print_chars('.', (tmp = (b->consumer.l_total - (raw->size - b->consumer.ndx))));
	print_buffer(raw->data, (tmp = (b->consumer.l_total - (raw->size - b->consumer.ndx))));
	print_chars('_', b->consumer.ndx - tmp);
	print_buffer(raw->data + b->consumer.ndx, raw->size - b->consumer.ndx);
      }
      else {
	print_chars('_', b->consumer.ndx);
	print_buffer(raw->data + b->consumer.ndx, b->consumer.l_total);
	print_chars('_', b->consumer.raw->size - b->consumer.l_total - b->consumer.ndx);
      }
    }
    else {
      printf("\n \033[1;31m[\033[0m");
      if (raw == b->buffer.raw) {
	print_buffer(raw->data, b->buffer.ndx);
	print_chars('_', raw->size - b->buffer.ndx);
	printf("\033[1;31m]}\033[0m\n""Last buffer: (%zu/%zu, %zu Bytes free space)\n",
	    b->buffer.ndx, raw->size, raw->size - b->buffer.ndx);
	return ;
      }
      else {
	print_buffer(raw->data, raw->size);
      }
    }
    printf("\033[1;31m]\033[0m");
    raw = raw->next;
  }
  printf("\033[1;31m}\033[0m\n");
}

void
rdflDevel_printbufferstate(t_rdfl *obj) {
  if (!obj) return ;
  print_buffers(&obj->data);
  // print_contexts(obj->data.consumer.ctx);
}

#endif
