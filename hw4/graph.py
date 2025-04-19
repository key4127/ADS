import matplotlib.pyplot as plt

n = [100, 1000, 2000]
q = range(5, 12)

seq = [[159703, 173130, 159797, 196163, 161017, 143385, 173549], 
       [1976262, 1953071, 1851205, 1872897, 1698337, 1538145, 1590003], 
       [3979346, 3775738, 3361424, 3483537, 3953542, 3737819, 4016447]]

rand = [[169992, 153732, 172851, 202796, 161319, 133264, 184107],
        [2156975, 2242252, 1996367, 1986859, 1910739, 1704549, 1736209], 
        [4703768, 4135997, 3967101, 3996790, 4342464, 4667219, 4527882]]

def paint(arr, name):
    plt.figure()
    for i in range(len(arr)):
        plt.plot(q, arr[i], label="n = {}".format(n[i]))
    plt.legend(bbox_to_anchor=(0.97, 0.8))
    plt.xlabel("Q")
    plt.ylabel("time(ns)")
    plt.savefig("./graph/{}.png".format(name))
    plt.close()

paint(seq, "seq")
paint(rand, "rand")