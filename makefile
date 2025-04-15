default:
	gcc dataframe.c main.c wordhash.c utf8_vn.c TFIDF.c sparse_matrix.c checktime.c string_pool.c log_print.c naive_bayes.c \
    -o test -lm \
    -O3 -march=native -flto -funroll-loops -DNDEBUG


