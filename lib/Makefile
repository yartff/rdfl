NAME		=		librdfl_$(HOSTTYPE).so
NOLN_NAME	=		librdfl.so

SRCDIR		=		src/
INCDIR		=		include/

CMD_CC		=		gcc
CMD_RM		=		@rm -rfv

SRC		=		\
				$(SRCDIR)rdfl.c				\
				$(SRCDIR)rdfl_devel.c			\
				$(SRCDIR)rdfl_buffer.c			\
				$(SRCDIR)rdfl_network.c			\
				$(SRCDIR)rdfl_buffer_access.c		\
				$(SRCDIR)rdfl_buffer_iterate.c		\
				$(SRCDIR)rdfl_consumer_type.c		\
				$(SRCDIR)rdfl_context.c			\
				$(SRCDIR)rdfl_readBNF.c			\
				$(SRCDIR)rdfl_applyBNF.c

OBJ		=		$(SRC:.c=.o)

CFLAGS		+=		-Wall -Wextra
CFLAGS		+=		-fPIC -std=gnu99
CFLAGS		+=		-O3
CFLAGS		+=		-finline-functions
CFLAGS		+=		-I$(INCDIR)
CFLAGS		+=		-D_POSIX_C_SOURCE
CFLAGS		+=		-D_POSIX_SOURCE
CFLAGS		+=		-D_BSD_SOURCE
CFLAGS		+=		-D_DEFAULT_SOURCE
## CFLAGS	+=		-g3
# LD_FLAGS	+=		-lrt

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
