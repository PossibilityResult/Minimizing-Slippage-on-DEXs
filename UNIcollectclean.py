import json
from UNIclasses import token, pool
import copy
import requests


def returnTokensandPrice():
  query_one = """{
    pairs(first: 1000, orderBy: reserveUSD, orderDirection: desc) {
      id
      reserveUSD
      token0 {
        name
        totalLiquidity
      }
      token1 {
        name
        totalLiquidity
      }
      token0Price
      token1Price
    }
  }"""
  query_two = """{
    pairs(first: 1000, skip: 1000, orderBy: reserveUSD, orderDirection: desc) {
      id
      reserveUSD
      token0 {
        name
        totalLiquidity
      }
      token1 {
        name
        totalLiquidity
      }
      token0Price
      token1Price
    }
  }"""

  edges_dict = {}
  price_dict = {}

  url = 'https://api.thegraph.com/subgraphs/name/uniswap/uniswap-v2'

  obj_one = requests.post(url, json = {"query": query_one})
  json_data_one = json.loads(obj_one.text)

  obj_two = requests.post(url, json = {"query": query_two})
  json_data_two = json.loads(obj_two.text)

  for pair in json_data_one['data']['pairs']:
    if (float(pair['reserveUSD']) > 500):
    #if (float(pair['token0']['totalLiquidity']) > 250 and float(pair['token1']['totalLiquidity']) > 250):
      key = (pair['token0']['name'], pair['token1']['name'])
      edges_dict[key] = float(pair['reserveUSD'])
      if (pair['token0']['name'] not in price_dict and pair['token0']['name'] != 'ulock.eth Wrapped Ether'):
        price_dict[pair['token0']['name']] = float(pair['token1Price'])
      if (pair['token1']['name'] not in price_dict and pair['token1']['name'] != 'ulock.eth Wrapped Ether'):
        price_dict[pair['token1']['name']] = float(pair['token0Price'])

  for pair in json_data_two['data']['pairs']:
    if (float(pair['reserveUSD']) > 500):
    #if (float(pair['token0']['totalLiquidity']) > 250 and float(pair['token1']['totalLiquidity']) > 250):
      key = (pair['token0']['name'], pair['token1']['name'])
      edges_dict[key] = float(pair['reserveUSD'])
      if (pair['token0']['name'] not in price_dict and pair['token0']['name'] != 'ulock.eth Wrapped Ether'):
        price_dict[pair['token0']['name']] = float(pair['token1Price'])
      if (pair['token1']['name'] not in price_dict and pair['token1']['name'] != 'ulock.eth Wrapped Ether'):
        price_dict[pair['token1']['name']] = float(pair['token0Price'])


#TEST CASE 1:
#price_dict ={"WETH" : 3000, "USDT": 1.01, "SHIB": .0005, "DOGE" : .01, "FEI" : .99}
  tokens = {}

  #TODO: Add a cleaner that automatically creates the untouchable list
  #Uniswap archipeligo, alternative, hardcoded cleaner: using while loop w/elena protocol instead
  #untouchables = ['Ethereum2Proof-of-stake', 'Dcoin', 'Diamond Blue','Blue Finance','Degen Ape','uLockS','Rootkit','open euro','hybrix hydra','Cap Coin GLD','Cap Coin','CBTC TOKEN','Free link','uLockXS','Bit Chinese YuanBITCNY','Zum Dark','Zum Token','BeeToken','Block Target Business','RONPROTOCOL','FOMOPROTOCOL','UniYield','Univalue','WINNER','fcore.finance','Best token','Eterbase Coin','EURBASE Stablecoin','Thor Company','Poseidon Foundation','xUAH','xUSD', "uLock", "Hanu Yokia", "Mia Neko", "Goji", "Rootkit"]

  #copy_dict = copy.deepcopy(edges_dict)

  #for key in copy_dict:
    #print(key[0])
    #if (key[0] in untouchables and key[1] in untouchables):
      #del edges_dict[key]
      #if (key[0] in price_dict):
        #del price_dict[key[0]]
      #if (key[1] in price_dict):
        #del price_dict[key[1]]

#initializes tokens with price and ticker (pools are left empty for now)
  for item in price_dict.items():
    tokens[item[0]] = token(item[1], item[0])
#print(len(tokens.items()))

#TEST CASE 1:
#edges_dict = {("WETH","USDT"): 5000, ("USDT", "SHIB"): 5000, ("DOGE", "SHIB") : 5000, ("FEI", "SHIB") : 5000, ("DOGE", "WETH") : 5000}


#for each token initialize the reserve pools
  count = 1
  for tok in tokens:
    ticker = tokens[tok].ticker
    for item in edges_dict.items():
      if(ticker in item[0]):
        if (ticker == item[0][0]):
          tokens[tok].pools[item[0][1]] = pool(item[1], price_dict, ticker, item[0][1]) 
        else:
          tokens[tok].pools[item[0][0]] = pool(item[1], price_dict, ticker, item[0][0]) 
  
  return tokens
