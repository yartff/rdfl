#ifndef		__RDFL_DEVEL_H_
# define	__RDFL_DEVEL_H_

# ifdef		DEVEL
#  include	"rdfl.h"

void		rdflDevel_printbufferstate(t_rdfl *obj);
void		*rdflDevel_get_func(e_rdflsettings);
const char	*rdflDevel_handler_typedef_declare(void *ptr);
const char	*rdflDevel_handler_func_declare(void *ptr);

# endif		/* !DEVEL */

typedef		enum {
  MODE_DEVEL	= 1 << 0,
  MODE_DEBUG	= 1 << 1,
  MODE_OPTI	= 1 << 2
}		e_buildMode;

#define		RDFLBUILD_ISMODE_DEVEL(x)	(x & (MODE_DEVEL))
#define		RDFLBUILD_ISMODE_DEBUG(x)	(x & (MODE_DEBUG))
#define		RDFLBUILD_ISMODE_OPTI(x)	(x & (MODE_OPTI))

typedef e_buildMode	(*rdflBuild_getMode_t)(void);

#endif		/* !__RDFL_DEVEL_H_ */
