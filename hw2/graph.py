import matplotlib.pyplot as plt

def paint_graph(x, x_exp, x_the, title):
    plt.plot(x, x_exp, label='experimental value')
    plt.plot(x, x_the, label='theoretical value')
    plt.legend()
    plt.xlabel(title)
    plt.ylabel('false positive rate')
    plt.xticks(x)
    plt.savefig('./graph/{}.png'.format(title[:1]))
    plt.close()

mn = [2, 3, 4, 5]
mn_exp = [0.39, 0.26, 0.18, 0.13]
mn_the = [0.393, 0.283, 0.221, 0.181]

k = [1, 2, 3, 4, 5]
k_exp = [0.13, 0.09, 0.09, 0.08, 0.11]
k_the = [0.181, 0.109, 0.092, 0.092, 0.101]

paint_graph(mn, mn_exp, mn_the, "m/n")
paint_graph(k, k_exp, k_the, "k")