# generate latex tabular

import math 
# log: log(x, base)

def returnSearchLengh(n, p):
    return -(math.log(n, p) / p) + 1 / (1 - p)

ns = [50, 100, 200, 500, 1000]
ps = [0.5, 1 / math.e, 0.25, 0.125]

for n in ns:
    search = []
    for p in ps:
        search.append(returnSearchLengh(n, p))
    print("{} & {} & {} & {} & {} \\\\".format(n, format(search[0], '.6f'), 
                                                format(search[1], '.6f'),
                                                format(search[2], '.6f'),
                                                format(search[3], '.6f')))
    print("\\hline")