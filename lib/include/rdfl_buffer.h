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
  size_t		end;
}			t_rdfl_cm;

// the first shall never be filled again if a next exists
// it is destroyed when empty
typedef			struct {
  t_rdfl_bm		buffer;
  t_rdfl_cm		consummer;
  size_t		total;
}			t_rdfl_buffer;

// in push
//   if begin == end -> add new
// in pull
//   if begin == end -> delete first
int		rdfl_buffer_init(t_rdfl_buffer *, size_t);
int		rdfl_b_create(t_rdfl_buffer *, size_t);

#endif			/* !__RDFL_BUFFER_H_ */
