default:
	gcc dataframe.c main.c wordhash.c utf8_vn.c TFIDF.c sparse_matrix.c checktime.c -o test -lm
