#ifndef			__RDFL__VALUES_H_
# define		__RDFL__VALUES_H_

# include		"rdfl_status_codes.h"

# define		RDFL_DEFAULT_BUFFSIZE	4096 // (bytes)
# define		RDFL_DEFAULT_TIMEOUT	-1 // (blocking reads)

typedef			enum {
  LOCAL_NONE		=	RDFL_LAST,
  LOCAL_OPEN		=	LOCAL_NONE << 1,
  LOCAL_ALLOC		=	LOCAL_NONE << 2,
  LOCAL_REACHED_EOF	=	LOCAL_NONE << 3,
}			e_rdflsettings_local;

#endif			/* !__RDFL__VALUES_H_ */
