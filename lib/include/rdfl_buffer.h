#ifndef			__RDFL_BUFFER_H_
# define		__RDFL_BUFFER_H_

typedef struct		s_rdfl_b_list {
  void			*data;
  size_t		size;
  size_t		begin;
  size_t		end;
  struct s_rdfl_b_list	*next;
}			t_rdfl_b_list;

typedef			struct {
  t_rdfl_b_list		*first;
  t_rdfl_b_list		*consummer;
  size_t		total;
}			t_rdfl_buffer;

void		rdfl_buffer_init(t_rdfl_buffer *, size_t);
int		rdfl_b_create(t_rdfl_buffer *, size_t);

#endif			/* !__RDFL_BUFFER_H_ */
