#ifndef			__RDFL_STATUSCODES_H_
# define		__RDFL_STATUSCODES_H_

typedef			enum {
  RDFL_NONE		= 0,
  RDFL_LEGACY		= 1 << 0,
  RDFL_FORCEREADSIZE	= 1 << 1,
  RDFL_ALL_AVAILABLE	= 1 << 2,
  RDFL_MONITORING	= 1 << 3,
  RDFL_NO_EXTEND	= 1 << 4,
  RDFL_ADJUST_BUFFSIZE	= 1 << 5,
  RDFL_FILLFREESPACE	= 1 << 6,
  RDFL_THREADSAFE	= 1 << 7,
  // RDFL_KEEPTRACK	= 1 << 8, Keep line/col infos
  // RDFL_SIGMASK	= 1 << 9, // Use pselect instead
  RDFL_FULLEMPTY	= 1 << 10,
  // RDFL_READTIMEOUT	= 1 << 11
  RDFL_LAST		= 1 << 12,
  // vvv : will come with consumer feature (ran in a different thread)
  // RDFL_CONSUMER_THREAD	= (RDFL_NONE + 1) << 1,
}			e_rdflsettings;

typedef			enum {
  ERR_NONE		= 0,
  ERR_SIZETOOBIG	= -1,
  ERR_READ		= -2,
  ERR_MEMORY		= -3,
  ERR_BADF		= -4,
  ERR_BADFLAGS		= -5,
  ERR_NOSPACELEFT	= -6,
  ERR_SELECT		= -7,
  ERR_OPEN		= -8,
  ERR_TIMER		= -9,
  ERR_NOT_IMPLEMENTED	= -10,
  ERR_CONNECTION	= -11,
  ERR_CONNECTION_CLOSED	= -12,
  VAL			= -13,
  VAL_POTENTIALDATA	= -14,
  VAL_TIMEOUT_REACHED	= -15,
  VAL_LAST_READ_0	= -16
}			e_rdflerrors;

#endif			/* !__RDFL_STATUSCODES_H_ */
