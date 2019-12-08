#include	<stdlib.h>
#include	<stdio.h>
#include	<dlfcn.h>
#include	"unit.h"
#include	"lib.h"

static void	*handle = NULL;

int
load_rdfl(void) {
  clean_rdfl();
  handle = dlopen ("./librdfl.so", RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

void *
load_rdfl_function(const char *fname, int exit_on_failure) {
  void	*ret;

  if (!handle) {
    fprintf(stderr, "Lib not loaded\n");
    return (NULL);
  }
  if (!(ret = dlsym(handle, fname))) {
    fprintf(stderr, "%s\n", dlerror());
    if (exit_on_failure)
      exit(EXIT_FAILURE);
  }
  return (ret);
}

void
clean_rdfl(void) {
  if (!handle)
    return ;
  dlclose(handle);
  handle = NULL;
}
