from Classes import Order
from helpers import create_samples, dense_order_book_generator, dense_edge_generator
from NetIntegratedClasses import loss_function
import random
import matplotlib.pyplot as plt
import copy

#population[i] = org
#population[i][j] = org[j] = edge or score
#population[i][j][k] = org[j][k] = edge[k] = token or allocation data

losses = []

def initPopulation(support_edges, total_liquidity, pop_size):
    population = create_samples(support_edges, total_liquidity, pop_size)
    return population

def scorePopulation(population):
    scored_population = []
    for i in range(len(population)):
        scored_population.append(population[i] + [loss_function(population[i], order_book)])
    return copy.deepcopy(scored_population)

def mutate(allocation, total_liquidity, support_edges):
    e1 = random.randint(0,len(support_edges)-1)
    e2 = random.randint(0,len(support_edges)-1)
    mut_amount = total_liquidity/(len(support_edges)*50)#*generation*4)
    if (allocation[e1][2] - mut_amount <= 0):
        temp = allocation[e1][2]
        allocation[e1][2] = 0
        allocation[e2][2] += temp
    else:
        allocation[e1][2] -= mut_amount
        allocation[e2][2] += mut_amount
    return copy.deepcopy(allocation)

def selection(population,  total_liquidity, support_edges):
    #score fitness
    population = list(scorePopulation(population))
    
    #sort the population by fitness
    population.sort(key= lambda allocation: allocation[len(support_edges)])
    #sum fitness and add it to plot
    global losses
    tot = 0
    for i in range(len(population)):
        tot += population[i][len(support_edges)]
    losses.append(tot/len(population))

   

    new_population = [list(org[:len(support_edges)]) for org in list(population[:20])]

    for org in population[:25]:
        for i in range(4): 
            new_population.append(list(mutate(list(org[:len(support_edges)]), total_liquidity, support_edges)))

    return copy.deepcopy(new_population)

#TODO: Finish
def evolve(population, mutate_prob, total_liquidity, num_generations, support_edges):
    for i in range(num_generations):
        population = selection(population, total_liquidity, support_edges)
        new_population = population[:5]
        for org in population[5:]:
            if (random.uniform(0, 1) <= mutate_prob):
                new_population.append(mutate(org, total_liquidity, support_edges))
    return copy.deepcopy(new_population)


#edges = [["1", "2"], ["1", "3"], ["1", "4"], ["1", "5"], ["1", "6"], ["2", "3"], ["2", "4"], ["2", "5"], ["2", "6"], ["3", "4"], ["3", "5"], ["3", "6"], ["4", "5"], ["4", "6"], ["5", "6"]]
#order_book = [Order(100, "1", "2"), Order(100, "1", "3"), Order(100, "1", "4"), Order(100, "1", "5"), Order(100, "1", "6"), Order(100, "2", "3"), Order(100, "2", "4"), Order(100, "2", "5"), Order(100, "2", "6"), Order(100, "3", "4"), Order(100, "3", "5"), Order(100, "3", "6"), Order(100, "4", "5"), Order(100, "4", "6"), Order(100, "5", "6")]

edges = dense_edge_generator(7)
order_book = dense_order_book_generator(edges)
total_liquidity = 10000
pop_size = 100
num_gens = 100
mut_prob = .2

population = initPopulation(edges, total_liquidity, pop_size)
final_population = evolve(population, .2, total_liquidity, num_gens, edges)
print(final_population[0])

plt.scatter(list(range(num_gens)), losses)
plt.xlabel("# of Generations")
plt.ylabel("$ Fees (slippage loss + gas fees + transaction fees)")
plt.title("Performance Evolutionary Alg. for Optimal Liquidity Allocation (7 tokens)")
plt.show()





