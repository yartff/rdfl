#ifndef			__RDFL_LOCAL_H_
# define		__RDFL_LOCAL_H_

# include		"rdfl.h"

# define		RDFL_DEFAULT_BUFFSIZE	4096 // (bytes) // TODO pagesize() ?
# define		RDFL_DEFAULT_TIMEOUT	-1 // (blocking reads)

typedef			enum {
  RDFL_LOC_NONE		=	RDFL_LAST,
  RDFL_LOC_OPEN		=	RDFL_LOC_NONE << 1,
  RDFL_LOC_ALLOC	=	RDFL_LOC_NONE << 2,
}			e_rdflsettings_local;

#endif			/* !__RDFL_LOCAL_H_ */
