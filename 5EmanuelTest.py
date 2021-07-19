from Classes import Order
from NetIntegratedClasses import loss_function
from helpers import create_samples
import numpy as np
import pandas as pd

total_liquidity = 100000
#edges = []

#edges = [["wETH", "SHIB", 1-2], ["SHIB", "ZAP", 2-3], ["ZAP", "UNI", 3-4], ["UNI", "wETH", 4-1], ["wETH", "ZAP", 1-3], ["UNI", "SHIB", 2,4]]

four_edges = [["1", "2", total_liquidity/5], ["1", "3", total_liquidity/5], ["1", "4", total_liquidity/5], ["1", "5", total_liquidity/5], ["2", "5", total_liquidity/5]]

#samples = create_samples(edges, total_liquidity, 10000)

order_book = [Order(100, "1", "2"), Order(100, "1", "3"), Order(100, "1", "4"), Order(100, "1", "5"), Order(100, "2", "3"), Order(100, "2", "4"), Order(100, "2", "5"), Order(100, "3", "4"), Order(100, "3", "5"), Order(100, "4", "5")]

print("four edges: " + str(loss_function(four_edges, order_book)))