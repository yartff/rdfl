#ifdef NOPE

public:
├── rdfl_consumer.h  rdfl.h               rdfl_types_buffer.h
└── rdfl_devel.h     rdfl_status_codes.h  rdfl_types.h

rdfl:
└── /* context.h */ clean.h  network.h  values.h
clean.c   devel.c  extract.c  network.c  rdfl.c

bnf:
└── bnf.h
rdfl_bnf.c  rdfl_bnf_dump.c  rdfl_bnf_params.c  rdfl_bnf_run.c

buffer:
└── buffer.h  buffer_access.h  iterate.h
buffer.c         consume.c	   read.c
buffer_access.c  buffer_iterate.c  /* rdfl_context.c */
accessors ───    compare.c	   get_content.c
consumers ───    identifier.c	   match_any.c    read_until.c  string.c

// TODO: buffer.h and buffer.c
// TODO: BACC_* in buffer.h
#endif
