#include "graph.h"
#include <cstdio>

Graph::Graph()
{
}

Graph::Graph(int n)
{
    /*
        Creates graph, that has n vertices
		We also create lists of incoming and outgoing arcs, we need them for DFS and looking for critical sections
	*/
    vertices_number = n;
    arcs_exiting_vertex.resize(n);
    arcs_entering_vertex.resize(n);
}

Graph::~Graph()
{
}

void Graph::addArc(int from, int to, int length)
{
    /*
        Adding an arc from vertex "from" to vertex "to" with determined length
	*/
    Arc outgoing_arc = {to, length};
    Arc incoming_arc = {from, length};
    arcs_exiting_vertex[from].push_back(outgoing_arc);
    arcs_entering_vertex[to].push_back(incoming_arc);
}

void Graph::setArcLength(int from, int to, int length)
{
    /*
        Sets different length of the arc from vertex "from" to another vertex "to"
	*/
    bool exists = false;
    for (Vertex::iterator it = arcs_exiting_vertex[from].begin(); it != arcs_exiting_vertex[from].end(); it++) //for every arc exiting vertex "from"
    {
        if (it->vertex_id == to)    //if found arc which enters vertex "tp"
        {
            it->length = length;    //change lenth
            exists = true;
        }
    }
    for (Vertex::iterator it = arcs_entering_vertex[to].begin(); it != arcs_entering_vertex[to].end(); it++) //for every arc entering vertex "to"
    {
        if (it->vertex_id == from)  //if found arc which goes from "from"
        {
            it->length = length;    //change length
            if (exists)
                return;
        }
    }
    if (!exists)
    {
        printf("Arc form %d to %d does not exist\n", from, to);
    }
}

Arc Graph::getArc(int from, int to)
{
    /*
        Returns arc from vertex "from" to vertex "to"
	*/
    for (Vertex::iterator it = arcs_exiting_vertex[from].begin(); it != arcs_exiting_vertex[from].end(); it++)  //for every outgoing arc
    {
        if (it->vertex_id == to)    //if found arc which enters "to"
        {
            return *it;     //return this arc
        }
    }
    printf("Arc form %d to %d does not exist\n", from, to);

}

int Graph::getArcLength(int from, int to)
{
    /*
		Returns length of arc from vertex "form" to vertex "to"
	*/
    Arc a = getArc(from, to);
    return a.length;
}

void Graph::deleteArc(int from, int to)
{
    /*
		Removes arc from vertex "from" to vertex "to"
	*/
    bool erased = false;
    for (Vertex::iterator it = arcs_exiting_vertex[from].begin(); it != arcs_exiting_vertex[from].end(); it++) //for every arc exiting vertex
    {
        if (it->vertex_id == to)    //if arc enters vertex "to"
        {
            arcs_exiting_vertex[from].erase(it);    //erase the arc
            erased = true;
            break;
        }
    }
    for (Vertex::iterator it = arcs_entering_vertex[to].begin(); it != arcs_entering_vertex[to].end(); it++) //for every arc entering vertex
    {
        if (it->vertex_id == from)  //if arc exits vertex "from"
        {
            arcs_entering_vertex[to].erase(it); //erase the arc
            break;
        }
    }
    if (erased)
    {
        return;
    }
    printf("Arc form %d to %d does not exist\n", from, to);
}

void Graph::invertArc(int from, int to)
{
    /*
        Inverts the direction of the arc
	*/
    int length = getArc(from, to).length;
    deleteArc(from, to);
    addArc(to, from, length);
}

vector<int> Graph::maxDistances()
{
    /*
        Returns vector of the maximum distances from source to every vertex
	*/
    vector<int> distances;  //creating vector of distances for every vertex
    int n = vertices_number;

    distances.resize(n);

    deque<int> topo = topologicalSort();   //topological sort of vertexes

    for (int j = 0; j < n; j++) //setting all distances to minimum, which is 0 in our case
    {
        distances[topo[j]] = 0;
    }

    for (int j = 1; j < n; j++) //for every vertex except source, which distance is minimum anyway
    {
        for (int i = 0; i < arcs_entering_vertex[topo[j]].size(); i++)  //for every vertex which is connected to our j vertex (arc has to enter j vertex)
        {
            int a = distances[topo[j]];

            int b = distances[arcs_entering_vertex[topo[j]][i].vertex_id];

            Arc arc = getArc(arcs_entering_vertex[topo[j]][i].vertex_id, topo[j]);
            int c = arc.length;

            distances[topo[j]] = max(a, b + c); //distance is max out of current distance and distance of neighbouring vertex plus length of an arc
        }
    }

    return distances;
}

