#include	<stdio.h>
#include	<stdlib.h>
#include	"unit_internal.h"

void
log_internal_type(LOGP, const char *ffrom, const char *file, int line, const char *reason) {
  switch(_logt) {
    case LOGT_FATAL:
      fprintf(stderr, "[FATAL][%s] %s\n", ffrom, reason);
      exit(EXIT_FAILURE);
      break ;
    case LOGT_LOG:
      fprintf(stderr, "[  LOG][%s] %s\n", ffrom, reason);
      break ;
  }
}

/*
void
log_internal_success()
  // -log=
*/

void
log_internal_warn(const char *ffrom, const char *file, int line, const char *reason) {
  fprintf(stderr, "[ WARN][%s (%s:%d)] %s\n", ffrom, file, line, reason);
}
