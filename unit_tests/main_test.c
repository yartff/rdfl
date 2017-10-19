#include	<stdlib.h>
#include	<stdio.h>
#include	"tester.h"

int	(*test_functions[])(void) = {
  &test_0__devel,
  &test_1__allavailable_reader,
  &test_2__inplace_reader,
  &test_3__bufferelasticity_tester,
  &test_4__monitoring_reader,
  &test_5__monitoringall_reader,
  &test_6__buffer_access,
  &test_7__buffer_access_routines
  /*
  &test_8__BNFparsing
  */
};

// TODO test_5-errorcases_tester.c
int
main(void) {
  unsigned int	i = 0;

  while (i < (sizeof(test_functions) / sizeof(*test_functions))) {
    if (test_functions[i]() == EXIT_FAILURE) {
      printf("%u: Failure\n", i + 1);
    }
    ++i;
  }
  return (0);
}
