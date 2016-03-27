#ifndef			__RDFL_BUFFER_H_
# define		__RDFL_BUFFER_H_

enum {
  B_ERR_READ		= -1,
  B_ERR_SIZETOOBIG	= -2,
  B_ERR_SELECTERR	= -3
};

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
  size_t		l_total;
  size_t		total;
}			t_rdfl_cm;

// the first shall never be filled again if a next exists
// it is destroyed when empty
typedef			struct {
  t_rdfl_bm		buffer;
  t_rdfl_cm		consumer;
}			t_rdfl_buffer;

// in push
//   if begin == end -> add new
// in pull
//   if begin == end -> delete first
int		rdfl_buffer_init(t_rdfl_buffer *, size_t);
int		rdfl_b_create(t_rdfl_buffer *, size_t);
void		rdfl_buffer_clean(t_rdfl_buffer *);
ssize_t		rdfl_b_push_all_local_monitoring(t_rdfl_buffer *,
    int fd, ssize_t buffersize, long timeout_value);
ssize_t		rdfl_b_push_all_local(t_rdfl_buffer *, int, size_t);
void		*rdfl_b_consume_all(t_rdfl_buffer *b, ssize_t *count_value);
void		rdfl_b_fullclean_if_empty(t_rdfl_buffer *);

#endif			/* !__RDFL_BUFFER_H_ */
