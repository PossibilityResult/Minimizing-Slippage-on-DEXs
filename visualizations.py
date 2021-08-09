import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import pandas as pd
import os
import numpy as np
import imageio

df = pd.read_csv("edges.csv")

G = nx.Graph()

#print(df.iloc[0].tolist()[0].split(" "))
filenames = []
for i in range(345):#df.shape[0]):
    print(i)
    index = 0
    edges = []
    tup = []
    # plot the line chart
    current_cost = df.iloc[i].tolist()[0].split(" ")[0]
    best_cost = df.iloc[i].tolist()[0].split(" ")[1]
    for x in df.iloc[i].tolist()[0].split(" ")[2:]:
        #print(i)
        if (x != "" and index != 2):
            tup.append(x)
            index += 1
        elif (index == 2 and x != ""):
            tup.append(x)
            edges.append(tup)
            #print(tup)
            tup = []
            index = 0

    for edge in edges:
        G.add_edge(edge[0], edge[1], weight=edge[2])
        #print(edge)

    edges,weights = zip(*nx.get_edge_attributes(G,'weight').items())

    weights_final = [float(x) for x in weights]

    pos = nx.circular_layout(G)
    plt.title("k = " + str(i) + "; Current cost: " + str(current_cost) + "; Lowest cost: " + str(best_cost))
    nx.draw(G, pos, edgelist = edges, edge_color = weights_final, width = 3.0, edge_cmap=cm.get_cmap("Greys"))
    
    # create file name and append it to a list
    filename = f'{i}.png'
    filenames.append(filename)
    
    # save frame
    plt.savefig(filename)
    plt.close()
    G.remove_edges_from(list(G.edges))


# build gif
with imageio.get_writer('mygif.gif', mode='I') as writer:
    for filename in filenames:
        image = imageio.imread(filename)
        writer.append_data(image)

# Remove files
for filename in set(filenames):
    os.remove(filename)
