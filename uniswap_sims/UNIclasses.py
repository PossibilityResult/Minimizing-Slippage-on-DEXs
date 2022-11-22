#Vertices/Nodes
class token:
    def __init__(self, price, ticker):
        #TODO: Price not used now but may be in future, make it a dictionary with a diff value for each pair
        self.price = price
        self.ticker = ticker
        #list of pairings; forms an "edge list" when coins are in a list
        #CHANGE TO DICTIONARY
        self.pools = {}

    def convertDollar(self, amount):
        return self.price * amount
    
    def slippageLossNormalized(self, pool_index):
        return self.convertDollar(self.pools[pool_index])

#edges/links
class pool:
    def __init__(self, total_liquidity, ticker_price_dict, ticker1, ticker2):
        #TODO: consider updating all ticker to name since it was changed in the query
        self.ticker1 = ticker1
        self.ticker2 = ticker2
        #TODO: check if these equations are right: commented out = multiplication for total liquidity
        self.R1_dollars = total_liquidity/2
        self.R2_dollars = total_liquidity/2
        #TODO: incorrect for now until price is fixed; see price member variable in init for more
        #self.R1_tokens = self.R1_dollars/ticker_price_dict[self.ticker1]
        #self.R2_tokens = self.R2_dollars/ticker_price_dict[self.ticker2]

    #Calculates slippage lost
    #NetworkX can use class variables as edges
    #Inputs: R1, R2 (pool status variables) - make custom class variable; q1: the quantity of coins being "deposited/exchanged"
    #Outputs: slippage lost
    def calcSlippageLoss(self, q1):
        R1 = self.R1_dollars
        R2 = self.R2_dollars

        q2_prime = (R2*q1)/R1
        q2 = (R2*q1)/(R1+q1)

        return q2_prime - q2

    #updates slippage - removed for now since no need to update reserves atm; may be used for simulation
    #def updateSlippageLoss(self, q1):
        #R1 = self.R1
        #R2 = self.R2

        #q2_prime = (R2*q1)/R1
        #q2 = (R2*q1)/(R1+q1)

        #self.R1 += q1
        #self.R2 -= q2
        #return q2_prime - q2