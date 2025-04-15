#include "naive_bayes.h"

Naive_Bayes_OJ* createNaive_Bayes(data_frame* df, word_hash* hash) {
    Naive_Bayes_OJ* nb = (Naive_Bayes_OJ*)malloc(sizeof(Naive_Bayes_OJ));
    checkExistMemory(nb);
    nb->num_classes = df->size_label;
    nb->vocab_size = hash->size;
    nb->hash = hash;
    nb->likelihood = (float*)malloc(sizeof(float) * nb->num_classes * nb->vocab_size);

    for(int i = 0; i < nb->num_classes * nb->vocab_size; i++) {
        nb->likelihood[i] = (float)ALPHA; // Initialize with 1.0f for Laplace smoothing
    }

    checkExistMemory(nb->likelihood);
    memset(nb->prior, 0, sizeof(nb->prior));

    // Initialize prior probabilities each class P(C)
    label_frequency* label_freq = df->label_freq;
    while(label_freq) {
        if(!isdigit(label_freq->label[0])){
            error_printf("Label index1:%c-%s is not valide is should be number\n",label_freq->label[0] ,label_freq->label);
            label_freq = label_freq->next;
            continue;
        }
        int class_index = atoi(label_freq->label);
        if(class_index < 0 || class_index >= nb->num_classes){
            error_printf("Class index %d is out of bounds\n", class_index);
            label_freq = label_freq->next;
            continue;
        }

        // Calculate prior probability P(C) = frequency of class C / total number of samples
        if(DEBUG){
            debug_printf("Class %d: Frequency: %d and Size is %d\n", class_index, label_freq->frequency, df->size);
            debug_printf("If is float: %f\n", (float)label_freq->frequency /(float) df->size);
            debug_printf("If is int: %d\n", label_freq->frequency / df->size);
        }
        nb->prior[class_index] = (float)label_freq->frequency /(float) df->size;

        label_freq = label_freq->next;
    }
    return nb;
}

void freeNaive_Bayes(Naive_Bayes_OJ* nb) {
    if (nb) {
        free(nb->likelihood);
        free(nb);
    }
}

void printNaive_Bayes(Naive_Bayes_OJ* nb) {
    info_printf("Naive Bayes Classifier:\n");
    info_printf("Number of classes: %d\n", nb->num_classes);
    info_printf("Vocabulary size: %d\n", nb->vocab_size);
    info_printf("Prior probabilities:\n");
    for (int i = 0; i < nb->num_classes; i++) {
        if(!nb->prior[i])continue;
        info_printf("Class %d: %f\n", i, nb->prior[i]);
    }
    // info_printf("Likelihood probabilities:\n");
    // for (int i = 0; i < nb->num_classes; i++) {
    //     for (int j = 0; j < nb->vocab_size; j++) {
    //         info_printf("Class %d, Word %d: %f\n", i, j, nb->likelihood[index(i, j, nb->vocab_size)]);
    //     }
    // }
}

