from UNIclasses import token, pool
import csv

#tokens: list of lists, tokens[i].pools[j] = reserve pool i, j pair, where i = a token node and j = a token i is paired with
#source_coin: integer 0 -> n-1
#dist[u]: slippage loss to reach coin u in dollar terms; 

#Utility function to convert distance dict to csv
def solution_to_csv(dist, tokens):
    solution_csv = {}
    for tok in tokens:
        solution_csv[tok] = dist[tok]
    with open('uni_SL_solutions.csv', 'w') as f:
        for key in dist:
            f.write("%s,%s\n"%(key, dist[key]))
            
#A utility function to print the constructed distance array
def printSolution(dist, tokens):
    print("Token \t\t Maximum Dollars Left When Reached\n")
    for tok in tokens:
        print("{} \t\t {}\n".format(tok, dist[tok]))

#INPUT: dict of distances where dist[i] = the minimum distance from the source_coin to the ith coin
#OUTPUT: returns the vertex with the m
def minDistance(dist, spt_set):
    minimum = float('inf')
    for key in dist.keys():
      if spt_set[key] == False and dist[key] < minimum:
        minimum = dist[key]
        min_ticker = key
    return min_ticker



def UNIdjSL(tokens, source_coin):
    #Creates a set (dict?) of bools that keeps track of vertices included in shortest path tree, i.e., whose minimum distance from source is calculated and finalized. Initialize to empty. 
    spt_set = {}
    #Create a list of distances (output array)
    # dist[i]: the minimum distance from source_coin to the ith coin
    dist = {}

    #Holds the weights for the network visualization; returned
    weights = {}
    
    #Initialize the distances to infinity and sets to 
    for tok in tokens.items():
        spt_set[tok[1].ticker] = False
        dist[tok[1].ticker] = float('inf')
    
    #Assign distance value as 0 for the source vertex so that it is picked first. 
    dist[source_coin] = 0

    u = ""

    #Find the shortest path for all vertices
    while(u != 'Elena Protocol'):
        #Pick the minimum distance vertex from the set of vertices not yet processed
        #u = src in the first iteration
        u = minDistance(dist, spt_set)

        #if (i > 1806): #UNISWAP ISLAND DEBUG
            #for i in spt_set:
                #if not spt_set[i]:
                    #print(i)

        #Mark the picked vertex as processed (set it equal to true)
        spt_set[u] = True
        
        starting_USD = 10000

        #Update the distance value of the adjacent vertices of the picked vertex
        for tok in tokens.items():
            #update dist[v] only if it:
            #1) not in spt_set
            #2) there is an edge from u to v
            #3) total weight of path from source_coin to v through u is smaller than current value of dist[v]
            if (spt_set[tok[1].ticker] == False
            and tok[1].ticker in tokens[u].pools
            and dist[tok[1].ticker] > dist[u] + tokens[u].pools[tok[1].ticker].calcSlippageLoss(starting_USD - dist[u])):
                dist[tok[1].ticker] = dist[u] + tokens[u].pools[tok[1].ticker].calcSlippageLoss(starting_USD - dist[u])
                weights[(tokens[u].ticker, tok[1].ticker)] = tokens[u].pools[tok[1].ticker].calcSlippageLoss(starting_USD - dist[u])
    
    #Call utility function to sace as csv and then print solution
    solution_to_csv(dist, tokens)
    printSolution(dist, tokens)
    return weights