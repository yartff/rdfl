NAME		=		librdfl_$(HOSTTYPE).so
NOLN_NAME	=		librdfl.so

SRCDIR		=		src/
INCDIR		=		include/

CMD_CC		=		gcc
CMD_RM		=		@rm -rfv

SRC_BNF		=		$(SRCDIR)bnf/
SRC_BC		=		$(SRCDIR)buffcons/
SRC_RDFL	=		$(SRCDIR)reader/

SRC		=		\
				$(SRC_RDFL)rdfl.c			\
				$(SRC_RDFL)rdfl_network.c		\
				$(SRC_RDFL)rdfl_devel.c			\
				\
				$(SRC_BC)rdfl_buffer.c			\
				$(SRC_BC)rdfl_buffer_access.c		\
				$(SRC_BC)rdfl_buffer_iterate.c		\
				$(SRC_BC)rdfl_consumer_type.c		\
				$(SRC_BC)rdfl_context.c			\
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

$(NOLN_NAME):		$(OBJ)
	$(CMD_CC) $(OBJ) -shared -o $(NAME) $(LD_FLAGS)
	ln -fs $(NAME) $(NOLN_NAME)

all:			$(NOLN_NAME)

clean:
	$(CMD_RM) $(OBJ)

fclean:			clean
	$(CMD_RM) $(NAME) $(NOLN_NAME)

re:	fclean all

.PHONY:	all clean fclean re
