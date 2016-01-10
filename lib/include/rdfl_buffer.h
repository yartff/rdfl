#ifndef			__RDFL_BUFFER_H_
# define		__RDFL_BUFFER_H_

typedef struct		s_rdfl_b_list {
  void			*data;
  size_t		size;
  int			nb_used;
  struct s_rdfl_b_list	*next;
}			t_rdfl_b_list;

typedef			struct {
  t_rdfl_b_list		*first;
  t_rdfl_b_list		*last;
  size_t		total;
}			t_rdfl_b;

typedef	struct		s_rdfl_reader {
  void			*chunk;
  size_t		size;
  struct s_rdfl_reader	*next;
}			t_rdfl_reader; // circular

typedef			struct {
  t_rdfl_b		raw;
  t_rdfl_reader		*consummer;
}			t_rdfl_buffer;

void		rdfl_buffer_init(t_rdfl_buffer *, size_t);
int		rdfl_b_create(t_rdfl_buffer *, size_t);

#endif			/* !__RDFL_BUFFER_H_ */
