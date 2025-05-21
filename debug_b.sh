gcc s_dataframe.c main.c s_wordhash.c s_utf8_vn.c s_TFIDF.c s_sparse_matrix.c s_checktime.c s_string_pool.c s_log_print.c s_naive_bayes.c \
    -o exc/model_debug -lm \
    -lm -g -O0 -fno-inline -fno-omit-frame-pointer -fsanitize=address