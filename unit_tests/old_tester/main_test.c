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
    // TODO test_5-errorcases_tester.c
    */
};

static void print_charlist(void *bytes, size_t from, size_t to) {
  // printf("=> print_charlist(\"%.5s\", %zu, %zu, %zu);\n", (char *)bytes + from, from, to, total);
  printf("{");
  if (from != to)
    printf("%d", ((char *)bytes)[from]);
  while (++from < to) {
    printf(", %d", ((char *)bytes)[from]);
  }
  printf("}");
}

static void outputDeclConsumer(t_rdfl_cm *cm, size_t nb) {
  (void)nb;
  { // consumer
    printf("  .consumer = {\n"
	"    .raw = NULL,\n"
	"    .ndx = %zu,\n"
	"    .skip = %zu,\n"
	"    .l_total = %zu,\n"
	"    .total = %zu,\n"
	"    .ctx = NULL\n"
	"  },\n", cm->ndx, cm->skip, cm->l_total, cm->total);
  }
}

static void outputDecl(t_rdfl_buffer *r, const char *varname) {
  { // t_rdfl_b_list
    t_rdfl_b_list	*b = r->consumer.raw;
    size_t		nb = 0, maxsize = 0;
    while (b) { if (maxsize < b->size) maxsize = b->size; ++nb; b = b->next; }

    {
      printf("unsigned char %s__d[][%ld] = {\n", varname, maxsize);
      b = r->consumer.raw;
      maxsize = r->consumer.l_total;
      print_charlist(b->data, 0, r->consumer.ndx + r->consumer.l_total);
      b = b->next;
      while (b) {
	printf(",\n");
	print_charlist(b->data, 0, b->next ? b->size : r->consumer.total - maxsize);
	maxsize += b->size;
	b = b->next;
      }
      printf("\n};\n");
    }
    {
      printf("t_rdfl_b_list %s__b[] = {\n", varname);
      b = r->consumer.raw;
      nb = 0;
      while (b) {
	if (b != r->consumer.raw) printf(",\n");
	if (b->next)
	  printf("  { .data = %s__d[%ld], .size = %zu, .next = &%s__b[%zu] }", varname, nb, b->size, varname, nb + 1);
	else
	  printf("  { .data = %s__d[%ld], .size = %zu, .next = NULL }", varname, nb, b->size);
	b = b->next;
	++nb;
      }
      printf("\n};\n");
    }
  }

  /*
     { // t_rdfl_buffer
     printf("%s\t%s = {\n", "t_rdfl_buffer", varname);
     { // buffer
     printf("  .buffer = {\n"
     "    .raw = NULL,\n"
     "    .ndx = %zu,\n"
     "  }\n", r->buffer.ndx);
     }
     printf("};\n");
     }
     */
}

void tmp_buff(t_rdfl_buffer *b) {
  printf("====>\nbuffer {\n  ndx: %zu,\n}", b->buffer.ndx);
  printf(" consumer {\n  ndx: %zu,\n  skip: %zu,\n  l_total: %zu,\n  total: %zu\n}\n",
      b->consumer.ndx, b->consumer.skip, b->consumer.l_total, b->consumer.total);
  rdfl_b_print_buffers(b);
  printf("<====\n");
}

int
main(void) {
  t_rdfl		example;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 100);
  rdfl_load_path(&example, EXINPUT_FILES_PATH("text/file"), 0, NULL);
  // printf("%d\n", rdflDevel_get_func(0) == &rdfl_read_singlestep);
  rdfl_read_singlestep(&example);
  rdfl_read_singlestep(&example);
  rdfl_read_singlestep(&example);
  rdfl_b_consume_size(&example.data, 40);

  /*
  rdflDevel_printbufferstate(&example);
  outputDecl(&example.data, "rdbuff");
  */

  unsigned char rdbuff__d[][100] = {
    {76, 111, 114, 101, 109, 32, 105, 112, 115, 117, 109, 32, 100, 111, 108, 111, 114, 32, 115, 105, 116, 32, 97, 109, 101, 116, 44, 32, 99, 111, 110, 115, 101, 99, 116, 101, 116, 117, 114, 32, 97, 100, 105, 112, 105, 115, 99, 105, 110, 103, 32, 101, 108, 105, 116, 46, 32, 67, 114, 97, 115, 32, 117, 108, 116, 114, 105, 99, 105, 101, 115, 32, 111, 114, 99, 105, 32, 97, 116, 32, 101, 114, 111, 115, 32, 108, 97, 99, 105, 110, 105, 97, 32, 102, 114, 105, 110, 103, 105, 108},
    {108, 97, 46, 32, 68, 117, 105, 115, 32, 101, 103, 101, 116, 32, 115, 117, 115, 99, 105, 112, 105, 116, 32, 117, 114, 110, 97, 46, 32, 80, 114, 97, 101, 115, 101, 110, 116, 32, 110, 101, 99, 32, 102, 101, 117, 103, 105, 97, 116, 32, 116, 117, 114, 112, 105, 115, 46, 32, 86, 101, 115, 116, 105, 98, 117, 108, 117, 109, 32, 116, 105, 110, 99, 105, 100, 117, 110, 116, 32, 97, 99, 32, 100, 111, 108, 111, 114, 32, 97, 116, 32, 100, 105, 103, 110, 105, 115, 115, 105, 109},
    {46, 32, 73, 110, 116, 101, 103, 101, 114, 32, 101, 117, 32, 97, 114, 99, 117, 32, 114, 105, 115, 117, 115, 46, 32, 67, 117, 114, 97, 98, 105, 116, 117, 114, 32, 101, 103, 101, 116, 32, 108, 111, 114, 101, 109, 32, 118, 101, 108, 32, 105, 112, 115, 117, 109, 32, 112, 108, 97, 99, 101, 114, 97, 116, 32, 102, 101, 117, 103, 105, 97, 116, 32, 105, 100, 32, 97, 99, 32, 110, 117, 110, 99, 46, 32, 83, 101, 100, 32, 113, 117, 105, 115, 32, 109, 97, 120, 105, 109, 117}
  };
  t_rdfl_b_list rdbuff__b[] = {
    { .data = rdbuff__d[0], .size = 100, .next = &rdbuff__b[1] },
    { .data = rdbuff__d[1], .size = 100, .next = &rdbuff__b[2] },
    { .data = rdbuff__d[2], .size = 100, .next = NULL }
  };


  // HERE
  t_rdfl_buffer rdbuff = {
    .consumer = {
      .raw = &rdbuff__b[0],
      .ndx = example.data.consumer.ndx,
      .skip = example.data.consumer.skip,
      .l_total = example.data.consumer.l_total,
      .total = example.data.consumer.total,
      .ctx = NULL
    },
    .buffer = {
      .raw = NULL,
      .ndx = 0
    }
  };

  rdfl_b_print_buffers(&example.data);
  rdfl_b_print_buffers(&rdbuff);

#if 0
      unsigned int	i = 0;

      while (i < (sizeof(test_functions) / sizeof(*test_functions))) {
      if (test_functions[i]() == EXIT_FAILURE) {
      printf("%u: Failure\n", i + 1);
      }
      ++i;
      }
      return (0);
#endif
      return (0);
      }

/*
 * UserSide Tests
 * -> assert t_rdfl between each calls
 *
 * test each function separately
 */
// foreach size_file {
//   foreach _readers {
//     test options()
//     test consumers
//   }
// }
