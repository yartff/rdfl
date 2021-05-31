#ifndef		__UNIT_LOG_H_
# define	__UNIT_LOG_H_

typedef enum	{
  LOGT_MUTE		= 0,
  LOGT_LOG,
  LOGT_WARN,
  LOGT_FATAL
}		e_unit_log_type;

void		log_internal_type(e_unit_log_type, const char *, const char *, int, const char *);
void		log_internal_error(const char *, const char *, int, const char *);

#define	LOGP			e_unit_log_type _logt
#define LOG_T(reason)		do { \
  log_internal_type(_logt, __FUNCTION__, __FILE__, __LINE__, reason); \
} while (0)

#define LOG_WARN(reason)	do { \
  log_internal_warn(__FUNCTION__, __FILE__, __LINE__, reason); \
} while (0)

#endif		/* !__UNIT_LOG_H_ */
