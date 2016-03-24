NAME		=		librdfl_$(HOSTTYPE).so
NOLN_NAME	=		librdfl.so

SRCDIR		=		src/
INCDIR		=		include/

CMD_CC		=		gcc
CMD_RM		=		@rm -rfv

SRC		=		\
				$(SRCDIR)rdfl.c				\
				$(SRCDIR)rdfl_buffer.c			\

OBJ		=		$(SRC:.c=.o)

CFLAGS		+=		-Wall -Wextra
CFLAGS		+=		-fPIC -std=c99
CFLAGS		+=		-I$(INCDIR)
CFLAGS		+=		-D_POSIX_C_SOURCE
## CFLAGS		+=		-g3

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
