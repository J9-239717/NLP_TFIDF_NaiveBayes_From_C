#!/usr/bin/env bash
# Script to launch gdb for the NaiveBayes model
# Usage: ./run_gdb.sh

# Set the working directory
cd /mnt/d/TestC_AI/NLP_TFIDF_NaiveBayes_From_C || exit 1

# Path to the compiled model and data files
MODEL_PATH="exc/model_debug"
ARGS=(
  assets/Train_Lecture_Set_Data.csv
  assets/Test_Lecture_Set_Data.csv
  assets/real.txt
  assets/predict.txt
  assets/stopword.txt
  assets/noise.txt
  4
)

# Launch gdb with program arguments
# --args tells gdb that following tokens are program + its args
exec gdb --args "$MODEL_PATH" "${ARGS[@]}"
