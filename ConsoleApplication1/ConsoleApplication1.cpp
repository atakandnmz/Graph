﻿// C / C++ program for Dijkstra's shortest path algorithm for adjacency 
// list or matrix representation of graph.I choose the adj list. 
#include<bits/stdc++.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h>

using namespace std;
// A structure to represent a node in adjacency list 
struct AdjListNode
{
	int dest;
	int weight;
	struct AdjListNode* next;
};

// A structure to represent an adjacency list 
struct AdjList
{
	struct AdjListNode* head; // pointer to head node of list 
};

// A structure to represent a graph. A graph is an array of adjacency lists. 
// Size of array will be V (number of vertices in graph) 
struct Graph
{
	int V;
	bool isCyclic();
	struct AdjList* array;
};

// A utility function to create a new adjacency list node 
struct AdjListNode* newAdjListNode(int dest, int weight)
{
	struct AdjListNode* newNode = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}

// A utility function that creates a graph of V vertices 
struct Graph* createGraph(int V)
{
	struct Graph* graph = (Graph*)malloc(sizeof(struct Graph));
	graph->V = V;

	// Create an array of adjacency lists. Size of array will be V 
	graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));

	// Initialize each adjacency list as empty by making head as NULL 
	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;

	return graph;
}

// Adds an edge to an directed graph 
void addEdge(Graph* graph, int src, int dest, int weight)
{
	// Add an edge from src to dest. A new node is added to the adjacency 
	// list of src. The node is added at the beginning 
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	// Since graph is directed, add an edge from dest to src also 
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

// Structure to represent a min heap node 
struct MinHeapNode
{
	int v;
	int dist;
};

// Structure to represent a min heap 
struct MinHeap
{
	int size;	 // Number of heap nodes present currently 
	int capacity; // Capacity of min heap 
	int* pos;	 // This is needed for decreaseKey() 
	struct MinHeapNode** array;
};

// A utility function to create a new Min Heap Node 
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
	struct MinHeapNode* minHeapNode =
		(struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}

// A utility function to create a Min Heap 
struct MinHeap* createMinHeap(int capacity)
{
	struct MinHeap* minHeap =
		(struct MinHeap*)malloc(sizeof(struct MinHeap));
	minHeap->pos = (int*)malloc(capacity * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =
		(struct MinHeapNode**)malloc(capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

// A utility function to swap two nodes of min heap. Needed for min heapify 
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

// A standard function to heapify at given idx 
// This function also updates position of nodes when they are swapped. 
// Position is needed for decreaseKey() 
void minHeapify(struct MinHeap* minHeap, int idx)
{
	int smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;

	if (left < minHeap->size &&
		minHeap->array[left]->dist < minHeap->array[smallest]->dist)
		smallest = left;

	if (right < minHeap->size &&
		minHeap->array[right]->dist < minHeap->array[smallest]->dist)
		smallest = right;

	if (smallest != idx)
	{
		// The nodes to be swapped in min heap 
		MinHeapNode* smallestNode = minHeap->array[smallest];
		MinHeapNode* idxNode = minHeap->array[idx];

		// Swap positions 
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;

		// Swap nodes 
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

		minHeapify(minHeap, smallest);
	}
}

// A utility function to check if the given minHeap is empty or not 
int isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}

// Standard function to extract minimum node from heap 
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	if (isEmpty(minHeap))
		return NULL;

	// Store the root node 
	struct MinHeapNode* root = minHeap->array[0];

	// Replace root node with last node 
	struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;

	// Update position of last node 
	minHeap->pos[root->v] = minHeap->size - 1;
	minHeap->pos[lastNode->v] = 0;

	// Reduce heap size and heapify root 
	--minHeap->size;
	minHeapify(minHeap, 0);

	return root;
}

// Function to decreasy dist value of a given vertex v. This function 
// uses pos[] of min heap to get the current index of node in min heap 
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
	// Get the index of v in heap array 
	int i = minHeap->pos[v];

	// Get the node and update its dist value 
	minHeap->array[i]->dist = dist;

	// Travel up while the complete tree is not hepified. 
	// This is a O(Logn) loop 
	while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
	{
		// Swap this node with its parent 
		minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
		minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

		// move to parent index 
		i = (i - 1) / 2;
	}
}

// A utility function to check if a given vertex 
// 'v' is in min heap or not 
bool isInMinHeap(struct MinHeap* minHeap, int v)
{
	if (minHeap->pos[v] < minHeap->size)
		return true;
	return false;
}

// A utility function used to print the solution 
void printArr(int dist[], int n)
{
	printf("Vertex Distance from Source\n");
	for (int i = 0; i < n; ++i)
		printf("%d \t\t %d\n", i, dist[i]);
}

// The main function that calulates distances of shortest paths from src to all 
// vertices. 
void dijkstra(struct Graph* graph, int src)
{
	int V = graph->V;// Get the number of vertices in graph 
	int dist[5];	 // dist values used to pick minimum weight edge in cut 

	// minHeap represents set E 
	struct MinHeap* minHeap = createMinHeap(V);

	// Initialize min heap with all vertices. dist value of all vertices 
	for (int v = 0; v < V; ++v)
	{
		dist[v] = INT_MAX;
		minHeap->array[v] = newMinHeapNode(v, dist[v]);
		minHeap->pos[v] = v;
	}

	// Make dist value of src vertex as 0 so that it is extracted first 
	minHeap->array[src] = newMinHeapNode(src, dist[src]);
	minHeap->pos[src] = src;
	dist[src] = 0;
	decreaseKey(minHeap, src, dist[src]);

	// Initially size of min heap is equal to V 
	minHeap->size = V;

	// In the followin loop, min heap contains all nodes 
	// whose shortest distance is not yet finalized. 
	while (!isEmpty(minHeap))
	{
		// Extract the vertex with minimum distance value 
		struct MinHeapNode* minHeapNode = extractMin(minHeap);
		int u = minHeapNode->v; // Store the extracted vertex number 

		// Traverse through all adjacent vertices of u (the extracted 
		// vertex) and update their distance values 
		struct AdjListNode* pCrawl = graph->array[u].head;
		while (pCrawl != NULL)
		{
			int v = pCrawl->dest;

			// If shortest distance to v is not finalized yet, and distance to v 
			// through u is less than its previously calculated distance 
			if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
				pCrawl->weight + dist[u] < dist[v])
			{
				dist[v] = dist[u] + pCrawl->weight;

				// update distance value in min heap also 
				decreaseKey(minHeap, v, dist[v]);
			}
			pCrawl = pCrawl->next;
		}
	}

	// print the calculated shortest distances 
	printArr(dist, V);
}

