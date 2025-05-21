
# NLP: TFIDF and Naive Bayes Using C pure

This is my classroom's project in class Introduction of AI
i choose C language because i want to know how to build one model from scratch and how to optimize performance for model
(but i think my own build model it's quite slow than it should be)

using `makefile` for build




## Build and Run

**For Linux bulild:**
```bash
  make # build
```
**For Window bulild:**
```bash
  mingw32-make # build
```

#### file execute will in exec folder
```bash
  exec/my-program <file-Train> <file-test-set> <file-real-test> /
  <file-out-put-predict> <file-stop-word> <file-noise> /
  <num-of-ngram> <op>
```

#### For `op` argument
  - c_n for make file noise word for filter noise off data frame and n is number *Ex:*  `c_3`. it's mean if which word have frequency lower or equal n(3) it's will be count to noise word and you will filter it later with hand
  - null or something for run normally

## Warning:
Data file should like example, should have 3 class: Positive, Negative, Neutral because in last 3 or 5 commit i wrote bad code on src that is fix data size something that fix not dynamic follow file that make if not have only 3 class it will make it **Error** Sorry :>
