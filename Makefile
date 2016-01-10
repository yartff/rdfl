LIB_DIR		=	lib/
TEST_DIR	=	test/

lib:
	@make -C $(LIB_DIR) --no-print-directory

test:	lib
	@make test -C $(TEST_DIR) --no-print-directory

retest:
	@make re -C $(TEST_DIR) --no-print-directory

relib:
	@make re -C $(LIB_DIR) --no-print-directory

exretest:	lib
	@make retest -C $(TEST_DIR) --no-print-directory

debug:		lib
	@make re debug -C $(TEST_DIR) --no-print-directory

clean:
	@make fclean -C $(LIB_DIR) --no-print-directory
	@make fclean -C $(TEST_DIR) --no-print-directory

.PHONY:		lib test
