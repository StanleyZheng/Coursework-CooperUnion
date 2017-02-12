#ifndef _GRAPH_H
#define _GRAPH_H

#include <vector>
#include <string>
#include <list>
#include "hash.h"
#include "heap.h"

class graph {

public:
    graph(int size);
    //
    //contains() - returns true if id exists in hashTable
    bool contains(std::string id);

    //
    //insert()  - inserts source and destination into graph if they do not exist
    //          - inserts an edge into the source's adjacency list
    void insert(std::string source, std::string destination, int edge_cost);

    //
    //dijkstra() - given a starting vertex, finds the shortest path to all other vertices
    //           - distance from original vertex is stored as dist in each vertex
    void dijkstra(std::string start_vertex);

    //
    //printPaths() - prints the paths of each vertex and prints it to output
    void printPaths(std::string start_vertex, std::ofstream &output);

private:
    class vertex {
    public:
        class edge {                //nested class to house the edges
        public:
            edge(vertex* input_destin, int input_cost);
            vertex *destin;         //edge destination, all sources are the same within the adjacency list
            int cost;               //edge cost
        };

        vertex(std::string input_id);
        std::string id;
        std::list<edge> adjList;    // adjacency list of edges
        int dist;                   // distance from starting vertex
        bool known;                 // whether or not it has its shortest distance already
        vertex *prev;               // pointer to 'parent' in the path
    };

    std::list<vertex> data;         // The actual Graph, list of all vertices
    hashTable *mapping;             // maps ids to vertex pointers
};

#endif //_GRAPH_H