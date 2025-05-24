#!/usr/bin/env bash
# Script to run the NaiveBayes model
# Usage: ./run.sh

# Set the working directory
cd /mnt/d/TestC_AI/NLP_TFIDF_NaiveBayes_From_C || exit 1

# Path to the compiled model 
MODEL_PATH="exc/model"  # Assuming your binary is named "test" based on your Makefile

# Arguments
ARGS=(
  assets/Train_Lecture_Set_Data.csv
  assets/Validation_Lecture_Set_Data.csv
  assets/real.txt
  assets/predict.txt
  assets/stopword.txt
  assets/noise.txt
  2
)

# Launch model with program arguments
"$MODEL_PATH" "${ARGS[@]}"