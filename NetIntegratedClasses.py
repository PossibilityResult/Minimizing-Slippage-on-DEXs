import networkx as nx
from Classes import Path, Order, Orderbook
from ModifiedNetworkX import calcSlippageLoss, _slippage_dijkstra_alg, slippage_dijkstra
#tuple of token 1, token 2 and total liquidity
def loss_function(edges, order_book):
    G = nx.Graph(type="main")

    for edge in edges: 
        if (edge[0] not in G):
            G.add_node(edge[0])

        if (edge[1] not in G):
            G.add_node(edge[1])

        G.add_edge(edge[0], edge[1], liquidity = edge[2])

    total_slippage_loss = 0

    for o in order_book:
        total_slippage_loss += slippage_dijkstra(G, o.amount, o.source, o.target)[0]

    return total_slippage_loss


#test case: 4 nodes, fixed liquidity
"""
edges = [["wETH", "SHIB", 10000], ["DAI", "SHIB", 20000], ["wETH", "ZAP", 5000], ["ZAP", "DAI", 10000]]
order_book = [Order(100, "wETH", "DAI"), Order(50, "wETH", "SHIB"), Order(75, "DAI", "ZAP")]
"""

