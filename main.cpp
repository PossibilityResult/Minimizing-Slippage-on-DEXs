//
//  pqdij.cpp
//  dijkstratest
//
//  Created by Elijah Fox on 7/26/21.
//  Copyright © 2021 Elijah Fox. All rights reserved.
//
// Program to find Dijkstra's shortest path using
// priority_queue in STL
// C / C++ program for Dijkstra's
// shortest path algorithm for adjacency
// list representation of graph
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <limits>
#include <vector>
#include <random>
#include <algorithm>
#include <list>
#include <functional>
#include <queue>

using namespace std;
# define INF 0x3f3f3f3f

// iPair ==> Integer Pair
typedef pair<uint16_t, double> iPair;

struct Order {
    uint16_t src;
    uint16_t target;
    double amount;
};

// This class represents a directed graph using
// adjacency list representation
class Graph
{
public:
    uint16_t V; // No. of vertices

    // In a weighted graph, we need to store vertex
    // and weight pair for every edge
    list<pair <uint16_t, double> > *adj;
    
    vector< pair< pair<uint16_t, double>*, pair<uint16_t, double>* > > edgeList;
    
    double costs;

    Graph(uint16_t V); // Constructor
    
    Graph(const Graph &g); // copy constructor
    
    ~Graph(); //destructor

    // function to add an edge to graph
    void addEdge(uint16_t u, uint16_t v, double w);

    // prints shortest path from s
    double shortestPath(uint16_t s, uint16_t t, double exchange_amount);
    
    //generates a random allocation of liquidity
    void randomAllocation(double totalLiquidity, uint16_t numTokens);
    
    //calculates total loss (gas + transaction fees + slippage) given an orderbook
    double lossFunction(vector<Order> orderBook);
};

// Allocates memory for adjacency list
Graph::Graph(uint16_t V) {
    this->V = V;
    adj = new list<iPair> [V];
}

//TODO: FINISH COPY CONSTRUCTOR
Graph::Graph(const Graph &g) {
    this->V = g.V;
    adj = new list<iPair> [V];
    
    if (g.edgeList.size() > 0) {
        for (uint16_t i = 0; i < g.edgeList.size(); ++i){
            this->addEdge(g.edgeList[i].first->first, g.edgeList[i].second->first, g.edgeList[i].first->second);
        }
    }
}

Graph::~Graph() {
    delete[] adj;
}

void Graph::addEdge(uint16_t u, uint16_t v, double w) {
    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w));
    
    edgeList.push_back(make_pair(&adj[u].back(), &adj[v].back()));
}

double calculateSlippage(double liquidity, double q1) {
    double r = liquidity/2;
    if (q1 >= r) {
        return INT_MAX;
    }
    return ( q1 - (r*q1)/(r+q1) );
}

// Prints shortest paths from src to all other vertices
double Graph::shortestPath(uint16_t src, uint16_t target, double exchange_amount)
{
    // Create a priority queue to store vertices that
    // are being preprocessed.
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq;

    // Create a vector for distances and initialize all
    // distances/slippage as infinite (INF)
    vector<double> dist(V, INF);
    vector<double> slip(V, INF);
    
    // Insert source itself in priority queue and initialize
    // its distance/slippage as 0.
    pq.push(make_pair(0, src));
    dist[src] = 0;
    slip[src] = 0;

    /* Looping till priority queue becomes empty (or all
    distances are not finalized) */
    while (!pq.empty())
    {
        // The first vertex in pair is the minimum distance
        // vertex, extract it from priority queue.
        // vertex label is stored in second of pair (it
        // has to be done this way to keep the vertices
        // sorted distance (distance must be first item
        // in pair)
        uint16_t u = pq.top().second;
        pq.pop();
        
        if (u == target) {
            return dist[target];
        }
    
        // 'i' is used to get all adjacent vertices of a vertex
        list< pair<uint16_t, double> >::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i)
        {
            // Get vertex label and weight of current adjacent
            // of u.
            uint16_t v = (*i).first;
            double weight = (*i).second;
            
            //calculates just slippage loss
            double s = calculateSlippage(weight, exchange_amount - slip[u]);
            //calculate distance (includes transaction fees and gas)
            double d = s + slip[u] + 10 + .003 * (exchange_amount - slip[u]);

            // If there is shorter path to v through u.
            if (dist[v] > dist[u] + weight)
            {
                // Updating distance of v
                dist[v] = dist[u] + d;
                slip[v] = slip[u] + s;
                pq.push(make_pair(dist[v], v));
            }
        }
    }
    return dist[target];
}

uint16_t maxEdges(uint16_t numTokens) {
    return numTokens*(numTokens-1)/2;
}

