from Classes import Order
from helpers import create_samples
from NetIntegratedClasses import loss_function
import random

edges = [["1", "2"], ["2", "3"], ["1", "3"]]
order_book = [Order(100, "1", "2"), Order(100, "2", "3"), Order(100, "3", "1")]

def initPopulation(support_edges, total_liquidity, pop_size):
    population = create_samples(edges, total_liquidity, pop_size)
    return population

def scorePopulation(population):
    for i in range(len(population)):
        population[i].append(loss_function(population[i], order_book))
    return population

def mutate(allocation, total_liquidity):
    e1 = random.randint(0,3)
    e2 = random.randint(0,3)
    mut_amount = total_liquidity/(len(allocation)*50)#*generation*4)
    if (allocation[e1][2] - mut_amount <= 0):
        temp = allocation[e1][2]
        allocation[e1][2] = 0
        allocation[e2][2] += temp
    else:
        allocation[e1][2] -= mut_amount
        allocation[e2][2] += mut_amount
    return allocation

def selection(population,  total_liquidity):
    population = scorePopulation(population)
    population.sort(key= lambda allocation: allocation[3])
    new_population = population[:50]

    for org in population[:50]: 
        new_population.append(mutate(org, total_liquidity))

    return new_population

#TODO: Finish
def evolve(population, mutate_prob, total_liquidity):
    new_population = selection(population,  total_liquidity)
    for org in population:
        if (random.uniform(0, 1) <= mutate_prob):
            org = mutate(org, total_liquidity)
    return new_population



        


