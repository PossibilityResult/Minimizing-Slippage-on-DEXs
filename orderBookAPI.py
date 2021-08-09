import requests

def main():
    resp = requests.get('https://api.cryptowat.ch/markets/dex-aggregated/wbtceth/trades')
    orderbook = resp.json()['result']
    print(orderbook)

if __name__ == '__main__':
    main()