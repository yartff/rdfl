#ifndef		__RDFL_BUFFER_H_
# define	__RDFL_BUFFER_H_

# define	RDFL_IS_ASCII_PRINTABLECHAR(c) ((c) >= ' ' && (c) <= '~')
# define	RDFL_IS_ASCII_NODISPLAYCHAR(c) (((c) == '\n') || ((c) == '\t'))

typedef	ssize_t		ctxid_t;
typedef struct		s_ctxstack {
  ssize_t		value; // TODO ssize_t?
  ctxid_t		id;
  struct s_ctxstack	*next;
}			t_ctxstack;

typedef struct		{
  size_t		total;
  ctxid_t		id;
  t_ctxstack		*stack;
}			t_rdfl_contexts;

typedef struct		s_rdfl_b_list {
  void			*data;
  size_t		size;
  struct s_rdfl_b_list	*next;
}			t_rdfl_b_list;

typedef struct		{
  t_rdfl_b_list		*raw;
  size_t		ndx;
}			t_rdfl_bm;

typedef struct		{
  t_rdfl_b_list		*raw;
  size_t		ndx;
  size_t		skip;
  size_t		l_total;
  size_t		total;
  t_rdfl_contexts	*ctx;
}			t_rdfl_cm;

typedef struct		{
  t_rdfl_bm		buffer;
  t_rdfl_cm		consumer;
}			t_rdfl_buffer;

// Constructor, Destructor and Cleaner
int		rdfl_buffer_init(t_rdfl_buffer *, size_t);
int		rdfl_b_add(t_rdfl_buffer *, size_t);
void		rdfl_buffer_clean(t_rdfl_buffer *);
void		rdfl_b_fullclean_if_empty(t_rdfl_buffer *);

// Buffer
void		*rdfl_b_buffer_getchunk(t_rdfl_buffer *, size_t *);
void		*rdfl_b_buffer_getchunk_extend(t_rdfl_buffer *, size_t *, size_t);
ssize_t		rdfl_b_push_read(t_rdfl_buffer *, int , void *, size_t);
int		rdfl_b_set_skip(t_rdfl_buffer *, size_t);

// Consumer
void		*rdfl_b_next_chunk(t_rdfl_buffer *, size_t *);
void		rdfl_b_consume_size(t_rdfl_buffer *, size_t);
void		*rdfl_b_consume_firstbuffer_alloc(t_rdfl_buffer *, ssize_t *);
void		*rdfl_b_consume_all_alloc(t_rdfl_buffer *b, ssize_t *count_value);

void		rdfl_b_print_buffers(t_rdfl_buffer *);

#endif			/* !__RDFL_BUFFER_H_ */