class Graph1
{
	int V; // No. of vertices 
	list<int>* adj; // Pointer to an array containing adjacency lists 
	bool isCyclicUtil(int v, bool visited[], bool* rs); // used by isCyclic() 
public:
	Graph1(int V); // Constructor 
	void addEdge1(int v, int w); // to add an edge to graph 
	bool isCyclic(); // returns true if there is a cycle in this graph 
};

Graph1::Graph1(int V)
{
	this->V = V;
	adj = new list<int>[V];
}

void Graph1::addEdge1(int v, int w)
{
	adj[v].push_back(w); // Add w to vâ€™s list. 
}


bool Graph1::isCyclicUtil(int v, bool visited[], bool* recStack)
{
	if (visited[v] == false)
	{
		// Mark the current node as visited and part of recursion stack 
		visited[v] = true;
		recStack[v] = true;

		// Recur for all the vertices adjacent to this vertex 
		list<int>::iterator i;
		for (i = adj[v].begin(); i != adj[v].end(); ++i)
		{
			if (!visited[*i] && isCyclicUtil(*i, visited, recStack))
				return true;
			else if (recStack[*i])
				return true;
		}

	}
	recStack[v] = false; // remove the vertex from recursion stack 
	return false;
}

// Returns true if the graph contains a cycle, else false. 

bool Graph1::isCyclic()
{
	// Mark all the vertices as not visited and not part of recursion 
	// stack 
	bool* visited = new bool[V];
	bool* recStack = new bool[V];
	for (int i = 0; i < V; i++)
	{
		visited[i] = false;
		recStack[i] = false;
	}

	// Call the recursive helper function to detect cycle in different 
	// DFS trees 
	for (int i = 0; i < V; i++)
		if (isCyclicUtil(i, visited, recStack))
			return true;

	return false;
}

// Driver program to test above functions 
int main()
{
	// create the graph given in above figure 
	int V = 5;
	Graph1 g(5);
	Graph* graph = createGraph(V);
	addEdge(graph, 0, 4, 8);
	addEdge(graph, 0, 3, 7);
	addEdge(graph, 0, 1, 3);
	addEdge(graph, 1, 3, 4);
	addEdge(graph, 1, 2, 1);
	addEdge(graph, 3, 2, 2);
	addEdge(graph, 4, 3, 3);
	dijkstra(graph, 2); //select source node

	/*g.addEdge1(0,4);
g.addEdge1(0,3);
g.addEdge1(0,1);
g.addEdge1(1,3);
g.addEdge1(1,2);
g.addEdge1(3,2);
	g.addEdge1(4,3);*/
	if (g.isCyclic())
		cout << "Graph contains cycle";
	else
		cout << "Graph doesn't contain cycle";
	return 0;

	dijkstra(graph, 2); //source distance

	return 0;
}
