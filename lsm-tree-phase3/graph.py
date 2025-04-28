import matplotlib.pyplot as plt

xs = [
    [6, 8, 10, 12, 14], # M
    [4, 6, 8, 10, 12], # L
    [25, 30, 35, 40, 45], # ef
    [12, 15, 18, 21, 24]  # max
]

rates = [
    [0.725, 0.750, 0.736, 0.725, 0.664], # M
    [0.742, 0.750, 0.700, 0.811, 0.731], # L
    [0.736, 0.744, 0.750, 0.772, 0.772], # ef
    [0.686, 0.742, 0.750, 0.750, 0.750]  # max
]

times = [
    [161881, 186525, 216079, 248820, 275645], # M
    [159587, 187418, 217693, 261541, 262893], # L
    [161196, 173764, 177908, 183148, 194347], # ef
    [175522, 176528, 178222, 179402, 176718]  # max
]

path = ["M", "m_L", "efConstruction", "M_max"]

def paint():
    for name, x, rate, time in zip(path, xs, rates, times):
        plt.plot(x, rate)
        plt.xlabel(name)
        plt.ylabel("rate")
        plt.tight_layout()
        plt.savefig("./graph/{}_rate.png".format(name))
        plt.close()

        plt.plot(x, time)
        plt.xlabel(name)
        plt.ylabel("time")
        plt.tight_layout()
        plt.savefig("./graph/{}_time.png".format(name))
        plt.close()

paint()