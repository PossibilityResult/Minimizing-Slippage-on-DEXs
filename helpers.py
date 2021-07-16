import networkx as nx
from Classes import Path, Order, Orderbook
from NetIntegratedClasses import loss_function
import numpy as np, numpy.random

#helper function that randomly allocates liquidity for edges 
def random_allocation_sampler(support_edges, total_liquidity):
    sample = []
    #normalizes random numbers to sum to total liquidity
    a = np.random.random(len(support_edges))
    a /= a.sum()
    a *= total_liquidity

    for i in range(len(support_edges)):
        #appends liquidity for edge[i] in the support
        sample += [support_edges[i] + [a[i]]]

    return sample

def create_samples(support_edges, total_liquidity, num_samples):
    #list of sample liquidity allocations
    samples = []
    for i in range(num_samples):
        for e in range(len(support_edges)):
            #print(len(list(random_allocation_sampler(support_edges, total_liquidity))))
            samples += [random_allocation_sampler(support_edges, total_liquidity)]

    return samples
    
#def get_nodes(order_book): for o in order_book:




    




