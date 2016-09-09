#ifndef			__RDFL_LOCAL_H_
# define		__RDFL_LOCAL_H_

# include		"rdfl_status_codes.h"

# define		RDFL_DEFAULT_BUFFSIZE	4096 // (bytes)
# define		RDFL_DEFAULT_TIMEOUT	-1 // (blocking reads)

typedef			enum {
  RDFL_LOC_NONE		=	RDFL_LAST,
  RDFL_LOC_OPEN		=	RDFL_LOC_NONE << 1,
  RDFL_LOC_ALLOC	=	RDFL_LOC_NONE << 2,
  RDFL_LOC_REACHED_EOF	=	RDFL_LOC_NONE << 3,
}			e_rdflsettings_local;

enum {
  BACC_CB_STOP		= 0,
  BACC_CB_CONTINUE,
  BACC_CB_NEEDDATA,
  BACC_CB_NOACTION,
};

#endif			/* !__RDFL_LOCAL_H_ */