void Graph::randomAllocation(double totalLiquidity, uint16_t numTokens) {
    //creates an instance of an engine.
    random_device rnd_device;
    // Specify the engine and distribution.
    mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    
    uniform_int_distribution<double> dist {1, 52};
    
    auto gen = [&dist, &mersenne_engine](){
        return dist(mersenne_engine);
    };
    
    //calculates max number of edges for a graph with numTokens (n choose 2)
    uint16_t numEdges = maxEdges(numTokens);
    vector<double> vec(numEdges);
    
    generate(begin(vec), end(vec), gen);
    
    //finds the sum of the random vector to help normalize
    double div = accumulate(vec.begin(), vec.end(), 0);
    
    //divides the vector by scalar (div); this makes the sum = 1
    transform(vec.begin(), vec.end(), vec.begin(), [div](double &c){ return c/div; });
    
    //multiplies the vector by scalar (totalLiquidity); this makes sum of weights to totalLiquidity
    transform(vec.begin(), vec.end(), vec.begin(), [totalLiquidity](double &c){ return c*totalLiquidity; });
    
    uint16_t index = 0;
    
    for (uint16_t i = 0; i < numTokens - 1; ++i) {
        for (uint16_t j = i + 1; j < numTokens; ++j){
            addEdge(i, j, vec[index]);
            ++index;
        }
    }
}


double Graph::lossFunction(vector<Order> orderBook) {
    double loss = 0;
    for (size_t s = 0; s < orderBook.size(); ++s) {
       loss += shortestPath(orderBook[s].src, orderBook[s].target, orderBook[s].amount);
    }
    costs = loss;
    return loss;
}

//Returns a mutant graph
Graph mutate(Graph &g, double totalLiquidity){
    Graph mutant = Graph(g);
    uint16_t numEdges = static_cast<uint16_t>(mutant.edgeList.size());
    
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<uint16_t> uni(0,numEdges - 1); // guaranteed unbiased
    
    //TODO: FIX BUG W/ MUTATION AND HIGH LOSS
    for (uint8_t i = 0; i < 20; i++) {
    
        uint16_t i1 = uni(rng);
        uint16_t i2 = uni(rng);

        double mut_amount = totalLiquidity/(numEdges*50);
        double temp;
    
        pair< pair<uint16_t, double>*, pair<uint16_t,double>* >* e1 = &mutant.edgeList[i1];
        pair< pair<uint16_t,double>*, pair<uint16_t,double>* >* e2 = &mutant.edgeList[i2];
        if (e1->first->second - mut_amount <= 0){
            temp = e1->first->second;
            e1->first->second = 0;
            e1->second->second = 0;

            e2->first->second += temp;
            e2->second->second += temp;
        }
        else {
            e1->first->second -= mut_amount;
            e1->second->second -= mut_amount;
        
            e2->first->second += mut_amount;
            e2->second->second += mut_amount;
        }
    }
    return mutant;
}

vector<Graph> initPopulation(uint16_t numTokens, uint16_t numOrgs, double totalLiquidity) {
    vector<Graph> population;
    population.resize(numOrgs, Graph(numTokens));
    
    for (uint16_t s = 0; s < numOrgs; ++s) {
        population[s].randomAllocation(totalLiquidity, numTokens);
    }
    return population;
}

struct comp2 {
    bool operator()(pair<size_t, double> a, pair<size_t, double> b) {
        return a.second < b.second;
    }
};

vector<pair<size_t, double> > scoring(vector<Graph> &population, vector<Order> orderbook) {
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
    cout << "Lowest Cost: " << population[lookup[0].first].costs << "\n\n";
    return lookup;
}

vector<Graph> selection(vector<Graph> &population, vector<Order> orderbook, double totalLiquidity, int numOrgs) {
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

vector<Order> initOrderBookDENSE(uint16_t numTokens) {
    vector<Order> orderbook;
    uint16_t index = 0;
    for (uint16_t i = 0; i < numTokens - 1; ++i) {
        for (uint16_t j = i + 1; j < numTokens; ++j) {
            orderbook.push_back(Order{i, j, 50});
            index++;
        }
    }
    return orderbook;
}

// Driver program to test methods of graph class
int main() {
    uint16_t numTokens = 100;
    double totalLiquidity = 10000000;
    uint16_t numOrgs = 100;
    uint16_t numGens = 100;
    
    vector<Order> orderBook = initOrderBookDENSE(numTokens);
    
    vector<Graph> population = initPopulation(numTokens, numOrgs, totalLiquidity);
    
    for (uint16_t i = 0; i < numGens; ++i) {
        cout << "Generation " << i << "\n";
        population = selection(population, orderBook, totalLiquidity, numOrgs);
    }
    return 0;
}



