#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <deque>
#include <algorithm>
#include <cstdio>

using namespace std;

typedef struct Arc
{				   // Arc structure
	int vertex_id; // number of the predecessor or successor
	int length;	// length of the arc
} Arc;

typedef vector<Arc> Vertex;		 // Element of the Vertices vector. It contains entering or exiting arcs
typedef vector<Vertex> Vertices; // Vector of predecessors or successors

class Graph
{
  public:
	Graph();
	Graph(int n); // n = number of vertices
	~Graph();

	void addArc(int from, int to, int length);		   // Adding an arc from vertex "from" to vertex "to" with determined length
	void setArcLength(int from, int to, int length); // Sets different length of the arc from vertex "from" to another vertex "to"
	Arc getArc(int from, int to);					   // Returns arc from vertex "from" to vertex "to"
	int getArcLength(int from, int to);			   // Returns length of arc from vertex "form" to vertex "to"
	void deleteArc(int from, int to);				   // Removes arc from vertex "from" to vertex "to"
	void invertArc(int from, int to);				   // Inverts the direction of the arc
	bool getCycleExists(); 

	void createAcyclicClique(vector<int> vertices, vector<int> lengths); // Creates disjunctive arcs between every pair of vertexes from "vertices".

	vector<int> maxDistances();			// Returns vector of the maximum distances from source to every vertex
	deque<int> criticalPath(int sink); // Returns the critical path from source to sink, which is simply the longest possible path


  private:
	int vertices_number;	// Number of vertices in graph
	Vertices arcs_exiting_vertex; // List of successors needed for DFS
	Vertices arcs_entering_vertex; // List of predecessors needed for longest path search

	vector<bool> dfs_already_visited; // Contains information whether vertex have been already visited during DFS
	vector<bool> dfs_currently_visited;		  // Contains information whether vertex is currently visited by DFS
	bool cycle_exists;				  // Informaton whether cycle has been detected during DFS

	deque<int> topological_order; // Topological order established in topologicalSort() method

	deque<int> topologicalSort(); // Returns topological order of the graph vertices
	void dfsVisitTopo(int node); // Calls DFS visit procedure for the topological sort
};

#endif