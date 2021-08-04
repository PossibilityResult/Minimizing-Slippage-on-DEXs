//
//  simulatedAnnealing.cpp
//  simulated annealing
//
//  Created by Elijah Fox on 8/3/21.
//  Copyright © 2021 Elijah Fox. All rights reserved.
//

#include "modifiedDijkstra.cpp"
#include <iostream>     // std::cout
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

class Annealing {
public:
    void randomLine(uint16_t numTokens);
    void randomSupport(uint16_t numTokens);
    
    vector<pair<uint16_t, uint16_t> > support;
};

void Annealing::randomLine(uint16_t numTokens) {
    vector<uint16_t> vec(numTokens);
    
    for (uint16_t i = 0; i < numTokens; ++i) {
        vec[i] = i;
    }
    
    random_shuffle(vec.begin(), vec.end());
        
    for (uint16_t i = 0; i < numTokens - 1; ++i) {
        support[i].first = vec[i];
        support[i].second = vec[i + 1];
    }
}


void Annealing::randomSupport(uint16_t numTokens) {
    std::mt19937 rng(numTokens);
    std::uniform_int_distribution<int> gen(numTokens, maxEdges(numTokens)); // uniform, unbiased
    int r = gen(rng);
    
    randomLine(numTokens);
    
    vector<Order> initOrderBookDENSE(uint16_t numTokens);
    
    for (int i = 0; i < r; ++i) {
        
    }
}



int main() {
    Annealing anneal;
    
    uint16_t numTokens = 8;
    anneal.randomSupport(numTokens);
    
    double t = T();
    
    return 0;
}