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
#include <math.h>
#include <fstream>

class Annealing {
public:
    Annealing();
    //Random first init
    Annealing(uint16_t V);
    //Edit constructor
    Annealing(Annealing &A, bool edit);
    void randomLine(uint16_t numTokens);
    void randomSupport(uint16_t numTokens);
    
    //pair<uint16_t, uint16_t> zeroEdge;
    
    vector<pair<size_t, double> > scoring(vector<Graph> &population, vector<Order> orderbook);
    Graph E();
    vector<Graph> selection(vector<Graph> &population, vector<Order> orderbook, double totalLiquidity, int numOrgs);
    
    double energy;
    uint16_t numTokens;
    vector<pair<uint16_t, uint16_t> > support;
    vector<pair<uint16_t, uint16_t> > out;
};

Annealing::Annealing() {
    numTokens = 0;
    energy = 0;
}

Annealing::Annealing(uint16_t V) {
    numTokens = V;
    randomSupport(numTokens);
}

Annealing::Annealing(Annealing &A, bool edit) {
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<uint16_t> uni(0, 2); // guaranteed unbiased
    
    numTokens = A.numTokens;
    support.assign(A.support.begin(), A.support.end());
    out.assign(A.out.begin(), A.out.end());
    energy = A.energy;
    
    bool validGraph = false;
    
    while(!validGraph and edit) {
        uint16_t e = uni(rng);
        /*if (A.zeroEdge.first != 0 or A.zeroEdge.second != 0) {
            for (size_t s = 0; s < support.size(); ++s) {
                if ((A.zeroEdge.first == support[s].first or A.zeroEdge.first == support[s].second) and (A.zeroEdge.second == support[s].first or A.zeroEdge.second == support[s].second)) {
                    support.erase(support.begin() + s);
                }
            }
        }*/
        if (e == 0 and out.size() > 0) {
        //remove edge
            support.push_back(out.back());
            out.pop_back();
        }
        else if (e == 1 and out.size() > 1 and support.size() > numTokens - 2) {
        //swap edge
            support.push_back(out.back());
            out.pop_back();
            random_shuffle(support.begin(), support.end());
            out.push_back(support.back());
            support.pop_back();
            
        }
        else if (e == 2 and support.size() > numTokens - 2){
        //add edge
            random_shuffle(support.begin(), support.end());
            out.push_back(support.back());
            support.pop_back();
        }
        Graph g = Graph(support, numTokens);
        validGraph = g.connectedComponents();
    }
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
                out.push_back(make_pair(vec[i], vec[j]));
            }
        }
    }
    random_shuffle(out.begin(), out.end());
}


void Annealing::randomSupport(uint16_t numTokens) {
    std::mt19937 rng(numTokens);
    std::uniform_int_distribution<int> gen(0, out.size()); // uniform, unbiased
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
    //cout << "Average Total Cost: " << averageLoss << "\n";
    //sort(population.begin(), population.end(), comp());
    sort(lookup.begin(), lookup.end(), comp2());
    energy = population[lookup[0].first].costs;
    //zeroEdge = make_pair(population[lookup[0].first].zeroEdge.first, population[lookup[0].first].zeroEdge.second);
    //cout << "Lowest Cost: " << population[lookup[0].first].costs << "\n\n";
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
Graph Annealing::E() {
    double totalLiquidity = 100000000;
    uint16_t numOrgs = 100;
    uint16_t numGens = 100;
    
    vector<Order> orderBook = initOrderBookDENSE(numTokens);
    
    vector<Graph> population = initPopulationSupport(numTokens, numOrgs, totalLiquidity, support);
    
    for (uint16_t i = 0; i < numGens; ++i) {
        //cout << "Generation " << i << "\n";
        population = selection(population, orderBook, totalLiquidity, numOrgs);
    }
    return population[scoring(population, orderBook)[0].first];
    //TODO: SORT POPULATION AND RETURN THE BEST GRAPH
}

double temperature(uint16_t k, uint16_t kmax) {
    return (1 - (k+1)/kmax);
}

double probability (double energyPrev, double energyCurr, double temperature) {
    if (energyPrev > energyCurr) {
        return 1;
    }
    else {
        return exp(- (energyCurr - energyPrev)/temperature);
    }
    return 0; //number between 0 and 1
}


int main() {
    std::ofstream myfile;
    myfile.open ("edges.csv");
    myfile << "\n";
    
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    
    double prob;
    
    uint16_t numTokens = 15;
    uint16_t kmax = 1000;
    double p;
    double temp;
    bool edit = false;
    uint16_t b = 0;
    
    Annealing prior = Annealing(numTokens);
    prior.E();
    Annealing best = Annealing(prior, edit);
    
    for (uint16_t k = 0; k < kmax; ++k) {
        //p = uni(rng);
        edit = true;
        Annealing current = Annealing(prior, edit);
        
        Graph g = prior.E();
        cout << "\n\n";
        cout << "ROUND " << k << "\n";
        cout << "Current energy : " << prior.energy << "\n";
        
        temp = temperature(k, kmax);
        p = probability (prior.energy, current.energy, temp);
        prob = unif(rng);
        myfile << prior.energy  << " ";
        myfile << best.energy  << " ";
        for (size_t i = 0; i < g.edgeList.size(); ++i) {
            myfile << g.edgeList[i].first->first  << " " << g.edgeList[i].second->first << " " << g.edgeList[i].first->second << " ";
        }
        myfile << "\n";
        if (p > prob) {
            edit = false;
            prior = Annealing(current, edit);
        
            
            if (current.energy < best.energy) {
                best = Annealing(current, edit);
                b = k;
                cout << "CURRENT IS NEW BEST!\n";
            }
        }
        cout << "Best energy : " << best.energy << "\n\n";
    }
    return 0;
}