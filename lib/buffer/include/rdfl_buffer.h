#ifndef			__RDFL_BUFFER_H_
# define		__RDFL_BUFFER_H_

# include		<unistd.h>
# include		"rdfl_status_codes.h"

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
}			t_rdfl_cm;

typedef struct		{
  t_rdfl_bm		buffer;
  t_rdfl_cm		consumer;
}			t_rdfl_buffer;

// TODO: typedef and change return type in consumers
enum {
  BACC_CB_STOP		= 0,
  BACC_CB_CONTINUE,
  BACC_CB_NEEDDATA,
  BACC_CB_NOACTION,
};

// Constructor, Destructor and Cleaner
int			b_init(t_rdfl_buffer *, size_t);
rdfl_retval		b_clean(t_rdfl_buffer *);
void			b_fullclean_if_empty(t_rdfl_buffer *);

// Buffer
void			*b_buffer_getchunk(t_rdfl_buffer *, size_t *);
void			*b_buffer_getchunk_extend(t_rdfl_buffer *, size_t *, size_t);
ssize_t			b_push_read(t_rdfl_buffer *, int , void *, size_t);
int			b_set_skip(t_rdfl_buffer *, size_t);

// Consumer
void			b_consume_size(t_rdfl_buffer *, size_t);
void			*b_consume_firstbuffer_alloc(t_rdfl_buffer *, ssize_t *);
void			*b_consume_all_alloc(t_rdfl_buffer *b, ssize_t *count_value);

void			*b_consumer_ptr(t_rdfl_buffer *, size_t *);

#endif			/* !__RDFL_BUFFER_H_ */
