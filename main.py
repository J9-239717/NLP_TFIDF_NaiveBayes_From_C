#!/usr/bin/env python3
import subprocess
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import (
    confusion_matrix,
    accuracy_score,
    f1_score,
    classification_report
)

# ————— EDIT THESE —————
WIN_TRAIN_CSV = r"D:\TestC_AI\pythonRun\file\Data1\Train_Lecture_Set_Data.csv"
WIN_TEST_CSV  = r"D:\TestC_AI\pythonRun\file\Data1\Test_Lecture_Set_Data.csv"
WIN_TRUE_TXT  = r"D:\TestC_AI\pythonRun\file\true.txt"
WIN_PRED_TXT  = r"D:\TestC_AI\pythonRun\file\predict.txt"
WIN_STOP_WORD = r"D:\TestC_AI\pythonRun\file\stopword.txt"
WIN_NOISE     = r"NULL"
WIN_MODEL     = r"D:\TestC_AI\pythonRun\model.exe"

MODEL_NAME    = "SentimentNB"
CLASS_NAMES   = ["negative", "neutral", "positive"]
# ————————————————————

def run_experiment(ngram: int):
    """Run the Windows model with a given n-gram, compute metrics, log and plot."""
    # 1️⃣ Build and run the command
    cmd = [
        WIN_MODEL,
        WIN_TRAIN_CSV,
        WIN_TEST_CSV,
        WIN_TRUE_TXT,
        WIN_PRED_TXT,
        WIN_STOP_WORD,
        WIN_NOISE,
        str(ngram),
    ]
    print(f"\n→ Running model with n-gram = {ngram}\n  ", " ".join(cmd))
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"✖ Model failed (exit {e.returncode}), skipping n-gram={ngram}")
        return

    # 2️⃣ Load outputs
    try:
        y_true = np.loadtxt(WIN_TRUE_TXT, dtype=int)
        y_pred = np.loadtxt(WIN_PRED_TXT, dtype=int)
    except Exception as e:
        print(f"✖ Error loading labels: {e}")
        return

    if y_true.size == 0 or y_pred.size == 0:
        print("⚠ Label files are empty—aborting.")
        return

    # 3️⃣ Align lengths
    if len(y_true) != len(y_pred):
        n = min(len(y_true), len(y_pred))
        print(f"⚠ Truncating to {n} samples (true={len(y_true)}, pred={len(y_pred)})")
        y_true, y_pred = y_true[:n], y_pred[:n]

    # 4️⃣ Compute metrics
    acc  = accuracy_score(y_true, y_pred)
    w_f1 = f1_score(y_true, y_pred, average='weighted')
    summary = f"n-gram={ngram} → Accuracy: {acc:.2f}, Weighted F1: {w_f1:.2f}\n"
    full_report = classification_report(
        y_true, y_pred,
        target_names=CLASS_NAMES,
        digits=2
    )

    # 5️⃣ Append to log_acc
    with open("log_acc", "a", encoding="utf-8") as log_file:
        log_file.write(summary)
        log_file.write(full_report)
        log_file.write("\n" + ("-" * 40) + "\n")

    # 6️⃣ Print to console
    print(summary, full_report)

    # 7️⃣ Plot confusion matrix
    cm = confusion_matrix(y_true, y_pred)
    plt.figure(figsize=(8, 6))
    sns.heatmap(
        cm, annot=True, fmt='d', cmap='Blues',
        xticklabels=CLASS_NAMES, yticklabels=CLASS_NAMES,
        annot_kws={"fontsize":12}
    )
    plt.title(f"Confusion Matrix for {MODEL_NAME} (n-gram={ngram})", fontsize=16)
    plt.xlabel("Predicted Label", fontsize=14)
    plt.ylabel("True Label", fontsize=14)
    plt.xticks(rotation=45)
    plt.yticks(rotation=45)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    # Loop n-gram from 1 through 5
    for ng in range(1, 6):
        run_experiment(ng)
