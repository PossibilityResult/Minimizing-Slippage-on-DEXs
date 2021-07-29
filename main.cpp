
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
#include <cstdlib>   // rand and srand
#include <ctime>


//TODO: CREATE A VECTOR A VECTOR OF PAIRS OF POINTERS TO EDGES

using namespace std;
# define INF 0x3f3f3f3f

// iPair ==> Integer Pair
typedef pair<int, double> iPair;


struct Order {
    int src;
    int target;
    double amount;
};

// This class represents a directed graph using
// adjacency list representation
class Graph
{
public:
    int V; // No. of vertices

    // In a weighted graph, we need to store vertex
    // and weight pair for every edge
    list< pair<int, double> > *adj;
    
    //vector<vector<double> > adjMat;
    vector< pair< pair<int,double>*, pair<int,double>* > > edgeList;
    
    double costs;

    Graph(int V); // Constructor
    
    Graph(const Graph &g); // copy constructor

    // function to add an edge to graph
    void addEdge(int u, int v, double w);

    // prints shortest path from s
    double shortestPath(int s, int t, double exchange_amount);
    
    //generates a random allocation of liquidity
    void randomAllocation(double totalLiquidity, int numTokens);
    
    //calculates total loss (gas + transaction fees + slippage) given an orderbook
    void lossFunction(vector<Order> orderBook);
};

// Allocates memory for adjacency list
Graph::Graph(int V)
{
    this->V = V;
    adj = new list<iPair> [V];
}

//TODO: FINISH COPY CONSTRUCTOR
Graph::Graph(const Graph &g)
{
    this->V = g.V;
    adj = new list<iPair> [V];
    
    if (g.edgeList.size() > 0) {
        for (int i = 0; i < V; ++i){
            this->addEdge(g.edgeList[i].first->first, g.edgeList[i].second->first, g.edgeList[i].first->second);
        }
    }
}


void Graph::addEdge(int u, int v, double w)
{
    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w));
    
    edgeList.push_back(make_pair(&adj[u].back(), &adj[v].back()));
    //adjMat[u][v] = w;
    //adjMat[v][u] = w;
}


double calculateSlippage(double liquidity, double q1) {
    double r = liquidity/2;
    if (q1 >= r) {
        return INT_MAX;
    }
    return ( q1 - (r*q1)/(r+q1) );
}


// Prints shortest paths from src to all other vertices
double Graph::shortestPath(int src, int target, double exchange_amount)
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
        int u = pq.top().second;
        pq.pop();

        // 'i' is used to get all adjacent vertices of a vertex
        list< pair<int, double> >::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i)
        {
            // Get vertex label and weight of current adjacent
            // of u.
            int v = (*i).first;
            double weight = (*i).second;
            
            //calculates just slippage loss
            double s = calculateSlippage(weight, exchange_amount - slip[u]);
            //calculate distance (includes transaction fees and gas)
            double d = s + slip[u] + 10 + .003 * (exchange_amount - slip[u]);

            // If there is shorted path to v through u.
            if (dist[v] > dist[u] + weight)
            {
                // Updating distance of v
                dist[v] = dist[u] + d;
                slip[v] = slip[u] + s;
                pq.push(make_pair(dist[v], v));
                if (v == target) {
                    return dist[v];
                }
            }
        }
    }
    return -1;
}


int maxEdges(int numTokens) {
    return numTokens*(numTokens-1)/2;
}


void Graph::randomAllocation(double totalLiquidity, int numTokens){
    //creates an instance of an engine.
    random_device rnd_device;
    // Specify the engine and distribution.
    mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    
    uniform_int_distribution<double> dist {1, 52};
    
    auto gen = [&dist, &mersenne_engine](){
        return dist(mersenne_engine);
    };
    
    //calculates max number of edges for a graph with numTokens (n choose 2)
    int numEdges = maxEdges(numTokens);

    vector<double> vec(numEdges);
    generate(begin(vec), end(vec), gen);
    
    //finds the sum of the random vector to help normalize
    double div = accumulate(vec.begin(), vec.end(), 0);
    
    //divides the vector by scalar (div); this makes the sum = 1
    transform(vec.begin(), vec.end(), vec.begin(), [div](double &c){ return c/div; });
    
    //multiplies the vector by scalar (totalLiquidity); this makes sum of weights to totalLiquidity
    transform(vec.begin(), vec.end(), vec.begin(), [totalLiquidity](double &c){ return c*totalLiquidity; });
    
    int index = 0;
    for (int i = 0; i < numTokens - 1; ++i) {
        for (int j = i + 1; j < numTokens; ++j){
            addEdge(i, j, vec[index]);
            ++index;
        }
    }
}


