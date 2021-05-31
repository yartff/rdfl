#include	"unit_file.h"

TREEDEF_FUNC(tree_rdfl_consumers_identifier)[] = {
  { .name = "_cb__csm_readIdentifier" },
  { .name = NULL }
};

TREEDEF_FILE(tree_rdfl_consumers)[] = {
  {
    .name = "identifier",
    .functions = TREE_FUNC(tree_rdfl_consumers_identifier)
  }, { .name = NULL }
};

TREEDEF_FUNC(tree_rdfl_accessors_compare)[] = {
  { .name = "rdfl_acc_cmp" },
  { .name = NULL }
};

TREEDEF_FILE(tree_rdfl_accessors)[] = {
  {
    .name = "compare",
    .functions = TREE_FUNC(tree_rdfl_accessors_compare)
  }, { .name = NULL }
};

TREEDEF_DIR(tree_rdfl)[] = {
  {
    .name = "accessors",
    .files = TREE_FILE(tree_rdfl_accessors),
    .dirs = NULL
  },
  {
    .name = "consumers",
    .files = TREE_FILE(tree_rdfl_consumers),
    .dirs = NULL
  }, { .name = NULL }
};


TREEDEF_FUNC(tree_buffer_buffer)[] = {
  {
    .name = "b_buffer_ptr_extend"
  }, { .name = NULL }
};
TREEDEF_FUNC(tree_buffer_iterate)[] = {
  {
    .name = "_iterate_extract"
  }, { .name = NULL }
};

TREEDEF_FILE(tree_buffer)[] = {
  {
    .name = "buffer",
    .functions = TREE_FUNC(tree_buffer_buffer)
  },
  {
    .name = "iterate",
    .functions = TREE_FUNC(tree_buffer_iterate)
  }, { .name = NULL }
};


TREEDEF_FUNC(tree_rdfl_extract)[] = {
  { .name = "rdfl_flush_firstbuffer_alloc" },
  { .name = NULL }
};

TREEDEF_FILE(tree_rdfl)[] = {
  {
    .name = "extract",
    .functions = TREE_FUNC(tree_rdfl_extract)
  }, { .name = NULL }
};

TREEDEF_DIR(tree)[] = {
  {
    .name = "buffer",
    .files = TREE_FILE(tree_buffer),
    .dirs = NULL
  },
  {
    .name = "rdfl",
    .files = TREE_FILE(tree_rdfl),
    .dirs = TREE_DIR(tree_rdfl)
  }, { .name = NULL }
};

TREEDEF_DIR(root) = {
  .name = "tree",
  .files = NULL,
  .dirs = TREE_DIR(tree)
};
