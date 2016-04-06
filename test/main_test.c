#include	<stdlib.h>
#include	<stdio.h>
#include	"tester.h"

int	(*test_functions[])(void) = {
  &test_1,
  &test_2,
};

// TODO test_4-modeswitching_tester.c
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
