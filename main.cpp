// Project Identifier: B99292359FFD910ED13A7E6C7F9705B8742F0D79

#include <iostream>
#include "graph.h"
using namespace std;

int main(int argc, char* argv[])
{   
    ios_base::sync_with_stdio(false);

    int num_tokens = 3;
    double total_liquidity = 10000;
    double exch_amnt = 1000;
    
    // create the graph given in above fugure
    DEX dex = DEX(num_tokens, total_liquidity);

    try {
      // Read and process the command line options.
      dex.get_options(argc, argv);
    }
    // Catch runtime_errors, print the message, and exit the
    // program with a non-zero status.
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    dex.addEdge(0, 1, 100);
    dex.addEdge(1, 2, 100);
    dex.addEdge(2, 0, 100);
    printArr(dex._slippage_dijkstra_alg(exch_amnt, 0, 1), dex.graph->num_tokens);
    
    
    return 0;
}