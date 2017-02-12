/*
    Stanley Zheng
    Data Structures and Algorithms II, Fall 2016
    Programming Assignment #3
    Due: November 15, 2016

    -   implementation of Dijkstra's algorithm to solve the single-source shortest path problem.
    -   the program will determine the shortest path from a 'source' or starting vertex
        to each other vertex in the graph.
    -   using hashTable for mapping (for linear time search/lookup)
    -   using a Bineary Heap to get the shortest distances (w/heap::deleteMin())

*/
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "hash.h"
#include "heap.h"
#include "graph.h"
using namespace std;


//constructors for graph, vertex, and edge
graph::graph(int size){
    mapping = new hashTable(size * 2);
}

graph::vertex::vertex(string input_id){
    id = input_id;
    dist = -1;                      //we are given that all edge costs are positive
    known = false;
    prev = NULL;
}

graph::vertex::edge::edge(vertex* input_destin, int input_cost){
    destin = input_destin;
    cost = input_cost;
}



//*** Functions for graph ***//
//
//contains() - returns true if id exists in hashTable
bool graph::contains(string id){
    return mapping->contains(id);
}

//
//insert()  - inserts source and destination into graph if they do not exist
//          - inserts an edge into the source's adjacency list
void graph::insert(string source, string destination, int edge_cost){
    if (!contains(source)){
        data.push_back(vertex(source));
        mapping->insert(source, &data.back());
    }
    if (!contains(destination)){
        data.push_back(vertex(destination));
        mapping->insert(destination, &data.back());
    }
    
    vertex *ps = static_cast<vertex *> (mapping->getPointer(source));
    vertex *pd = static_cast<vertex *> (mapping->getPointer(destination));
    ps->adjList.push_back(vertex::edge(pd, edge_cost));
    return;
}

//
//dijkstra() - given a starting vertex, finds the shortest path to all other vertices
//           - distance from original vertex is stored as dist in each vertex
void graph::dijkstra(string start_vertex){
    vertex *v = static_cast<vertex *> (mapping->getPointer(start_vertex));
    //set the distance and path of the starting vertex
    v->dist = 0;
    v->prev = v;
    vertex *adjacentv;                  //adjacent vertex
    heap myHeap(data.size());           //initialize myHeap with the size of data, one slot for each vertex
    int newpathcost;                    //newpathcost = v->dist + iter->cost

    //set the distance from starting vertix, and parent path
    while(1){
        if (v->known == false){
            //add all adjacent vertices into the heap, update their values
            for(list<vertex::edge>::iterator iter = v->adjList.begin(); iter != v->adjList.end(); iter++){
                adjacentv = iter->destin;
                newpathcost = v->dist + iter->cost;
                if (adjacentv->known == false){
                    //if not set yet, set now       //update dist with the lower value
                    if ((adjacentv->dist == -1) || (adjacentv->dist > newpathcost)){
                        adjacentv->dist = newpathcost;
                    }
                    else {  //if v->dist + iter->cost >= adjacent's dist, do nothing/move on
                        continue;
                    }
                    //update the path
                    adjacentv->prev = v;

                    //insert new node into heap, if it already exists update its key
                    if (myHeap.insert(adjacentv->id, adjacentv->dist, adjacentv) == 2)
                        myHeap.setKey(adjacentv->id, adjacentv->dist);
                }
            }
            v->known = true;
        }

        //update v and break when heap is empty
        if (myHeap.deleteMin(NULL, NULL, &v) == 1)
            break;
    }
    return;
}

//
//printPaths() - prints the paths of each vertex and prints it to output
void graph::printPaths(string start_vertex, ofstream &output){
    vertex *v = static_cast<vertex *> (mapping->getPointer(start_vertex));
    for(list<vertex>::iterator iter = data.begin(); iter != data.end(); iter++){
        output << iter->id << ": ";
        if (iter->dist == -1){
            output << "NO PATH" << endl;
            continue;
        }
        output << iter->dist << " [";
        if (iter->id == start_vertex){
            output << start_vertex << "]" << endl;
            continue;
        }
        string temp = iter->id;
        vertex *previous = iter->prev;
        while(previous != v){
            temp = previous->id + ", " + temp;
            previous = previous->prev;
        }
        temp = v->id + ", " + temp;
        output << temp << "]" << endl;
    }
}