int getlikelihood_to_file(Naive_Bayes_OJ* nb, const char* filename) {
    if (!nb || !filename) {
        error_printf("Naive Bayes or filename is NULL\n");
        return -1;
    }
    FILE* file = fopen(filename, "w");
    if (!file) {
        error_printf("Cannot open file %s\n", filename);
        return -1;
    }

    int vocab_size = nb->vocab_size;
    word_hash* vocab = nb->hash;

    fprintf(file, "%-25s", "Word");  // หัวตาราง
    for (int i = 0; i < nb->num_classes; i++) {
        fprintf(file, "[%2d] Likelihood      ", i);
    }
    fprintf(file, "\n");

    for (int j = 0; j < vocab_size; j++) {
        const char* word = getWordFromIndex(vocab, j);
        fprintf(file, "%-25s", word);  // ชื่อคำ

        for (int i = 0; i < nb->num_classes; i++) {
            int idx = index(i, j, vocab_size);
            if (idx < 0 || idx >= nb->num_classes * vocab_size) {
                fprintf(file, "   [Invalid Index]   ");
                continue;
            }
            fprintf(file, "[%d] %-14.6f ", idx, nb->likelihood[idx]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

int getlikelihood_to_file_csv(Naive_Bayes_OJ* nb,const char* filename) {
    if (!nb || !filename) {
        error_printf("Naive Bayes or filename is NULL\n");
        return -1;
    }
    FILE* file = fopen(filename, "w");
    if (!file) {
        error_printf("Cannot open file %s\n", filename);
        return -1;
    }
    word_hash* vocab = nb->hash;
    int vocab_size = nb->vocab_size;

    fprintf(file, "Word");  // หัวตาราง
    for (int i = 0; i < nb->num_classes; i++) {
        fprintf(file, ",[%2d],Likelihood", i);
    }
    fprintf(file, "\n");

    for (int j = 0; j < vocab_size; j++) {
        const char* word = getWordFromIndex(vocab, j);
        fprintf(file, "%s", word);  // ชื่อคำ

        for (int i = 0; i < nb->num_classes; i++) {
            int idx = index(i, j, vocab_size);
            if (idx < 0 || idx >= nb->num_classes * vocab_size) {
                fprintf(file, ",Invalid Index");
                continue;
            }
            fprintf(file, ",%d,%f", idx, nb->likelihood[idx]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}


int fitNB(Naive_Bayes_OJ* nb, TF_IDF_OJ* tf_idf, data_frame* df) {
    if (!nb || !tf_idf) {
        error_printf("Naive Bayes or TF-IDF object is NULL\n");
        return -1;
    }
    info_printf("Fit Naive Bayes model\n");
    func_printf("Find sigma F(i,C) phase\n");
    start_timer();
    int n = tf_idf->tf_idf_matrix->size;
    int vocab_size = tf_idf->hash->size;
    int index_ = 0;
    for(int i = 0; i < n; i++) {
        int colum = tf_idf->tf_idf_matrix->data[i].col;
        int row = tf_idf->tf_idf_matrix->data[i].row;
        int class_index = atoi(df->data[row].label);
        if(class_index < 0 || class_index >= nb->num_classes){
            error_printf("Class index %d is out of bounds\n", class_index);
            continue;
        }
        index_ = index(class_index, colum, vocab_size);
        if(index_ < 0 || index_ >= nb->num_classes * vocab_size){
            error_printf("Index %d is out of bounds with row is %d and colum %d with realsize %d\n", index_, row, colum,nb->num_classes * vocab_size);
            continue;
        }
        // calculate sigma P(fi,y)
        nb->likelihood[index_] += (float) tf_idf->tf_idf_matrix->data[i].value;
    }
    show_time();
    if(DEBUG){
        getlikelihood_to_file_csv(nb, "assets/debug_likelihood_before_calculate.csv");
        getlikelihood_to_file(nb, "assets/debug_likelihood_before_calculate.txt");
    }
    func_printf("Calculate P(Fi,C) = log(sigma F(i,C) / sigma F(all,C)) phase\n");
    start_timer();
    float sum_class;
    int row = 0;
    while(row < nb->num_classes){
        sum_class = 0;
        for(int j = 0; j < vocab_size; j++){
            int index = index(row, j, vocab_size);
            if(index < 0 || index >= nb->num_classes * vocab_size){
                error_printf("Index %d is out of bounds\n", index);
                continue;
            }
            // sigma F(all,C)
            sum_class += nb->likelihood[index];
        }
        // sigma F(all,C) + ALPHA * vocab_size
        sum_class += ALPHA * vocab_size; // Laplace smoothing
        info_printf("Sum class %d: %f\n", row, sum_class);
        for(int j = 0; j < vocab_size; j++){
            int index = index(row, j, vocab_size);
            if(index < 0 || index >= nb->num_classes * vocab_size){
                error_printf("Index %d is out of bounds\n", index);
                continue;
            }
            // calculate P(fi|C) = ln( F(fi,C) / sigma F(all,C) )
            nb->likelihood[index] /= sum_class;
            nb->likelihood[index] = log(nb->likelihood[index]);
        }
        row++;
    }
    show_time();
    info_printf("Naive Bayes model fitted successfully\n");
    return 0;
}
