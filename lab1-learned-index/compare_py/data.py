import numpy as np
import random
import csv

def random_data(n):
    list = []
    rand = random.sample(range(-n, n), n)
    rand.sort()
    for i in range(n):
        list.append([rand[i], i])
    return list

def linear_data(n):
    list = []
    for i in range(n):
        list.append([i - n // 2, i])
    return list

def write_csv(path, list):
    with open(path, mode="w", newline="") as file:
        writer = csv.writer(file)
        for row in list:
            writer.writerow(row)

# normal data in data directory
# others in compare_test directory
def test(path, n):
    write_csv(path + "random_{}.csv".format(n), random_data(n))
    write_csv(path + "linear_{}.csv".format(n), linear_data(n))

test("../compare_test/", 10000)
test("../compare_test/", 100000)