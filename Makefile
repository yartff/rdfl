LIB_DIR		=	lib/
TEST_DIR	=	test/

lib:
	@make -C $(LIB_DIR) --no-print-directory

devel:
	@make -C $(LIB_DIR) D=1 --no-print-directory

debug:
	@make -C $(LIB_DIR) F=1 --no-print-directory

devall:
	@make -C $(LIB_DIR) D=1 F=1 --no-print-directory

valexec:	devall
	@make valexec -C $(TEST_DIR) D=1 F=1 --no-print-directory

exec:	lib
	@make exec -C $(TEST_DIR) --no-print-directory

clean:
	@make fclean -C $(LIB_DIR) --no-print-directory
	@make fclean -C $(TEST_DIR) --no-print-directory

.PHONY:		lib test devel debug devall clean
