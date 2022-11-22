from Classes import Order
from NetIntegratedClasses import loss_function
from helpers import create_samples
from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt

#heuristics ideas
# if node not in order book, don't look through it
# if liquidity less than order on order book delete it 


total_liquidity = 10000
order_book = [Order(100, "1", "2"), Order(100, "2", "3"), Order(100, "3", "1")]

edges = [["1", "2"], ["2", "3"], ["1", "3"]]

samples = create_samples(edges, total_liquidity, 100000)

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
ax.set_ylabel("Liquidity $ (2 - 3)")
ax.set_zlabel("Liquidity $ (3 - 1)")
plt.show()
        






