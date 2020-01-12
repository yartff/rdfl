#ifndef			__BUFFER_H_
# define		__BUFFER_H_

# include		"rdfl_types.h"

// TODO: typedef and change return type in consumers
typedef enum {
  BACC_CB_STOP		= 0,
  BACC_CB_CONTINUE,
  BACC_CB_NEEDDATA,
  BACC_CB_NOACTION,
}		e_next_action;

// Constructor, Destructor and Cleaner
int		b_init(t_rdfl_buffer *b, size_t amount);
void		b_fullclean_if_empty(t_rdfl_buffer *b);
rdfl_retval	b_clean(t_rdfl_buffer *b);

// Chunk Getters
void		*b_buffer_ptr_extend(t_rdfl_buffer *b, size_t *s, size_t amount);
void		*b_consumer_ptr(t_rdfl_buffer *b, size_t *s);
void		*b_buffer_ptr(t_rdfl_buffer *b, size_t *s);

// Consumer
void		*b_consume_all_alloc(t_rdfl_buffer *b, ssize_t *count_value);
void		*b_consume_firstbuffer_alloc(t_rdfl_buffer *b, ssize_t *count_value);
void		b_consume_size(t_rdfl_buffer *b, size_t value);

// Read wrappers utils
ssize_t		push_read(t_rdfl *obj, void *ptr, size_t available);
ssize_t		read_into_chunk_extend(void *obj);
ssize_t		read_into_chunk(void *obj);

// Manage
int		b_set_skip(t_rdfl_buffer *b, size_t value);

//
// local
void		leave_unique_consumer_blist(t_rdfl_buffer *b); // local

#endif			/* !__BUFFER_H_ */
