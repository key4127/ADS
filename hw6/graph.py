import matplotlib.pyplot as plt

x = list(range(1, 41))
y = [1, 1.6, 2.6, 2.5, 3.6, 3.7, 3.5, 3.4,
     4.5, 3.9, 4, 3.7, 4.1, 3.9, 4.4, 4,
     4.7, 4.5, 5.4, 4.1, 4.3, 4.1, 4.2, 4.4,
     4.1, 4.3, 4, 4.4, 3.8, 4.3, 3.8, 4.2,
     4.3, 4.2, 3.7, 4.5, 4.4, 4.8, 4.3, 4]

plt.plot(x, y)
plt.xlabel("max threadNum")
plt.ylabel("speedup ratio")
plt.savefig("./graph.png")