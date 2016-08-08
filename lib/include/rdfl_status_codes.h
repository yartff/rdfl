#ifndef			__RDFL_STATUSCODES_H_
# define		__RDFL_STATUSCODES_H_

typedef			enum {
  RDFL_NONE		= 0,
  RDFL_LEGACY		= (RDFL_NONE + 1) << 1,
  RDFL_FORCEREADSIZE	= (RDFL_NONE + 1) << 2,
  RDFL_ALL_AVAILABLE	= (RDFL_NONE + 1) << 3,
  RDFL_MONITORING	= (RDFL_NONE + 1) << 4,
  RDFL_NO_EXTEND	= (RDFL_NONE + 1) << 5,
  RDFL_ADJUST_BUFFSIZE	= (RDFL_NONE + 1) << 6,
  // vvv : will come with consumer feature (ran in a different thread)
  // RDFL_CONSUMER_THREAD	= (RDFL_NONE + 1) << 1,
  RDFL_FILLFREESPACE	= (RDFL_NONE + 1) << 7,
  RDFL_THREADSAFE	= (RDFL_NONE + 1) << 8,
  // RDFL_KEEPTRACK	= (RDFL_NONE + 1) << 9, Keep line/col infos
  // RDFL_SIGMASK	= (RDFL_NONE + 1) << 10, // Use pselect instead
  RDFL_FULLEMPTY	= (RDFL_NONE + 1) << 11,
  // RDFL_READTIMEOUT	= (RDFL_NONE + 1) << 12
  RDFL_LAST		= (RDFL_NONE + 1) << 13,
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
