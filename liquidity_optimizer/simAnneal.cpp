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
#include <time.h>

struct edge {
    uint16_t sourceToken;
    uint16_t targetToken;
    double liquidity;
};

class Annealing {
public:
    Annealing();
    //Random first init
    Annealing(uint16_t V);
    //Edit constructor
    Annealing(Annealing &A, bool edit);
    void randomLine(uint16_t numTokens);
    void randomSupport(uint16_t numTokens);
    vector<pair<size_t, double> > scoredLookup;
    
    //pair<uint16_t, uint16_t> zeroEdge;
    
    vector<pair<size_t, double> > scoring(vector<Graph> &population, vector<Order> orderbook);
    vector<edge> E(vector<Order> orderBook);
    vector<Graph> selection(vector<Graph> &population, vector<Order> orderbook, int numOrgs);
    
    double totalLiquidity;
    double energy;
    Costs fees;
    uint16_t numTokens;
    vector<pair<uint16_t, uint16_t> > support;
    vector<pair<uint16_t, uint16_t> > out;
};

Annealing::Annealing() {
    numTokens = 0;
    energy = 0;
    totalLiquidity = 0;
}

Annealing::Annealing(uint16_t V) {
    numTokens = V;
    totalLiquidity = 0;
    //randomSupport(V);
}

