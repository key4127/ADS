import matplotlib.pyplot as plt
import pandas as pd

n = 100000

def paint(path):
    data = pd.read_csv("../compare_test/" + path + "_100000.csv", header=None)
    series = pd.Series(data.iloc[:, 0].tolist())
    cate = pd.cut(series, bins=50)
    bounds = [(interval.left, interval.right) for interval in cate.cat.categories]
    mid = [(left + right) / 2 for left, right in bounds]
    frequency = cate.value_counts(normalize=True).sort_index().values
    plt.plot(mid, frequency)
    plt.savefig("../graph/origin_" + path + ".png")
    plt.close()

def paint_normal(path):
    data = pd.read_csv("../data/" + path + "_100000.csv", header=None)
    series = pd.Series(data.iloc[:, 0].tolist())
    cate = pd.cut(series, bins=50)
    bounds = [(interval.left, interval.right) for interval in cate.cat.categories]
    mid = [(left + right) / 2 for left, right in bounds]
    frequency = cate.value_counts(normalize=True).sort_index().values
    plt.plot(mid, frequency)
    plt.savefig("../graph/origin_" + path + ".png")
    plt.close()

paint("random")
paint("linear")

paint_normal("normal")