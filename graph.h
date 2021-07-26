// Project Identifier: B99292359FFD910ED13A7E6C7F9705B8742F0D79

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>
#include <map>

using namespace std;

// A structure to represent a
// node in adjacency list
struct AdjListNode
{
    int dest;
    double weight;
    struct AdjListNode* next;
};
 
// A structure to represent
// an adjacency list
struct AdjList
{
   // Pointer to head node of list
   struct AdjListNode *head;
};


// A structure to represent a graph.
// A graph is an array of adjacency lists.
// Size of array will be V (number of
// vertices in graph)
struct Graph
{
	int num_tokens;
	struct AdjList* array;
};

double calculateSlippage(double liquidity, double q1) {
    double r = liquidity/2;
    if (q1 >= r) {
        return numeric_limits<double>::infinity();
    }
    return ( q1 - (r*q1)/(r+q1) );
}

class DEX {
	public:

    double liquidity;
    int num_edges;
    Graph* graph;
    
    //dense, equal liquidity distribution
    DEX(int s, double l);

    //modified dijkstra
    vector<double> _slippage_dijkstra_alg(double exchange_amount, int src, int target);

	void addEdge(int src, int dest, double weight);

	void get_options(int argc, char** argv);
};

struct AdjListNode* newAdjListNode(int dest, double liquidity)
{
	struct AdjListNode* newNode =
			(struct AdjListNode*)
	malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = liquidity;
	newNode->next = NULL;
	return newNode;
}

// Adds an edge to an undirected graph
void DEX::addEdge(int src, int dest, double weight)
{
	// Add an edge from src to dest.
	// A new node is added to the adjacency
	// list of src. The node is
	// added at the beginning
	struct AdjListNode* newNode =
			newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	// Since graph is undirected,
	// add an edge from dest to src also
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

DEX::DEX(int s, double l) {
    graph->num_tokens = s;
    num_edges = s*(s-1)/2;
    liquidity = l;

    double liquidity_per = liquidity/num_edges;

	struct Graph* graph = (struct Graph*)
			malloc(sizeof(struct Graph));
	graph->num_tokens = s;

	// Create an array of adjacency lists.
	// Size of array will be s
	graph->array = (struct AdjList*)
	malloc(static_cast<unsigned int>(s) * sizeof(struct AdjList));

	// Initialize each adjacency list
	// as empty by making head as NULL
	for (int i = 0; i < s; ++i)
		graph->array[i].head = NULL;

	for (int i = 0; i < s; ++i) {
		for (int j = i + 1; j < s; ++j) {
			if (i != j) {
				addEdge(i, j, liquidity_per);
			}
		}
	}
}


// Structure to represent a min heap node
struct MinHeapNode
{
	int v;
	double dist;
};

// Structure to represent a min heap
struct MinHeap
{
	// Number of heap nodes present currently
	int size;	

	// Capacity of min heap
	int capacity;

	// This is needed for decreaseKey()
	int *pos;
	struct MinHeapNode **array;
};

// A utility function to create a
// new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v,
								double dist)
{
	struct MinHeapNode* minHeapNode =
		(struct MinHeapNode*)
	malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}

// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
	struct MinHeap* minHeap =
		(struct MinHeap*)
	malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(
			static_cast<unsigned long>(capacity) * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =
		(struct MinHeapNode**)
				malloc(static_cast<unsigned long>(capacity) *
	sizeof(struct MinHeapNode*));
	return minHeap;
}

// A utility function to swap two
// nodes of min heap.
// Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a,
					struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

// A standard function to
// heapify at given idx
// This function also updates
// position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap,
								int idx)
{
	int smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;

	if (left < minHeap->size &&
		minHeap->array[left]->dist <
		minHeap->array[smallest]->dist )
	smallest = left;

	if (right < minHeap->size &&
		minHeap->array[right]->dist <
		minHeap->array[smallest]->dist )
	smallest = right;

	if (smallest != idx)
	{
		// The nodes to be swapped in min heap
		MinHeapNode *smallestNode =
			minHeap->array[smallest];
		MinHeapNode *idxNode =
				minHeap->array[idx];

		// Swap positions
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;

		// Swap nodes
		swapMinHeapNode(&minHeap->array[smallest],
						&minHeap->array[idx]);

		minHeapify(minHeap, smallest);
	}
}

// A utility function to check if
// the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}

// Standard function to extract
// minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap*
								minHeap)
{
	if (isEmpty(minHeap))
		return NULL;

	// Store the root node
	struct MinHeapNode* root =
				minHeap->array[0];

	// Replace root node with last node
	struct MinHeapNode* lastNode =
		minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;

	// Update position of last node
	minHeap->pos[root->v] = minHeap->size-1;
	minHeap->pos[lastNode->v] = 0;

	// Reduce heap size and heapify root
	--minHeap->size;
	minHeapify(minHeap, 0);

	return root;
}

// Function to decreasy dist value
// of a given vertex v. This function
// uses pos[] of min heap to get the
// current index of node in min heap
void decreaseKey(struct MinHeap* minHeap,
						int v, double dist)
{
	// Get the index of v in heap array
	int i = minHeap->pos[v];

	// Get the node and update its dist value
	minHeap->array[i]->dist = dist;

	// Travel up while the complete
	// tree is not hepified.
	// This is a O(Logn) loop
	while (i && minHeap->array[i]->dist <
		minHeap->array[(i - 1) / 2]->dist)
	{
		// Swap this node with its parent
		minHeap->pos[minHeap->array[i]->v] =
									(i-1)/2;
		minHeap->pos[minHeap->array[
							(i-1)/2]->v] = i;
		swapMinHeapNode(&minHeap->array[i],
				&minHeap->array[(i - 1) / 2]);

		// move to parent index
		i = (i - 1) / 2;
	}
}

// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
if (minHeap->pos[v] < minHeap->size)
	return true;
return false;
}

// A utility function used to print the solution
void printArr(vector<double> dist, int n)
{
	printf("Vertex Distance from Source\n");
	for (int i = 0; i < n; ++i)
		cout << "token " << i << "is " << dist[static_cast<unsigned int>(i)] << " is slippage loss away from source.";
}


vector<double> DEX::_slippage_dijkstra_alg(double exchange_amount, int src, int target) {
    /*push = heappush
    pop = heappop
    dist = {}  # dictionary of final distances
    slip = {} # dictionary of slippage for recursion
    seen = {}
    # fringe is heapq with 3-tuples (distance,c,node)
    # use the count c to avoid comparing nodes (may not be able to)
    c = count()
    fringe = []
    if source not in G:
        raise nx.NodeNotFound(f"Source {source} not in G")
    seen[source] = 0
    push(fringe, (0, next(c), source))
    while fringe:
        (d, _, v) = pop(fringe)
        if v in dist:
            continue  # already searched this node.
        dist[v] = d
        slip[v] = d
        if v == target:
            break
        for u, e in G_succ[v].items():
            slippage = calcSlippageLoss(e['liquidity'], (exchange_amount - dist[v]))
            #cost = calcSlippageLoss(e['liquidity'], (exchange_amount - dist[v]))
            #cost = weight(v, u, e)
            if slippage is None:
            #if cost is None:
                continue
            #vu_dist = dist[v] + cost
            vu_dist = slip[v] + slippage + 10 + .003 * (exchange_amount) #slippage + gas + transaction fee
            if u in dist:
                u_dist = dist[u]
                if vu_dist < u_dist:
                    raise ValueError("Contradictory paths found:", "negative weights?")
            elif u not in seen or vu_dist < seen[u]:
                seen[u] = vu_dist
                slip[u] = slip[v] + slippage
                push(fringe, (vu_dist, next(c), u))
                if paths is not None:
                    paths[u] = paths[v] + [u]

    # The optional predecessor and path dictionaries can be accessed
    return dist*/

	// dist values used to pick
	// minimum weight edge in cut

	target++; //TODO: FIX THIS AND ADD TARGET CODE!!!

    vector<double> dist;
	dist.resize(static_cast<unsigned int>(graph->num_tokens), INT_MAX);	

	// minHeap represents set E
	struct MinHeap* minHeap = createMinHeap(graph->num_tokens);

	double d;

	// Initialize min heap with all
	// vertices. dist value of all vertices
	for (int v = 0; v < graph->num_tokens; ++v)
	{
		minHeap->array[v] = newMinHeapNode(v, dist[static_cast<unsigned int>(v)]);
		minHeap->pos[v] = v;
	}

	// Make dist value of src vertex
	// as 0 so that it is extracted first
	minHeap->array[src] =
		newMinHeapNode(src, dist[static_cast<unsigned int>(src)]);
	minHeap->pos[src] = src;
	dist[static_cast<unsigned int>(src)] = 0;
	decreaseKey(minHeap, src, dist[static_cast<unsigned int>(src)]);

	// Initially size of min heap is equal to V
	minHeap->size = graph->num_tokens;

	// In the followin loop,
	// min heap contains all nodes
	// whose shortest distance
	// is not yet finalized.
	while (!isEmpty(minHeap))
	{
		// Extract the vertex with
		// minimum distance value
		struct MinHeapNode* minHeapNode =
					extractMin(minHeap);
	
		// Store the extracted vertex number
		int u = minHeapNode->v;

		// Traverse through all adjacent
		// vertices of u (the extracted
		// vertex) and update
		// their distance values
		struct AdjListNode* pCrawl =
					graph->array[u].head;
		while (pCrawl != NULL)
		{
			int v = pCrawl->dest;
            d = calculateSlippage(pCrawl->weight, exchange_amount);

			// If shortest distance to v is
			// not finalized yet, and distance to v
			// through u is less than its
			// previously calculated distance
			if (isInMinHeap(minHeap, v) &&
					dist[static_cast<unsigned int>(u)] != INT_MAX &&
			d + dist[static_cast<unsigned int>(u)] < dist[static_cast<unsigned int>(v)])
			{
				dist[static_cast<unsigned int>(v)] = dist[static_cast<unsigned int>(u)] + d;

				// update distance
				// value in min heap also
				decreaseKey(minHeap, v, dist[static_cast<unsigned int>(v)]);
			}
			pCrawl = pCrawl->next;
		}
	}

	// print the calculated shortest distances
	return dist;
}

void DEX::get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    opterr = false;

    // use getopt to find command line options
    struct option longOpts[] = {
        {nullptr,        0,                 nullptr, '\0'}
    };
    
    string out;

    while ((option = getopt_long(argc, argv, "vmgw", 
    longOpts, &option_index)) != -1) {
        switch (option) {
                break;
        }
    }
    //checks if s and q arguments were both given
}