#ifndef		__RDFL_TYPES_BUFFER_H_
# define	__RDFL_TYPES_BUFFER_H_

typedef struct		s_rdfl_blist {
  void			*data;
  size_t		size;
  struct s_rdfl_blist	*next;
}			t_rdfl_blist;

typedef struct		{
  t_rdfl_blist		*raw;
  size_t		ndx;
}			t_rdfl_bm;

typedef struct		{
  t_rdfl_blist		*raw;
  size_t		ndx;
  size_t		skip;
  size_t		l_total;
  size_t		total;
}			t_rdfl_cm;

typedef struct		{
  t_rdfl_bm		buffer;
  t_rdfl_cm		consumer;
}			t_rdfl_buffer;

#endif		/* !__RDFL_TYPES_BUFFER_H_ */
