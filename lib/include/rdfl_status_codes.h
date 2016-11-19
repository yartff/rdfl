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
  // RDFL_READTIMEOUT	= 1 << 11,
  RDFL_AUTOREAD		= 1 << 12,
  RDFL_AUTOCLOSE	= 1 << 13,
  // Allows the parsed bnf to use rdfl builtins consumers
  RDFL_ALLOWBUILTINS	= 1 << 14,
  RDFL_CONTEXT		= 1 << 15,
  RDFL_AUTOCLEAR_BLANKS	= 1 << 16,
  // vvv : will come with consumer feature (ran in a different thread)
  // RDFL_CONSUMER_THREAD	= (RDFL_NONE + 1) << 1,
  RDFL_LAST		= 1 << 17,
}			e_rdflsettings;

typedef			enum {
  ERR_NONE		= 0,
  ERR_READ		= -2,
  ERR_MEMORY		= -3,
  ERR_BADF		= -4,
  ERR_NOSPACELEFT	= -5,
  ERR_SELECT		= -6,
  ERR_OPEN		= -7,
  ERR_TIMER		= -8,
  ERR_NOT_IMPLEMENTED	= -9,
  ERR_CONNECTION	= -10,
  ERR_CONNECTION_CLOSED	= -11,

# ifdef			DEVEL
  ERRDEV_OUTOFBOUND	= -30,
  ERRDEV_NULLOBJECT	= -31,
  ERRDEV_BADFLAGS	= -32,
  ERRDEV_BAD_CTXID	= -33,
  ERRDEV_SIZETOOBIG	= -34,
# endif

  ERR_MISC		= -99,
  VCSM			= -100,
  VCSM_INCOMPLETE_TOKEN	= -101,
  VCSM_UNMARKED_TOKEN	= -102,
  VCSM_REACHED_EOF	= -103,
  ERRBNF_SYNTAX		= -200,
  ERRBNF_MISSINGID	= -201,
  VAL			= -400,
  VAL_POTENTIALDATA	= -401,
  VAL_TIMEOUT_REACHED	= -402,
  VAL_LAST_READ_0	= -403,
  // If you get this value, pls fill in buffer manually and try again
  VAL_NEED_DATA		= -404
}			e_rdflerrors;

typedef				enum {
  RDFL_P_NONE			= 0,
  RDFL_P_NULLTERMINATED		= 1 << 0,
  // will assume when a token matches a rule, not any other rules can be applied
  RDFL_P_CONSUME		= 1 << 1,
  RDFL_P_SETCONTEXT		= 1 << 2,
  RDFL_P_IGNORE_PREDATA		= 1 << 3,
  RDFL_P_LAST			= 1 << 4,
}				e_bacc_options;

#endif			/* !__RDFL_STATUSCODES_H_ */