deque<int> Graph::criticalPath(int sink)
{
    /*
        Returns the critical path from source(0) to sink, which is simply the longest possible path of coming through all the vertexes
	*/
    vector<int> d = maxDistances();

    deque<int> path;       //critical path
    path.push_front(sink); // pushing last element to critical path
    int v = sink;          // current vertex
    bool overflow = false;
    while (v != 0) // while we don't reach source
    {
        int u;
        for (int i = 0; i < arcs_entering_vertex[v].size(); i++)    //for every arc entering the current vertex
        {
            if (d[v] == d[arcs_entering_vertex[v][i].vertex_id] + getArcLength(arcs_entering_vertex[v][i].vertex_id, v)) // find the vertex for which te path is the biggest
            {
                u = arcs_entering_vertex[v][i].vertex_id; // set u to that vertex
                break;
            }
        }
        path.push_front(u); // push u to the heap
        v = u;              // set current vertex to u
        if (path.size() > 2000 && !overflow)
        {
            printf("Path size is too big!\n");
            overflow = true;
        }
    } // end

    return path;
}

void Graph::createAcyclicClique(vector<int> vertices, vector<int> lengths)
{
    /*
        Creates disjunctive arcs between every pair of vertexes from "vertices".
        The arcs have length = lengths[i], which is length of arc exiting vertex i
        The direction of arcs is from vertex to it's neighbouring vertexes, that is not connected with yet.
        This guarantees that we get an acyclic graph.
	*/

    for (int i = 0; i < vertices.size(); i++) // for every vertex in vertices
    {
        int from = vertices[i];
        for (int j = i + 1; j < vertices.size(); j++)   //for the remaining vertices
        {
            addArc(from, vertices[j], lengths[i]);     //add arc from the current i vertex to remaining vertices, with length of the i task.
        }
    }
}

deque<int> Graph::topologicalSort() // result[i] = wierzcholek bedacy i-tym w uszeregowaniu topologicznym
{
    /*
        Returns topological order of the graph (or empty deque if there is a cycle in graph, which should not happen). 
        We use our vectors of already visited nad currently visited vertexes
	*/

    topological_order.clear(); //cleaning topological_order deque

    dfs_already_visited.resize(vertices_number);
    dfs_currently_visited.resize(vertices_number);

    fill(dfs_already_visited.begin(), dfs_already_visited.end(), false);
    fill(dfs_currently_visited.begin(), dfs_currently_visited.end(), false);

    cycle_exists = false;

    for (int i = 0; i < vertices_number; i++) //for every vertex that is not yet visited
    {
        if (!dfs_already_visited[i])
        {
            dfsVisitTopo(i); //perform "topological search"
        }
    }
    return (!cycle_exists) ? topological_order : deque<int>();  //if cycle was not found return topological_order
}

void Graph::dfsVisitTopo(int node)
{
    /*
        Part of the topological sort algorithm. We make precausions in case a cycle occurred, 
        but it shouldn't happenas we are working on directed acyclic graph.
        We visit the vertex and its successors (except for when we already visited it) until we find a vertex with no successors. 
        Then we add it to topological_order deque.
	*/
    if (cycle_exists)
    {
        return;
    }
    if (dfs_currently_visited[node])
    {
        cycle_exists = true;
        return;
    }
    else if (!dfs_already_visited[node]) //if vertex was not visited yet
    {
        dfs_currently_visited[node] = true;
        for (int i = 0; i < arcs_exiting_vertex[node].size(); i++) //for the vertex successors
        {
            dfsVisitTopo(arcs_exiting_vertex[node][i].vertex_id); //perform "topological search"
        }
        dfs_already_visited[node] = true;
        dfs_currently_visited[node] = false;
        topological_order.push_front(node); //after reaching a vertex with no unvisited successors left add the vertex to the topological_order deque
    }
}

bool Graph::getCycleExists(){
    return cycle_exists;
}