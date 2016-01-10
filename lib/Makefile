NAME		=		librdfl.so
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

$(NAME):		$(OBJ)
	$(CMD_CC) $(OBJ) -shared -o $(NAME) $(LD_FLAGS)

all:			$(NAME)

clean:
	$(CMD_RM) $(OBJ)

fclean:			clean
	$(CMD_RM) $(NAME)

re:	fclean all

.PHONY:	all clean fclean re
