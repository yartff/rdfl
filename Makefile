##
## LIBRARY
##

LIB_DIR		=	lib/
NAME		=	$(LIB_DIR)librdfl_$(HOSTTYPE).so
NOLN_NAME	=	librdfl.so

SRC_MAIN	=	$(LIB_DIR)rdfl/
SRC_BNF		=	$(LIB_DIR)bnf/
SRC_BC		=	$(LIB_DIR)buffer/
SRC_MAIN_CSM	=	$(SRC_MAIN)consumers/
SRC_MAIN_ACC	=	$(SRC_MAIN)accessors/

INCLUDE		=	$(LIB_DIR)include/
INCLUDE_MAIN	=	$(SRC_MAIN)include/
INCLUDE_BC	=	$(SRC_BC)include/
INCLUDE_BNF	=	$(SRC_BNF)include/
INCLUDES_FLAGS	=	-I$(INCLUDE) -I$(INCLUDE_MAIN) -I$(INCLUDE_BC) -I$(INCLUDE_BNF)

OBJ		=	$(SRC:.c=.o)

SRC		=	$(SRC_MAIN)rdfl.c			\
			$(SRC_MAIN)rdfl_readers.c		\
			$(SRC_MAIN)rdfl_utils.c			\
			$(SRC_MAIN)rdfl_infos.c			\
			$(SRC_MAIN)clean.c			\
			$(SRC_MAIN)network.c			\
			$(SRC_MAIN)extract.c			\
			$(SRC_MAIN)devel.c

SRC		+=	$(SRC_MAIN_CSM)identifier.c		\
			$(SRC_MAIN_CSM)string.c			\
			$(SRC_MAIN_CSM)match_any.c		\
			$(SRC_MAIN_CSM)read_until.c

SRC		+=	$(SRC_MAIN_ACC)info.c			\
			$(SRC_MAIN_ACC)compare.c		\
			$(SRC_MAIN_ACC)get_content.c		\
			$(SRC_MAIN_ACC)index.c

SRC		+=	$(SRC_BC)buffer.c			\
			$(SRC_BC)manage.c			\
			$(SRC_BC)consume.c			\
			$(SRC_BC)read.c				\
			$(SRC_BC)iterate.c

SRC		+=	$(SRC_BNF)rdfl_bnf.c			\
			$(SRC_BNF)rdfl_bnf_params.c		\
			$(SRC_BNF)rdfl_bnf_run.c		\
			$(SRC_BNF)rdfl_bnf_dump.c

CFLAGS		+=	-Wall -Wextra
CFLAGS		+=	-fPIC -std=gnu99
CFLAGS		+=	-finline-functions
CFLAGS		+=	$(INCLUDES_FLAGS)
CFLAGS		+=	-D_POSIX_C_SOURCE
CFLAGS		+=	-D_POSIX_SOURCE
CFLAGS		+=	-D_BSD_SOURCE
CFLAGS		+=	-D_DEFAULT_SOURCE

$(NOLN_NAME):		$(OBJ)
	@echo Linking: $(CMD_CC) '{}' -shared -o $(NAME)
	@$(CMD_CC) $(OBJ) -shared -o $(NAME)
	@ln -fs $(NAME) $(NOLN_NAME)

## all:			CFLAGS += -O3 -DOPTI
all:			$(NOLN_NAME)

ifeq ($(O),1)
  CFLAGS += -O3 -DOPTI
endif

clean:
	$(CMD_RM) $(OBJ)

devel:			CFLAGS += -DDEVEL
devel:			clean $(NOLN_NAME)

debug:			CFLAGS += -g3 -DDEBUG
debug:			clean $(NOLN_NAME)

both:			CFLAGS += -DDEVEL -g3 -DDEBUG
both:			clean $(NOLN_NAME)

re:			fclean all

##
## UNIT TESTS
##

UNIT_DIR	=	unit_tests/
UNIT_TESTS_DIR	=	$(UNIT_DIR)testers.d/
UNIT_TREE	=	$(UNIT_TESTS_DIR)tree/
UNIT_NAME	=	unit

UNIT_OBJ	=	$(UNIT_SRC:.c=.o)

UNIT_SRC	=	$(UNIT_DIR)unit.c			\
			$(UNIT_DIR)unit_devel.c			\
			$(UNIT_DIR)opt.c			\
			$(UNIT_DIR)lib.c			\
			$(UNIT_DIR)public.c			\
			$(UNIT_TESTS_DIR)file.c#			\
			\
			$(UNIT_TREE)buffer/buffer.d/b_buffer_ptr_extend.c

INCLUDES_UFLAGS	=	-I$(UNIT_DIR)include/ -I$(UNIT_TREE) -I$(LIB_DIR)

UNIT_LD_FLAGS	=	-ldl

$(UNIT_NAME):		CFLAGS	=	-g3
$(UNIT_NAME):		CFLAGS	=	-DDEVEL
$(UNIT_NAME):		CFLAGS	+=	$(INCLUDES_FLAGS)
$(UNIT_NAME):		CFLAGS	+=	$(INCLUDES_UFLAGS)
$(UNIT_NAME):		CFLAGS	+=	-D_GNU_SOURCE
$(UNIT_NAME):		$(UNIT_OBJ)
	echo Linking: $(CMD_CC) '{}' -o $(UNIT_NAME) $(UNIT_LD_FLAGS)
	$(CMD_CC) $(UNIT_OBJ) -o $(UNIT_NAME) $(UNIT_LD_FLAGS)

uclean:
	$(CMD_RM) $(UNIT_NAME) $(UNIT_OBJ)

%.o:			%.c
	@echo -e [...] - $*.c
	@$(CMD_CC) -c $(CFLAGS) $*.c -o $*.o

details:
	@echo "Command line: "\`"$(CMD_CC) -c $(CFLAGS) {.c} -o {.o}"\`"."

fclean:			uclean clean
	$(CMD_RM) $(NAME) $(NOLN_NAME)

CMD_CC		=	gcc
CMD_RM		=	@rm -rfv

.PHONY:	all clean fclean re details
