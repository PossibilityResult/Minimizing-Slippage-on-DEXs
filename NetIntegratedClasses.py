import networkx as nx
from ModifiedNetworkX import _dijkstra_slippage, calcSlippageLoss,  multi_source_dijkstra, single_source_dijkstra

G = nx.Graph(type="main")

#tuple of token 1, token 2 and total liquidity
edges = [["wETH", "SHIB", 10000], ["DAI", "SHIB", 20000], ["wETH", "ZAP", 5000], ["ZAP", "DAI", 10000]]

for edge in edges: 
    if (edge[0] not in G):
        G.add_node(edge[0])

    if (edge[1] not in G):
        G.add_node(edge[1])

    G.add_edge(edge[0], edge[1], liquidity = edge[2])

#TODO: 1) Bring in slippage loss function
#2) Check graph structure with visualization

print(single_source_dijkstra(G, "wETH"))

