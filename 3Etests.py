import networkx as nx
from Classes import Path, Order, Orderbook
from ModifiedNetworkX import calcSlippageLoss, _slippage_dijkstra_alg, slippage_dijkstra
from NetIntegratedClasses import loss_function
from helpers import random_allocation_sampler, create_samples
import tensorflow as tf
from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt

#heuristics ideas
# if node not in order book, don't look through it
# if liquidity less than order on order book delete it 


total_liquidity = 100000
order_book = [Order(100, "wETH", "ZAP"), Order(100, "wETH", "SHIB"), Order(100, "SHIB", "ZAP")]
#edges = []

#edges = [["wETH", "SHIB", 10000], ["ZAP", "SHIB", 20000], ["wETH", "ZAP", 5000]]

edges = [["wETH", "SHIB"], ["ZAP", "SHIB"], ["wETH", "ZAP"]]

samples = create_samples(edges, total_liquidity, 1000)

print(samples)

order_book = [Order(100, "wETH", "ZAP"), Order(100, "wETH", "SHIB"), Order(100, "SHIB", "ZAP")]

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

ax.set_xlabel("Liquidiy (wETH - SHIB)")
ax.set_ylabel("Liquidiy (ZAP - SHIB)")
ax.set_zlabel("Liquidiy (wETH - ZAP)")
plt.show()
        






