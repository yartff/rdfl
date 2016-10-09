// #ifdef		DEVEL

# include	<unistd.h>
# include	<limits.h>
# include	<stdio.h>
# include	<string.h>
# include	<stdlib.h>
# include	"rdfl_buffer.h"
# include	"rdfl_local.h"
# include	"rdfl_context.h"
# include	"rdfl_devel.h"

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

// TODO display size of each buffer
// TODO display contexts
void
rdfl_b_print_buffers(t_rdfl_buffer *b) {
  t_rdfl_b_list		*raw = b->consumer.raw;
  t_ctxstack		*l;
  size_t		tmp;

  if (!raw) {
    printf("\033[0;33mTOT: \033[0m%zu\n" , b->consumer.total);
    return ;
  }
  if (b->consumer.ctx) {
    l = b->consumer.ctx->stack;
    while (l != NULL) {
      printf("_%zd_\n", l->value);
      l = l->next;
    }
  }
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

// RDFL

// Helpers
//
static struct {
  void			*fct;
  const char		*name;
  const char		*func;
  e_rdflsettings	flag;
}	readersTable[] = {
  // Order matters
  {&_read_size, "readsize_handler_t", "_read_size", RDFL_FORCEREADSIZE},
  {&_read_monitoring_no_extend, "readmonitoringnoext_handler_t", "_read_monitoring_no_extend", RDFL_MONITORING | RDFL_NO_EXTEND},
  {&_read_monitoring_allavail, "readmonitoringall_handler_t", "_read_monitoring_allavail", RDFL_MONITORING | RDFL_ALL_AVAILABLE},
  {&_read_monitoring, "readmonitoring_handler_t", "_read_monitoring", RDFL_MONITORING},
  {&_read_noextend, "readnoextend_handler_t", "_read_noextend", RDFL_NO_EXTEND},
  {&_read_all_available, "readall_handler_t", "_read_all_available", RDFL_ALL_AVAILABLE},
  {&_read_legacy, "readlegacy_handler_t", "_read_legacy", RDFL_LEGACY},
  {&_read_singlestep, "read_singlestep_t", "_read_singlestep", RDFL_NONE},
};

const char *
handler_typedef_declare(void *ptr) {
  unsigned int	i = 0;

  while (i < (sizeof(readersTable) / sizeof(*readersTable))) {
    if (ptr == readersTable[i].fct)
      return (readersTable[i].name);
    ++i;
  }
  return (NULL);
}

const char *
handler_func_declare(void *ptr) {
  unsigned int	i = 0;

  while (i < (sizeof(readersTable) / sizeof(*readersTable))) {
    if (ptr == readersTable[i].fct)
      return (readersTable[i].func);
    ++i;
  }
  return (NULL);
}

void *
get_func(e_rdflsettings settings) {
  unsigned int	i = 0;
  while (i < ((sizeof(readersTable) / sizeof(*readersTable)) - 1)) {
    if (RDFL_OPT_CONTAINALL(settings, readersTable[i].flag))
      return (readersTable[i].fct);
    ++i;
  }
  return (&_read_singlestep);
}

void
rdfl_printbufferstate(t_rdfl *obj) {
  rdfl_b_print_buffers(&obj->data);
}

// #endif
