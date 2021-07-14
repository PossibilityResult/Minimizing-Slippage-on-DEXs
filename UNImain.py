from UNIclasses import token, pool
from UNIcollectclean import returnTokensandPrice
from UNIdijkstraSL import UNIdjSL
from UNIdijkstraTFSL import UNIdjTFSL
import networkx as nx
import matplotlib.pyplot as plt

tokens = returnTokensandPrice()

source_coin = 'Wrapped Ether'

mode = input("Just Slippage? (type: 1) or Trading Fees and Slippage? (type: 2): ")

starting_USD = input("How much USD worth of {} would you like to exchange?".format(source_coin))

if (mode == '1'):
    graph = UNIdjSL(tokens, source_coin, float(starting_USD))
elif (mode == '2'):
    graph = UNIdjTFSL(tokens, source_coin, float(starting_USD))
else:
    print("Invalid Mode")

G = nx.Graph()
#TODO: add a mapping from token ticker to index (this way we can add edges)
#TODO: create a separate file
#list of tuples with edges and weights where edge[0] is tok1, edge[1] is tok2, and edge[2] is the weight/slippage
token_edges_weights_indexed = []
for edge in graph:
    G.add_edge(edge[0], edge[1], weight = graph[edge])

pos = nx.spring_layout(G)  # positions for all nodes

#nodes
d = dict(G.degree)
nx.draw(G, pos, nodelist=d.keys(), node_size=[v*10 for v in d.values()])

# edges
nx.draw_networkx_edges(G, pos,  width=.2)
nx.draw_networkx_edges(G, pos, width=.2, alpha=0.5, edge_color="b", style="dashed")

# labels
nx.draw_networkx_labels(G, pos, font_size=1, font_family="sans-serif")

plt.axis("off")
plt.show()