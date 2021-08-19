import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import pandas as pd
import os
import numpy as np
import matplotlib.image as mpimg
import imageio

image_dict = {
  "WETH" : mpimg.imread('WETH.png'), 
  "USDC" : mpimg.imread('USDC.png'),
  "DAI" : mpimg.imread('DAI.png'),
  "USDT" : mpimg.imread('USDT.png'),
  "SUSHI" : mpimg.imread('SUSHI.png'),
  "WBTC" : mpimg.imread('WBTC.png'),
  "YFI" : mpimg.imread('YFI.png'),
  "SNX" : mpimg.imread('SNX.png'),
  "AAVE" : mpimg.imread('AAVE.png'),
  "LINK" : mpimg.imread('LINK.png'),
}

tick_to_index = {
  "WETH" : 0, 
  "USDC" : 1,
  "DAI" : 2,
  "USDT" : 3,
  "SUSHI" : 4,
  "WBTC" : 5,
  "YFI" : 6,
  "SNX" : 7,
  "AAVE" : 8,
  "LINK" : 9,
}

index_to_tick = {
  0 : "WETH", 
  1 : "USDC",
  2 : "DAI",
  3 : "USDT",
  4 : "SUSHI",
  5 : "WBTC",
  6 : "YFI",
  7 : "SNX",
  8 : "AAVE",
  9 : "LINK",
}

df = pd.read_csv("edges.csv")

G = nx.Graph()

#print(df.iloc[0].tolist()[0].split(" "))
filenames = []
for i in range(df.shape[0]):
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
    
    for integer in range(10):
        G.add_node(index_to_tick[integer], image = image_dict[index_to_tick[integer]])

    for edge in edges:
        G.add_edge(index_to_tick[int(edge[0])], index_to_tick[int(edge[1])], weight=edge[2])
        #print(edge)

    pos = nx.circular_layout(G)
    #fig=plt.figure(figsize=(7,7))
    #ax=plt.subplot(111)
    #ax.set_aspect('equal')

    
    edges,weights = zip(*nx.get_edge_attributes(G,'weight').items())
    weights_final = [float(x) for x in weights]
    nx.draw_networkx_edges(G, pos, width = 3.0)#edge_color = weights_final, width = 3.0, edge_cmap=cm.get_cmap("Greys"))

    ax=plt.gca()
    fig=plt.gcf()

    plt.xlim(-1.1,1.1)
    plt.ylim(-1.1,1.1)
    trans=ax.transData.transform
    trans2=fig.transFigure.inverted().transform
    piesize=0.1 # this is the image size
    p2=piesize/2.0
    for n in G:
        xx,yy=trans(pos[n]) # figure coordinates
        xa,ya=trans2((xx,yy)) # axes coordinates
        a = plt.axes([xa-p2,ya-p2, piesize, piesize])
        a.set_aspect('equal')
        a.imshow(G.nodes[n]['image'])
        a.axis('off')
    #ax.axis('off')
    #ax.xaxis.set_label_position('top')
    #ax.xaxis.tick_top()
    ax.set_title("Round: " + str(i) + "; Current cost: " + str(current_cost) + "; Lowest cost: " + str(best_cost))
    ax.axis('off')
    fig.set_size_inches(6, 6)
    #nx.draw(G, pos, edgelist = edges, edge_color = weights_final, width = 3.0, edge_cmap=cm.get_cmap("Greys"))
    #nx.draw(G, pos, with_labels = True, edgelist = edges, edge_color = weights_final, width = 3.0, edge_cmap=cm.get_cmap("Greys"))
    
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
