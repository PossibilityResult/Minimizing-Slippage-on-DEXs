//
//  simulatedAnnealing.cpp
//  simulated annealing
//
//  Created by Elijah Fox on 8/3/21.
//  Copyright © 2021 Elijah Fox. All rights reserved.
//

#include "modDij.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

class Annealing {
public:
    Annealing();
    Annealing(uint16_t V);
    void randomLine(uint16_t numTokens);
    void randomSupport(uint16_t numTokens);
    
    vector<pair<size_t, double> > scoring(vector<Graph> &population, vector<Order> orderbook);
    void T();
    vector<Graph> selection(vector<Graph> &population, vector<Order> orderbook, double totalLiquidity, int numOrgs);
    
    double temperature;
    uint16_t numTokens;
    vector<pair<uint16_t, uint16_t> > support;
    vector<pair<uint16_t, uint16_t> > out;
    

    
};


Annealing::Annealing(uint16_t V) {
    numTokens = V;
    randomSupport(numTokens);
}

void Annealing::randomLine(uint16_t numTokens) {
    vector<uint16_t> vec(numTokens);
    
    for (uint16_t i = 0; i < numTokens; ++i) {
        vec[i] = i;
    }
    
    random_shuffle(vec.begin(), vec.end());
    
    for (uint16_t i = 0; i < numTokens - 1; ++i) {
        support.push_back(make_pair(vec[i], vec[i + 1]));
    }
    
    for (uint16_t i = 0; i < numTokens - 1; ++i) {
        for (uint16_t j = i + 1; j < numTokens; ++j) {
            if (vec[j] != vec[i+1] and i != j) {
                out.push_back(make_pair(i, j));
            }
        }
    }
    random_shuffle(out.begin(), out.end());
}


void Annealing::randomSupport(uint16_t numTokens) {
    std::mt19937 rng(numTokens);
    std::uniform_int_distribution<int> gen(numTokens, maxEdges(numTokens)); // uniform, unbiased
    int r = gen(rng);
    
    randomLine(numTokens);
    
    for (int i = 0; i < r; ++i) {
        support.push_back(make_pair(out.back().first, out.back().second));
        out.pop_back();
    }
}

vector<pair<size_t, double> > Annealing::scoring(vector<Graph> &population, vector<Order> orderbook) {
    double totalLoss = 0;
    
    vector<pair<size_t, double> > lookup(population.size());
    for (size_t s = 0; s < population.size(); ++s) {
        lookup[s].first = s;
        lookup[s].second = population[s].lossFunction(orderbook);
        totalLoss += lookup[s].second;
    }
    double averageLoss = totalLoss/static_cast<uint16_t>(population.size());
    cout << "Average Total Cost: " << averageLoss << "\n";
    //sort(population.begin(), population.end(), comp());
    sort(lookup.begin(), lookup.end(), comp2());
    temperature = population[lookup[0].first].costs;
    cout << "Lowest Cost: " << population[lookup[0].first].costs << "\n\n";
    return lookup;
}

vector<Graph> Annealing::selection(vector<Graph> &population, vector<Order> orderbook, double totalLiquidity, int numOrgs) {
    vector<pair<size_t, double> > scoredLookup = scoring(population, orderbook);
    vector<Graph> newPopulation;
    //TODO: DO NOT HARDCODE 4LOOPS USE NUMORGS FOR LOOPING AMOUNTS
    for (size_t s = 0; s < (population.size() /10); ++s) {
        newPopulation.push_back(Graph(population[scoredLookup[s].first]));
    }
    for (size_t s = 0; s < (population.size() * 9 / 20); ++s) {
        newPopulation.push_back(mutate(population[scoredLookup[s].first], totalLiquidity));
        newPopulation.push_back(mutate(population[scoredLookup[s].first], totalLiquidity));
    }
    return newPopulation;
}

// Driver program to test methods of graph class
void Annealing::T() {
    double totalLiquidity = 10000;
    uint16_t numOrgs = 100;
    uint16_t numGens = 500;
    
    vector<Order> orderBook = initOrderBookDENSE(numTokens);
    
    vector<Graph> population = initPopulationSupport(numTokens, numOrgs, totalLiquidity, support);
    
    for (uint16_t i = 0; i < numGens; ++i) {
        cout << "Generation " << i << "\n";
        population = selection(population, orderBook, totalLiquidity, numOrgs);
    }
}


int main() {
    uint16_t numTokens = 8;
    Annealing anneal(numTokens);
    anneal.T();
    
    
    cout << anneal.temperature << "\n";
    
    return 0;
}