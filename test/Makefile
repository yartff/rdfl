TEST		=		tester
LIBDIR		=		../lib/
INCDIR		=		$(LIBDIR)include/

CMD_CC		=		gcc
CMD_RM		=		@rm -rfv

SRC		=		main_test.c				\
				test_1-allavailable_reader.c		\
				test_2-inplace_reader.c			\
				test_3-buffer-elasticity_tester.c	\
				test_4-monitoring_reader.c		\
				test_5-monitoringall_reader.c		\
				test_6-buffer_access.c			\
				test_7-buffer_access_routines.c		\
				test_8-BNFparsing.c

OBJ		=		$(SRC:.c=.o)

LD_FLAGS	+=		-lrdfl
CFLAGS		+=		-Wall -Wextra
CFLAGS		+=		-I$(INCDIR)
CFLAGS		+=		-g3

$(TEST):		$(OBJ)
	$(CMD_CC) $(OBJ) -L$(LIBDIR) $(LD_FLAGS) -o $(TEST)

exec:			$(TEST)
	@env "LD_LIBRARY_PATH=$(LIBDIR)" ./$(TEST) 

valexec:		$(TEST)
	@env "LD_LIBRARY_PATH=$(LIBDIR)" valgrind ./$(TEST) 

all:			$(TEST)

clean:
	$(CMD_RM) $(OBJ)

fclean:			clean
	$(CMD_RM) $(TEST)

re:	fclean all

.PHONY:		all clean fclean re exec valexec