void Graph::lossFunction(vector<Order> orderBook) {
    double loss = 0;
    
    for (size_t s = 0; s < orderBook.size(); ++s) {
       loss += shortestPath(orderBook[s].src, orderBook[s].target, orderBook[s].amount);
    }
    costs = loss;
}




//Returns a mutant graph
//TODO: TEST WHETHER RANDOM SEED RETURNS SAME EDGES for every mutation
Graph mutate(Graph &g, double totalLiquidity){
    Graph mutant = Graph(g);
    
    // Get the system time.
    unsigned seed = static_cast<unsigned>(time(0));
    
    // Seed the random number generator.
    srand(seed);
    
    int numEdges = static_cast<int>(mutant.edgeList.size());
    
    int i1 = rand() % numEdges;
    cout << i1 << "\n";

    int i2 = rand() % numEdges;
    cout << i2 << "\n\n";

    double mut_amount = totalLiquidity/(numEdges*50);
    
    double temp;
    
    pair< pair<int,double>*, pair<int,double>* >* e1 = &mutant.edgeList[i1];
    pair< pair<int,double>*, pair<int,double>* >* e2 = &mutant.edgeList[i2];
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
    return mutant;
}

vector<Graph> initPopulation(int numTokens, int numOrgs, int totalLiquidity) {
    vector<Graph> population;
    Graph temp(3);
    for (int s = 0; s < numOrgs; ++s) {
        temp = Graph(numTokens);
        population.push_back(temp);
    }
    
    for (int s = 0; s < numOrgs; ++s) {
        population[s].randomAllocation(totalLiquidity, numTokens);
        //cout << (&population[s]->edgeList[0])->first->second << "\n";
    }
    return population;
}

/*void destructor(vector<Graph> &population) {
    for (size_t s = 0; s < population.size(); ++s) {
        delete population[s];
        population[s] = nullptr;
    }
}*/

struct comp {
    bool operator()(Graph a, Graph b) {
        return a.costs < b.costs;
    }
};

vector< Graph > scoring(vector<Graph> &population, vector<Order> orderbook) {
    double loss;
    double totalLoss = 0;
    for (size_t s = 0; s < population.size(); ++s) {
        population[s].lossFunction(orderbook);
        loss = population[s].costs;
        totalLoss += loss;
    }
    cout << "Average Total Cost: " << totalLoss/population.size() << "\n";
    sort(population.begin(), population.end(), comp());
    cout << "Lowest Cost: " << population[0].costs << "\n\n";
    return population;
}

vector<Graph> selection(vector<Graph> &population, vector<Order> orderbook, double totalLiquidity) {
    vector<Graph> scoredPopulation = scoring(population, orderbook);
    vector<Graph> newPopulation;
    for (size_t s = 0; s < population.size() - 80; ++s) {
        newPopulation.push_back(Graph(population[s]));
    }
    for (size_t s = 0; s < 40; ++s) {
        for (size_t s = 0; s < 2; ++s) {
            newPopulation.push_back(mutate(population[s], totalLiquidity));
        }
    }
    return population;
}



// Driver program to test methods of graph class
int main()
{
    int numTokens = 3;
    double totalLiquidity = 10000;
    int numOrgs = 100;
    int numGens = 10;
    
    //TODO: Maybe create a variable to specify order book size for space optimization (use .resize())
    vector<Order> orderBook;
    //initialize order book; TODO: find a more efficient way of doing this
    orderBook.push_back(Order{0, 1, 100});
    orderBook.push_back(Order{1, 2, 100});
    orderBook.push_back(Order{1, 2, 100});
    
    
    vector<Graph> population = initPopulation(numTokens, numOrgs, totalLiquidity);
    
    for (int i = 0; i < numGens; ++i) {
        cout << "Generation " << i << "\n";
        population = selection(population, orderBook, totalLiquidity);
    }
    //destructor(population);
    return 0;
}

