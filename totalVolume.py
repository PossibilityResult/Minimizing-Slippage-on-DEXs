import pandas as pd

df = pd.read_csv("orderbook.csv")
Total = df['Amount'].sum()
print (Total*.003)