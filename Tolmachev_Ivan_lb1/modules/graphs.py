import csv
import matplotlib.pyplot as plt
import numpy as np

n_values = []
squares_values = []

with open("square_partition_research.csv", newline='') as file:
    reader = csv.reader(file, delimiter=';')
    next(reader)
    for row in reader:
        n_values.append(int(row[0]))
        squares_values.append(int(row[1]))

plt.figure(figsize=(15, 9))
plt.plot(n_values, squares_values, 'bo-', linewidth=2, markersize=6)
plt.xlabel("Размер квадрата N", fontsize=12)
plt.ylabel("Минимальное количество квадратов", fontsize=12)
plt.title("Зависимость минимального количества квадратов от размера N", fontsize=14)
plt.grid(True, alpha=0.3)
plt.xticks(n_values)
plt.xlim(1, 41)

for i, (n, squares) in enumerate(zip(n_values, squares_values)):
    plt.annotate(str(squares), (n, squares), textcoords="offset points", 
                 xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.show()