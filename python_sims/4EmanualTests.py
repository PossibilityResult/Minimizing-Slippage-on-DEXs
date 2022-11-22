from Classes import Order
from NetIntegratedClasses import loss_function
from helpers import create_samples
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

#heuristics ideas
# if node not in order book, don't look through it



total_liquidity = 10000
order_book = [Order(100, "1", "2"), Order(100, "2", "3"), Order(100, "3", "4"), Order(100, "1", "4"), Order(100, "1", "3"), Order(100, "2", "4")]
#edges = []

#edges = [["wETH", "SHIB", 1-2], ["SHIB", "ZAP", 2-3], ["ZAP", "UNI", 3-4], ["UNI", "wETH", 4-1], ["wETH", "ZAP", 1-3], ["UNI", "SHIB", 2,4]]

#edges = [["wETH", "SHIB", total_liquidity/3], ["SHIB", "ZAP", 0], ["ZAP", "UNI", 0], ["UNI", "wETH", total_liquidity/3], ["wETH", "ZAP", total_liquidity/3], ["UNI", "SHIB", 0]]

edges = [["1", "2"], ["1", "4"], ["1", "3"]]

samples = create_samples(edges, total_liquidity, 100000)

#print(loss_function(edges, order_book))

loss_data = []
z_data = []
x_data = []
y_data = []

for i in range(len(samples)):
    loss_data += [loss_function(samples[i], order_book)]
    z_data += [samples[i][0][2]]
    x_data += [samples[i][1][2]]
    y_data += [samples[i][2][2]]


#fig = plt.figure()
ax = plt.axes(projection='3d')

ax.scatter(x_data, y_data, z_data, c=loss_data, cmap='nipy_spectral')

ax.set_xlabel("Liquidity $ (1 - 2)")
ax.set_ylabel("Liquidity $ (1 - 3)")
ax.set_zlabel("Liquidity $ (1 - 4)")
ax.text2D(0.00, 0.95, "4 Token DEX w/ 6 orders, case 1: A_6 = A_1 = A_2 < A_3 = A_4 = A_5", transform=ax.transAxes)
plt.show()


