#ifndef			__BUFFER_READ_H_
# define		__BUFFER_READ_H_

# include		"data_container.h"
# include		"rdfl_buffer.h"

ssize_t			b_push_read(t_rdfl_buffer *b, int fd, void *ptr, size_t s);
ssize_t			push_read(t_rdfl *obj, void *ptr, size_t available);
ssize_t			read_into_chunk_extend(void *obj);
ssize_t			read_into_chunk(void *obj);

#endif			/* !__BUFFER_READ_H_ */
