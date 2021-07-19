from Classes import Order
from NetIntegratedClasses import loss_function
from helpers import create_samples
from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt


total_liquidity = 100000
order_book = [Order(100, "1", "2"), Order(100, "2", "3"), Order(100, "3", "1")]

two_edges = [["1", "2", total_liquidity/2], ["2", "3", total_liquidity/2], ["1", "3", 0]]
three_edges = [["1", "2", total_liquidity/3], ["2", "3", total_liquidity/3], ["1", "3", total_liquidity/3]]

print("two edges: " + str(loss_function(two_edges, order_book)))
print("three edges: " + str(loss_function(three_edges, order_book)))