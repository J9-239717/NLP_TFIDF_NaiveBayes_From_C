test: main.c dataframe.c
	gcc dataframe.c main.c -o test -fsanitize=address
