# plot_two_lines.py
import csv
import matplotlib.pyplot as plt
import numpy as np

n_pr = []
ops_pr = []

n_non = []
ops_non = []

with open("square_partition_primes.csv", newline='') as file:
    reader = csv.reader(file, delimiter=';')
    next(reader)
    for row in reader:
        if len(row) < 3:
            continue
        n_pr.append(int(row[0]))
        ops_pr.append(int(row[2]))

with open("square_partition_nonprimes.csv", newline='') as file:
    reader = csv.reader(file, delimiter=';')
    next(reader)
    for row in reader:
        if len(row) < 3:
            continue
        n_non.append(int(row[0]))
        ops_non.append(int(row[2]))

plt.figure(figsize=(18, 12))
plt.plot(n_pr, ops_pr, marker='o', linestyle='-', linewidth=2, label='Простые числа больше 5')
plt.plot(n_non, ops_non, marker='o', linestyle='-', linewidth=2, label='Остальные числа')
plt.yscale('log')
plt.xlabel("Размер квадрата N", fontsize=12)
plt.ylabel("Количество операций (log scale)", fontsize=12)
plt.title("Зависимость количества операций от размера N (лог-шкала)", fontsize=14)
plt.grid(True, which="both", alpha=0.3)
xticks = sorted(set(n_pr) | set(n_non))
plt.xticks(xticks)
plt.legend()
plt.tight_layout()
plt.savefig("operations_vs_N_log.png", dpi=200)
plt.show()