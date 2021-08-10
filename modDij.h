//
//  modDij.h
//  simulated annealing
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

//extern double bestAllocation;

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
    
    double totalLiq;
    
    vector< pair< pair<uint16_t, double>*, pair<uint16_t, double>* > > edgeList;
    
    double costs;
    
    //pair<uint16_t, uint16_t> zeroEdge;

    Graph(uint16_t V); // Constructor
    
    Graph(const Graph &g); // copy constructor
    
    Graph(vector<pair<uint16_t, uint16_t> > support, uint16_t numTokens); //support constructor
    
    ~Graph(); //destructor
    
    bool connectedComponents();
    
    void DFSUtil(uint16_t v, bool visited[]);

    // function to add an edge to graph
    void addEdge(uint16_t u, uint16_t v, double w);

    // prints shortest path from s
    double shortestPath(uint16_t s, uint16_t t, double exchange_amount);
    
    //generates a random allocation of liquidity
    void randomAllocationDENSE(double totalLiquidity, uint16_t numTokens);
    
    void randomAllocationSupport(double totalLiquidity, uint16_t numTokens, vector<pair<uint16_t, uint16_t> > support);
    
    //calculates total loss (gas + transaction fees + slippage) given an orderbook
    double lossFunction(vector<Order> orderBook);
    
};

// Allocates memory for adjacency list
Graph::Graph(uint16_t V) {
    this->V = V;
    adj = new list<iPair> [V];
    //zeroEdge = make_pair(0, 0);
}

Graph::Graph(const Graph &g) {
    this->V = g.V;
    adj = new list<iPair> [V];
    //zeroEdge = make_pair(g.zeroEdge.first,g.zeroEdge.second);
    
    if (g.edgeList.size() > 0) {
        for (uint16_t i = 0; i < g.edgeList.size(); ++i){
            this->addEdge(g.edgeList[i].first->first, g.edgeList[i].second->first, g.edgeList[i].first->second);
        }
    }
}

Graph::Graph(vector<pair<uint16_t, uint16_t> > support, uint16_t numTokens) {
    this->V = numTokens;
    adj = new list<iPair> [V];
    
    for (size_t i = 0; i < support.size(); ++i) {
        this->addEdge(support[i].first, support[i].second, 100);
    }
}

Graph::~Graph() {
    delete[] adj;
}


bool Graph::connectedComponents()
{
    bool connected = true;
    // Mark all the vertices as not visited
    bool* visited = new bool[V];
    for (uint16_t v = 0; v < V; v++)
        visited[v] = false;
 
    DFSUtil(0, visited);
    for (uint16_t v = 0; v < V; v++) {
        if (visited[v] == false) {
            // print all reachable vertices
            // from v
            connected = false;
            
            
        }
    }
    delete[] visited;
    return connected;
}
 
void Graph::DFSUtil(uint16_t v, bool visited[])
{
    // Mark the current node as visited and print it
    visited[v] = true;

 
    // Recur for all the vertices
    // adjacent to this vertex
    list<iPair>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i) {
        if (!visited[i->first]) {
            DFSUtil(i->first, visited);
        }
    }
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
    
    return ( q1 - ((r*q1)/(r+q1)) );
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
            double d = s + 10 + .003 * (exchange_amount - slip[u]);

            // If there is shorter path to v through u.
            if (dist[v] > dist[u] + d)
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
    return numTokens*(numTokens - 1)/2;
}

void Graph::randomAllocationSupport(double totalLiquidity, uint16_t numTokens, vector<pair<uint16_t, uint16_t> > support) {
    totalLiq = totalLiquidity;
    //creates an instance of an engine.
    random_device rnd_device;
    // Specify the engine and distribution.
    mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    uniform_int_distribution<double> dist {1, 52};
    auto gen = [&dist, &mersenne_engine](){
        return dist(mersenne_engine);
    };
    
    //calculates max number of edges for a graph with numTokens (n choose 2)
    uint16_t numEdges = static_cast<uint16_t>(support.size());
    vector<double> vec(numEdges);
    
    generate(begin(vec), end(vec), gen);
    
    //finds the sum of the random vector to help normalize
    double div = accumulate(vec.begin(), vec.end(), 0);
    
    //divides the vector by scalar (div); this makes the sum = 1
    transform(vec.begin(), vec.end(), vec.begin(), [div](double &c){ return c/div; });
    
    //multiplies the vector by scalar (totalLiquidity); this makes sum of weights to totalLiquidity
    transform(vec.begin(), vec.end(), vec.begin(), [totalLiquidity](double &c){ return c*totalLiquidity; });
    
    for (size_t i = 0; i < support.size(); ++i) {
        this->addEdge(support[i].first, support[i].second, vec[i]);
    }
}

void Graph::randomAllocationDENSE(double totalLiquidity, uint16_t numTokens) {
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
    
    uint16_t iter = uni(rng);
    
    //TODO: FIX BUG W/ MUTATION AND HIGH LOSS
    for (uint16_t i = 0; i < iter; i++) {
    
        uint16_t i1 = uni(rng);
        uint16_t i2 = uni(rng);

        double mut_amount = totalLiquidity/(numEdges*100);
        double temp;
    
        pair< pair<uint16_t, double>*, pair<uint16_t,double>* >* e1 = &mutant.edgeList[i1];
        pair< pair<uint16_t,double>*, pair<uint16_t,double>* >* e2 = &mutant.edgeList[i2];
        if (e1->first->second - mut_amount > totalLiquidity/(numEdges*20)){
            e1->first->second -= mut_amount;
            e1->second->second -= mut_amount;
        
            e2->first->second += mut_amount;
            e2->second->second += mut_amount;
        }
    }
    return mutant;
}

vector<Graph> initPopulationDENSE(uint16_t numTokens, uint16_t numOrgs, double totalLiquidity) {
    vector<Graph> population;
    population.resize(numOrgs, Graph(numTokens));
    
    for (uint16_t s = 0; s < numOrgs; ++s) {
        population[s].randomAllocationDENSE(totalLiquidity, numTokens);
    }
    return population;
}

vector<Graph> initPopulationSupport(uint16_t numTokens, uint16_t numOrgs, double totalLiquidity, vector<pair<uint16_t, uint16_t> > support) {
    vector<Graph> population;
    population.resize(numOrgs, Graph(numTokens));
    
    for (uint16_t s = 0; s < numOrgs; ++s) {
        population[s].randomAllocationSupport(totalLiquidity, numTokens, support);
        
    }
    return population;
}

struct comp2 {
    bool operator()(pair<size_t, double> a, pair<size_t, double> b) {
        return a.second < b.second;
    }
};


vector<Order> initOrderBookDENSE(uint16_t numTokens) {
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
        // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(500, 10000);
        
    vector<Order> orderbook;
    uint16_t index = 0;
    for (uint16_t i = 0; i < numTokens - 1; ++i) {
        for (uint16_t j = i + 1; j < numTokens; ++j) {
            orderbook.push_back(Order{i, j, unif(rng)});
            index++;
        }
    }
    return orderbook;
}
       



