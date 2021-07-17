from Classes import Order
from NetIntegratedClasses import loss_function
from helpers import create_samples
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

#heuristics ideas
# if node not in order book, don't look through it



total_liquidity = 100000
order_book = [Order(100, "wETH", "SHIB"), Order(100, "SHIB", "ZAP"), Order(100, "ZAP", "UNI"), Order(100, "UNI", "wETH")]
#edges = []

#edges = [["wETH", "SHIB", 1-2], ["SHIB", "ZAP", 2-3], ["ZAP", "UNI", 3-4], ["UNI", "wETH", 4-1], ["wETH", "ZAP", 1-3], ["UNI", "SHIB", 2,4]]

edges = [["wETH", "SHIB", total_liquidity/4], ["SHIB", "ZAP", total_liquidity/4], ["ZAP", "UNI", total_liquidity/4], ["UNI", "wETH", total_liquidity/4], ["wETH", "ZAP", 0], ["UNI", "SHIB", 0]]

print(loss_function(edges, order_book))


