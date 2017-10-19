NAME		=		librdfl_$(HOSTTYPE).so
NOLN_NAME	=		librdfl.so

SRCDIR		=		src/
INCDIR		=		include/

CMD_CC		=		gcc
CMD_RM		=		@rm -rfv

SRC_BNF		=		$(SRCDIR)bnf/
SRC_BC		=		$(SRCDIR)buffcons/
SRC_BC_CONS	=		$(SRC_BC)consumers/
SRC_BC_BACC	=		$(SRC_BC)accessors/

SRC		=		\
				$(SRCDIR)rdfl.c				\
				$(SRCDIR)rdfl_network.c			\
				$(SRCDIR)rdfl_devel.c			\
				\
				$(SRC_BC)rdfl_buffer.c			\
				$(SRC_BC)rdfl_buffer_access.c		\
				$(SRC_BC)rdfl_buffer_iterate.c		\
				$(SRC_BC)rdfl_context.c			\
				\
				$(SRC_BC_CONS)bnf_identifier.c		\
				$(SRC_BC_CONS)bnf_string.c		\
				$(SRC_BC_CONS)match_any.c		\
				$(SRC_BC_CONS)read_until.c		\
				\
				$(SRC_BC_BACC)compare.c			\
				$(SRC_BC_BACC)get_content.c		\
				\
				$(SRC_BNF)rdfl_bnf.c			\
				$(SRC_BNF)rdfl_bnf_params.c		\
				$(SRC_BNF)rdfl_bnf_run.c		\
				$(SRC_BNF)rdfl_bnf_dump.c

OBJ		=		$(SRC:.c=.o)

CFLAGS		+=		-Wall -Wextra
CFLAGS		+=		-fPIC -std=gnu99
CFLAGS		+=		-finline-functions
CFLAGS		+=		-I$(INCDIR)
CFLAGS		+=		-D_POSIX_C_SOURCE
CFLAGS		+=		-D_POSIX_SOURCE
CFLAGS		+=		-D_BSD_SOURCE
CFLAGS		+=		-D_DEFAULT_SOURCE

ifeq	($(D),1)
  CFLAGS	+=		-DDEVEL
endif
ifeq	($(F),1)
  CFLAGS	+=		-g3
else
  CFLAGS	+=		-O3
endif

COLOR_BLUE	=	"\033[1;34m"
COLOR_RED	=	"\033[1;31m"
COLOR_GREEN	=	"\033[1;32m"
COLOR_YELLOW	=	"\033[0;33m"
COLOR_NORMAL	=	"\033[0m"

%.o:			%.c
	@echo -e [...] - $*.c
	@$(CC) -c $(CFLAGS) $*.c -o $*.o

$(NOLN_NAME):		$(OBJ)
	$(CMD_CC) $(OBJ) -shared -o $(NAME) $(LD_FLAGS)
	ln -fs $(NAME) $(NOLN_NAME)

all:			details $(NOLN_NAME)

details:
	@echo "Command line: "\""$(CC) $(CFLAGS) {}"\""."

clean:
	$(CMD_RM) $(OBJ)

fclean:			clean
	$(CMD_RM) $(NAME) $(NOLN_NAME)

re:	fclean all

.PHONY:	all clean fclean re details
