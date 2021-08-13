import requests
import json
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.image as mpimg
import pandas as pd

url = 'https://api.thegraph.com/subgraphs/name/sushiswap/exchange'
token_query = """{
  tokens(first: 10, orderBy: volumeUSD, orderDirection: desc) {
    symbol
    id
    volumeUSD
  }
}"""

G = nx.Graph()

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

top_tokens = requests.post(url, json = {"query": token_query})
top_tokens_data = json.loads(top_tokens.text)
top_tokens_ids = []
for token in top_tokens_data['data']['tokens']:
  G.add_node(token['symbol'], image = image_dict[token['symbol']])
  top_tokens_ids.append(token['id'])

#print(top_tokens_ids)

pair_query = """{{
  pairs (
    first: 1,
    where: {{
      token1_in: ["{0}", "{1}"],
      token0_in: ["{1}", "{0}"]
    }}
  ) {{
    id
    name
    reserveUSD
  }}
}}"""

pair_ids = []
"""
pairs = requests.post(url, json = {"query": pair_query.format(top_tokens_ids[0], top_tokens_ids[1])})
pair_data = json.loads(pairs.text)
print(pair_data)
"""

totalLiquidity = 0

edge_data = []

for i in top_tokens_ids:
  for j in top_tokens_ids:
    if (i != j):
      pairs = requests.post(url, json = {"query": pair_query.format(i, j)})
      pair_data = json.loads(pairs.text)
      try:
        edge = pair_data['data']['pairs'][0]
        if float(edge["reserveUSD"]) > 1000:
          pair_ids.append(edge['id'])
          tokens = edge['name'].split("-")
          print(tokens[0], tokens[1])
          edge_data.append((tick_to_index[tokens[0]], tick_to_index[tokens[1]], float(edge["reserveUSD"])))
          G.add_edge(tokens[0], tokens[1], weight=float(edge["reserveUSD"]))
          totalLiquidity += float(edge["reserveUSD"])
      except:
        None

edge_data.append((11, 11, totalLiquidity))

pos = nx.circular_layout(G)

fig=plt.figure(figsize=(6,6))
ax=plt.subplot(111)
ax.set_aspect('equal')

edges,weights = zip(*nx.get_edge_attributes(G,'weight').items())
weights_final = [float(x) for x in weights]
nx.draw_networkx_edges(G, pos, width = 3.0)#ax=ax, edge_color = weights_final, width = 3.0, edge_cmap=cm.get_cmap("Greys"))

plt.xlim(-1,1)
plt.ylim(-1,1)
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
ax.axis('off')


#nx.draw(G, pos, with_labels = True, edgelist = edges, edge_color = weights_final, width = 3.0, edge_cmap=cm.get_cmap("Greys"))

#plt.show()

query = """{{
  swaps (
    first: 1000,
      orderBy: timestamp,
    orderDirection: desc,
    skip: {0},
    where: {{ pair_in: 
      [
          "{1}", 
      ]}}
  ) {{
    timestamp
    amountUSD
    amount1In
    amount0In
    amount0Out
    amount1Out
    id
    pair {{
      token0 {{
        name
        symbol
        id
      }}
      token1 {{
        name
        symbol
        id
      }}
    }}
  }}
}}"""

timestamp_query = """{
  swaps (
      first: 1,
      orderBy: timestamp,
      orderDirection: desc
    ){
      timestamp
    }
}"""

timestamp_rq = requests.post(url, json = {"query": timestamp_query})
timestamp_data = json.loads(timestamp_rq.text)
daystamp = int(timestamp_data['data']['swaps'][0]['timestamp']) - 86400

swaps = []
orderBook = []
#TODO: FIGURE OUT HOW TO GET 24H WORTH OF ORDERS
#MAYBE ONLY RETRIEVE THOSE THAT HAVE TO DO WITH THE DESIRED PAIRS
#^FIND OUT IF THIS IS POSSIBLE
#print(pair_ids)
for pair_id in pair_ids:
  #print(pair_id)
  needSwaps = True
  skipAmnt = 0
  while (needSwaps):
    swaps_rq = requests.post(url, json = {"query": query.format(skipAmnt, pair_id)})
    swaps_data = json.loads(swaps_rq.text)
    skipAmnt += 1000
    index = 0
    while (needSwaps and index != 1000):
      try:
        swap = swaps_data['data']['swaps'][index]
        if (swap not in swaps):
          swaps.append(swap)
          if (float(swaps_data['data']['swaps'][index]['amount0In']) > 0):
            order = (tick_to_index[swap['pair']['token0']['symbol']], tick_to_index[swap['pair']['token1']['symbol']], swap['amountUSD'])
            orderBook.append(order)
          else:
            order = (tick_to_index[swap['pair']['token1']['symbol']], tick_to_index[swap['pair']['token0']['symbol']], swap['amountUSD'])
            orderBook.append(order)
          if (int(swap['timestamp']) <= daystamp):
            needSwaps = False
        index += 1
      except:
        needSwaps = False


df_ob = pd.DataFrame(orderBook, columns=["Source", "Target", "Amount"])
df_ob.to_csv("orderbook.csv")

df_ed = pd.DataFrame(edge_data, columns=["Node 1", "Node 2", "Liquidity"])
df_ed.to_csv("edges_data.csv")

print(len(orderBook))
print(totalLiquidity)