Annealing::Annealing(Annealing &A, bool edit) {
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<uint16_t> uni(0, 2); // guaranteed unbiased
    
    numTokens = A.numTokens;
    support.assign(A.support.begin(), A.support.end());
    out.assign(A.out.begin(), A.out.end());
    energy = A.energy;
    totalLiquidity = A.totalLiquidity;
    fees = A.fees;
    
    bool validGraph = false;
    
    while(!validGraph and edit) {
        uint16_t e = uni(rng);
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
    vector<pair<size_t, double> > lookup(population.size());
    for (size_t s = 0; s < population.size(); ++s) {
        lookup[s].first = s;
        Costs temp = population[s].lossFunction(orderbook);
        lookup[s].second = temp.dist;
    }
    
    sort(lookup.begin(), lookup.end(), comp2());
    energy = population[lookup[0].first].costs;
    fees.dist = population[lookup[0].first].costs;
    fees.slip = population[lookup[0].first].slip;
    fees.gas = population[lookup[0].first].gas;
    fees.trans = population[lookup[0].first].trans;
    return lookup;
}

vector<Graph> Annealing::selection(vector<Graph> &population, vector<Order> orderbook, int numOrgs) {
    scoredLookup = scoring(population, orderbook);
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
vector<edge> Annealing::E(vector<Order> orderBook) {
    uint16_t numOrgs = 150;
    uint16_t numGens = 150;
    
    //vector<Order> orderBook = initOrderBookDENSE(numTokens);
    
    vector<Graph> population = initPopulationSupport(numTokens, numOrgs, totalLiquidity, support);
    
    for (uint16_t i = 0; i < numGens; ++i) {
        //cout << "Generation " << i << "\n";
        population = selection(population, orderBook, numOrgs);
    }
    vector<edge> edges;
    
    for (size_t s = 0; s < population[scoredLookup[0].first].edgeList.size(); ++s) {
        uint16_t source = population[scoredLookup[0].first].edgeList[s].first->first;
        uint16_t target = population[scoredLookup[0].first].edgeList[s].second->first;
        double liquid = population[scoredLookup[0].first].edgeList[s].first->second;
        edges.push_back({source, target, liquid});
    }
    return edges;
}

double temperature(uint16_t k, uint16_t kmax) {
    return (1 - (k+1)/kmax);
}

double probability (double energyPrev, double energyCurr, double temperature) {
    if (energyPrev > energyCurr) {
        return 1;
    }
    else {
        return exp(- (energyCurr - energyPrev)/(temperature*90));
    }
    return 0; //number between 0 and 1
}

int main() {
    ifstream file("/Users/elijahfox/Desktop/edges_data.txt");
    
    if (!file.is_open())
    {
        cout << "Path Wrong!!!!" << endl;
    }
    
    int node1;
    int node2;
    double liquidity;
    Graph g(10);
    string line;
    //char dud;
    
    uint16_t numTokens = 10;
    uint16_t kmax = 300;
    double p;
    double temp;
    bool edit = false;
    uint16_t b = 0;
    
    Annealing prior = Annealing(numTokens);
    
    vector<vector<uint16_t> > adjmat;
    
    vector<uint16_t> vec(numTokens);
    
    for (uint16_t i = 0; i < numTokens; ++i) {
        vec[i] = i;
    }
    int index = 0;
    for (uint16_t i = 0; i < numTokens; i++) {
        for (uint16_t j = i; j < numTokens; ++j) {
            if (i != j) {
                prior.out.push_back(make_pair(vec[i], vec[j]));
                index++;
            }
        }
    }
    
    getline(file, line);
    while(getline(file, line, ',')) {
        getline(file, line, ',');
        node1 = stoi(line);
        getline(file, line, ',');
        node2 = stoi(line);
        getline(file, line);
        liquidity = stod(line);
        
        //TODO: DEBUG AND FIGURE OUT
        if (node1 != 11) {
            size_t s = prior.out.size();
            for (size_t x = 0; x < s; ++x){
                if ((prior.out[x].first == node1 and prior.out[x].second == node2)
                    or (prior.out[x].second == node1 and prior.out[x].first == node2)){
                    prior.support.push_back(make_pair(node1, node2));
                    prior.out.erase(prior.out.begin() + x);
                    g.addEdge(node1, node2, liquidity);
                    
                }
            }

        }
        else {
            prior.totalLiquidity = liquidity;
        }
    }
    std::ofstream myfile;
    myfile.open ("edges.csv");
    myfile << "\n";
    
    std::ofstream costFile;
    costFile.open ("costs.csv");
    costFile << "\n";
    
    typedef std::numeric_limits< double > dbl;
    cout.precision(dbl::max_digits10);
    myfile.precision(dbl::max_digits10);
    costFile.precision(dbl::max_digits10);
    
    vector<Order> ob = initOrderBookCSV();
    g.lossFunction(ob);
    
    cout << "SushiSwap Loss: " << g.costs << "\n";
    cout << "SushiSwap Slippage: " << g.slip << "\n";
    cout << "SushiSwap Transaction Fees: " << g.trans << "\n";
    cout << "SushiSwap Gas Fees: " << g.gas << "\n\n";
    
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    
    srand( (unsigned)time( NULL ) );
    
    double prob;

    
    vector<edge> edges = prior.E(ob);
    //vector< pair< pair<uint16_t, double>*, pair<uint16_t, double>* > > out = g.edgeList;
    Annealing best = Annealing(prior, edit);
    costFile << best.fees.dist  << " " << best.fees.slip << " " << best.fees.trans << best.fees.gas << "\n";
    
    for (uint16_t k = 0; k < kmax; ++k) {
        //p = uni(rng);
        edit = true;
        Annealing current = Annealing(prior, edit);
        vector<edge> tempEdges = current.E(ob);
        
        cout << "\n\n";
        cout << "ROUND " << k << "\n";
        cout << "Prior energy : " << prior.energy << "\n";
        cout << "Current energy : " << current.energy << "\n";
        cout << "Best energy : " << best.energy << "\n";
        
        myfile << prior.energy  << " ";
        myfile << best.energy  << " ";
        for (size_t i = 0; i < edges.size(); ++i) {
            myfile << edges[i].sourceToken  << " " << edges[i].targetToken << " " << edges[i].liquidity << " ";
        }
         myfile << "\n";
        
        temp = temperature(k, kmax);
        p = probability (prior.energy, current.energy, temp);
        prob =  unif(rng);//unif(rng);
        
        if (p > prob) {
            edit = false;
            prior = Annealing(current, edit);
            edges = tempEdges;
            
            if (current.energy < best.energy) {
                best = Annealing(current, edit);
                b = k;
                costFile << best.fees.dist  << " " << best.fees.slip << " " << best.fees.trans << best.fees.gas << "\n";
                cout << "CURRENT IS NEW BEST!\n\n";
            }
        }
    }
    return 0;
}

}