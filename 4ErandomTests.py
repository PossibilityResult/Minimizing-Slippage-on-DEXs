from Classes import Order
from NetIntegratedClasses import loss_function
from helpers import create_samples
import numpy as np
import pandas as pd

#heuristics ideas
# if node not in order book, don't look through it
# if liquidity less than order on order book delete it 


total_liquidity = 100000
#edges = []

#edges = [["wETH", "SHIB", 1-2], ["SHIB", "ZAP", 2-3], ["ZAP", "UNI", 3-4], ["UNI", "wETH", 4-1], ["wETH", "ZAP", 1-3], ["UNI", "SHIB", 2,4]]

edges = [["1", "2"], ["2", "3"], ["3", "4"], ["1", "4"], ["1", "3"], ["2", "4"]]

samples = create_samples(edges, total_liquidity, 10000)

order_book = [Order(300, "1", "2"), Order(200, "2", "3"), Order(400, "3", "4"), Order(600, "1", "4"), Order(100, "1", "3"), Order(500, "2", "4")]

data = []

for i in range(len(samples)):
    data.append([loss_function(samples[i], order_book), samples[i][0][2], samples[i][1][2], samples[i][2][2], samples[i][3][2], samples[i][4][2], samples[i][5][2]])


data.sort(key = lambda tup: tup[0])

data.insert(0, ["Slippage Loss", "Liquidity ({})".format(samples[i][0][0] + " - " + samples[i][0][1]), "Liquidity ({})".format(samples[i][1][0] + " - " + samples[i][1][1]), "Liquidity ({})".format(samples[i][2][0] + " - " + samples[i][2][1]), "Liquidity ({})".format(samples[i][3][0] + " - " + samples[i][3][1]), "Liquidity ({})".format(samples[i][4][0] + " - " + samples[i][4][1]), "Liquidity ({})".format(samples[i][5][0] + " - " + samples[i][5][1])])

#for tup in data[:30]:
    #print(tup)

df = pd.DataFrame(data[1:], columns=data[0])

df[:10].to_csv("best_allocations.csv")
