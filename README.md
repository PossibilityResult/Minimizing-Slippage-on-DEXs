# Minimizing-Slippage-Loss-on-DEXs
An optimization algorithm for liquidity allocation on DEXs


The algorithm has two major components:

- 1) A discrete optimization algorithm for choosing which liquidity pools should exist (a modified version of **Simulated Annealing**, https://en.wikipedia.org/wiki/Simulated_annealing)
- 2) A *kinda* continous optimization algorithm for choosing how much liquidity should be allocated given the set of liquidity pools (an **Evolutionary Algorithm**)

The loss function that the algorithm trains on captures the minimum cost that traders would have to pay given an order book with 24h of transactions and a fully allocated decentralized exchange (DEX).
To do so we created a modified dijkstra's algorithm (https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm), which recursively finds the minimum cost for how much a trader pays in gas, transaction fees and slippage loss for a single transaction. 

To improve run time, the optimization algorithm was entirely written in C++, and its results are stored in respective CSV files which can be visualized using the python visualizations. The necessary packages to run all of the python files are:
- Networkx
- Pandas
- Imageio
- Matplotlib
- Numpy

If you are serious about helping contribute to and organizing the repository, reach out on (https://twitter.com/eljhfx)

