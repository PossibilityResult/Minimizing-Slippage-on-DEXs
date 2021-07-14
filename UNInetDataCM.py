import requests
from bs4 import BeautifulSoup as bs
from pprint import pprint
import pandas as pd

def get_row(item):
    d = {}

    for k,v in item.items():
        if k in ['market_pair_base', 'market_pair_quote', 'quote']:
            for nk, nv in v.items():
                if k == 'quote':
                    for nnk, nnv in nv.items():
                        d[k +'-' + nk + '-' + nnk] = nnv
                else:
                    d[k +'-' + nk] = nv
        else:
            d[k] = v
    return d
        
headers = {'User-Agent':'Mozilla/5.0'}
r = requests.get('https://web-api.coinmarketcap.com/v1/exchange/market-pairs/latest?aux=num_market_pairs,category,fee_type,market_url,currency_name,currency_slug,effective_liquidity&convert=USD,BTC&limit=5000&market_status=active&slug=uniswap-v2&start=1',
                  headers = headers)
#print(type(r))
data = r.json()['data']['market_pairs']
#print(len(data))
results = [get_row(item) for item in data]
df = pd.DataFrame(results)
#df['Rank'] = [i for i in range(1, len(data) + 1)]
print(df.columns)
print(df[['market_pair','quote-exchange_reported-volume_24h_base']])

df = df[['market_pair','quote-USD-volume_24h', 'quote-USD-price']]
df.to_csv('pairs.csv', encoding = 'utf-8', index=False)

token_count = {}
for pair in df['market_pair']:
    temp = pair.split('/')
    if (temp[0] in token_count):
        token_count[temp[0]] += 1
    else:
        token_count[temp[0]]  = 1

    if (temp[1] in token_count):
        token_count[temp[0]] += 1
    else:
        token_count[temp[1]]  = 1

tc_to_csv = {}
tc_to_csv[1] = []
tc_to_csv[2] = []
for tok in token_count.items():
    tc_to_csv[1].append(tok[0])
    tc_to_csv[2].append(tok[1])
token_count = pd.DataFrame(tc_to_csv)
token_count = token_count.sort_values(by=[2], ascending=False)
token_count.to_csv('TokenCount.csv', encoding = 'utf-8', index=False)

#df = df[df['quote-USD-effective_liquidity'] > 0]
first_coin = []
second_coin = []
for pair in df['market_pair']:
    temp = pair.split('/')
    first_coin.append(temp[0])
    second_coin.append(temp[1])
df['first_coin'] = first_coin
df['second_coin'] = second_coin

intersection_set = set.union(set(first_coin), set(second_coin))

id = 1
nodes_map = {}
for i in list(intersection_set):
    nodes_map[i] = id
    id += 1


source = []
target = []
for pair in df['market_pair']:
    temp = pair.split('/')
    source.append(nodes_map[temp[0]])
    target.append(nodes_map[temp[1]])

df['Source'] = source
df['Target'] = target


nodes = pd.DataFrame(pd.Series(nodes_map))
nodes.reset_index(inplace=True)
nodes.set_index(nodes.columns[1], inplace=True)
nodes.rename(columns = {'index' : "Labels"}, inplace=True)
nodes['id'] = [i+1 for i in range(nodes.shape[0])]
nodes = nodes[['id', 'Labels']]

df['Type'] = 'Undirected'
df.rename(columns = {'quote-USD-volume_24h' : "Weight"}, inplace=True)
edges = df[['Source', 'Target', 'Type', 'Weight']]



#print(nodes)
#edges
#print(df.columns)
nodes.to_csv('Nodes.csv', encoding = 'utf-8', index=False)
edges.to_csv('Edges.csv', encoding = 'utf-8', index=False)