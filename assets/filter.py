import re

with open("wordhash.txt", encoding="utf-8") as f:
    lines = f.readlines()

low_freq_words = []
for line in lines:
    match = re.search(r"{\s*(.+?)\s*,\s*(\d+)\s*}", line)
    if match:
        word, freq = match.groups()
        if int(freq) <= 2:
            low_freq_words.append((word, int(freq)))

print("คำที่ควรถูกลบ (ความถี่ ≤ 2):")
for word, freq in low_freq_words:
    print(f"{word} ({freq})")
