import matplotlib.pyplot as plt
import numpy as np

ps = ["1/2", "1/e", "1/4", "1/8"]
ns = [50, 100, 200, 500, 1000]

experiment = [[10.457900, 8.809000, 9.055900, 11.007900],
              [12.722300, 11.773500, 12.117100, 13.401500],
              [12.624300, 11.87810, 11.323200, 12.512300],
              [16.483500, 14.805100, 14.391000, 17.154300],
              [17.292900, 15.695300, 15.438800, 20.355300]]

theory = [[13.287712, 12.215958, 12.621046, 16.193140],
          [15.287712, 14.100127, 14.621046, 18.859807],
          [17.287712, 15.984297, 16.621046, 21.526474],
          [19.931569, 18.475033, 19.264902, 25.051615],
          [21.931569, 20.359202, 21.264902, 27.718282]]

for i in range(5):
    n = ns[i]
    plt.plot(ps, experiment[i], label = "experimental value")
    plt.plot(ps, theory[i], label = "theoretical value")
    plt.xlabel("p")
    plt.ylabel("average search length")
    plt.legend()
    plt.title("n = {}".format(n))
    plt.show()
    plt.savefig("./graph/{}.png".format(n))
    plt.close()