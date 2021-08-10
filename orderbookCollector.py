import requests
import json

url = 'https://api.thegraph.com/subgraphs/name/sushiswap/exchange'
query = """{
  tokens(first: 10, orderBy: volumeUSD, orderDirection: desc) {
    name
    id
    volumeUSD
  }
}"""

sushi = requests.post(url, json = {"query": query})
sushi_data = json.loads(sushi.text)
print(sushi_data)