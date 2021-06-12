# Minimizing-Slippage-Loss-on-DEXs

DECENTRALIZED FINANCE (DeFi)

The rise of Blockchain technology has led to the development of an Internet of value - a growing, global network of traders buying and selling tokens now reaching daily volumes upwards of $70 billion. Within this network, decentralized finance (DeFi) has emerged into the limelight as a major component of the industry. Supporters of Defi stress the importance of establishing a decentralized economy, citing its promise of building an economy that's robust, transparent, and trustless. These goals have motivated investment in DeFi to jump from $1.1 billion in Total Value Locked (TVL) in June of last year to where it is currently - $60 billion TVL across all DeFi ecosystems. 

DECENTRALIZED EXCHANGES(DEXs)

One of the most popular functions of DeFi are Decentralized Exchanges (DEXs) which provide traders with the ability to exchange tokens through the use of on-chain smart contracts. Since transactions are on-chain, traders do not need to worry about trusting a third party with their money. 

Different DEXs are currently competing for market share in the Internet of value. The internet of value is a massive, interconnected network of traders buying and selling tokens from one another. While not every marketplace within the internet of value is decentralized, many are. One of the most popular protocols is, Uniswap is a decentralized trading protocol built on top of the Ethereum blockchain which allows traders to exchange tokens. Uniswap currently holds over $5 billion in liquidity across thousands of different tokens. From May 9th to May 15th, the protocol did over $16 billion USD in volume.

Exchanges on Uniswap and other DEXs are made possible by the creation of token pairs which each contain a liquidity pool (with a reserve for each token). When an exchange is made, the protocol deposits the inputted tokens into its reserve and withdraws the approximately equivalent value of the desired token.

Slippage Loss

Since the quantity received by the trader is calculated after the changes to reserves are made, the trader will receive less tokens than determined by  q_2 = p_1 q_1 at the time of making the exchange. This is referred to as slippage loss and it occurs because the price of the deposited token p_1 = R_2 / R_1 decreases after some amount of token_1 is deposited into the liquidity pool and some amount of token_2 is withdrawn. Slippage loss is especially a problem when no token pair has been established for a particular two tokens. Traders must navigate through existing token pairs until they can get their desired token which accumulates additional slippage loss for each necessary additional exchange. 

While slippage loss is only a small fraction of individual trades, when measured collectively for every exchange across DEXs it costs traders millions annually. If DeFi continues to grow, so will total slippage loss, creating a dire need for solutions before losses reach the billions.

MINIMIZING SLIPPAGE LOSS

In the coming months, we will be researching slippage loss with Zap Labs with the goal of identifying mechanisms to alleviate the growing burden of slippage loss on crypto traders.

We have begun by creating a program that models DEXs as minimum slippage networks. Each node in the network is a token and each edge represents a token pair that traders can exchange coins between. The size of the edges are proportional to the minimum slippage path from any source token to all possible destination tokens. The source token is Ethereum, and the ecosystem is Uniswap for the network visualization included in this post. Trader's who would like to exchange Ethereum for any token on Uniswap could use this network model to find the path to their desired token that will minimize their slippage. The size of the node is proportional to its degree - how many other tokens it's connected to. Unsurprisingly, the big node in the middle is Ethereum, which is connected to over 300 other tokens (out of 1800+ total). The tokens with the next highest degrees are USDC (45) and DAI (42). Knowing the degree of the node is important to slippage loss, since the more connections a node has, the easier it is to be able to reach other tokens without incurring too much slippage. The algorithm can also be used on other DEXs such as PancakeSwap, SushiSwap, and MDEX and measure the minimum slippage paths from other tokens such as Chainlink, Polygon and Shiba Inu.

HOW THE CODE WORKS

1. The main function calls returnTokensandPrice() from the collect and clean file to compile a list of tokens (custom class), which each have corresponding liquidity pools with reserve amounts. This data was pulled using queries from the uniswap graphql api. To run the algorithm on other DEXs, the collect and cleaner must be changed to the desired DEX in addition to a few other edits to functions (which will be described later).  
2. The main function then prompts the user to input the preferred mode. There is one mode for just slippage loss (1) and another for slippage loss and transaction fees (2). In the future, we will also build in gas costs.
3.  The main function then prompts the user to enter their desired transaction amount.
4.  After selecting the mode and transaction amount, the algorithm is ran and prints a list of the minimum slippage fees to reach every token on the DEX (Uniswap in this case). In addition, it returns a csv file with the data to the user.
5. Finally, a visualization is generated using the networkx package in python.
