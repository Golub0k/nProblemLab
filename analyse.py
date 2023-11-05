import os
import time
import matplotlib.pyplot as plt
import numpy as np

C_BIN = "nProblem"

steps = 100
particles = (50, 250, 500)
threads = (1, 2, 4, 8, 16, 32)

values = {i: [] for i in threads}

for particles_count in particles:
    for threads_count in threads:
        print(f"Start {particles_count} particles for {threads_count} threads")
        os.system(
            f"python generate.py {particles_count} {steps} ./input_data/input_{particles_count}_{steps}.txt")
        start_time = time.time()
        os.system(
            f"{C_BIN} ./input_data/input_{particles_count}_{steps}.txt ./output_data/output_{particles_count}_{steps}.csv {threads_count}")
        execute_time = time.time() - start_time
        values[threads_count].append(execute_time)
        print(f"Result = {execute_time}")


x = np.arange(len(particles))
fig, ax = plt.subplots(constrained_layout=True)
width = 0.15
multiplier = 0
for attribute, measurement in values.items():
    offset = width * multiplier
    rects = ax.bar(x + offset, measurement, width, label=attribute)
    multiplier += 1

ax.set_title(f'Скорость выполнения для {steps} шагов')
ax.set_xlabel('Кол-во частиц')
ax.set_ylabel('Время выполнения (сек.)')
ax.set_xticks(np.arange(len(particles)) + width *
              (len(threads) / 2 - 0.5), map(str, particles))
ax.legend(loc='upper left', ncols=len(
    threads) / 2, title="Кол-во потоков")

plt.savefig("./statistic/figure.png")
plt.show()

fig2, chart = plt.subplots()
circle = 0
for threads_count, measurement in values.items():
    chart.plot(particles, measurement, label=threads_count)
chart.set_title(f'Скорость выполнения для {steps} шагов')
chart.set_xlabel('Кол-во частиц')
chart.set_ylabel('Время выполнения (сек.)')
chart.set_xlim(left=min(particles), right=max(particles) + 10)
chart.legend(loc='upper left', ncols=len(
    threads) / 2, title="Кол-во потоков")
plt.savefig("./statistic/figure2.png")
plt.show()
