/*
    Stanley Zheng
    Data Structures and Algorithms II, Fall 2016
    Programming Assignment #3
    Due: November 15, 2016

    -   implementation of Dijkstra's algorithm to solve the single-source shortest path problem.
    -   asks user to enter: 
                            a file specifying the graph
                            a starting vertex
                            an output file to write to
    -   all edge costs are > 0 and < 1 Million
    -   assumes file is valid if it can be opened.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "heap.h"
#include "graph.h"

using namespace std;

int main(){
    string infile, outfile;
    string start_vertex;
    clock_t t1, t2;
    double timeDiff;


    //open and read graph file
    cout << "Enter name of graph file: ";
    cin >> infile;
    ifstream input(infile.c_str());
    if(!input){
        cerr << "Error: could not open " << infile << endl;
        exit(1);
    }


    //load graph.txt into hash table
    graph myGraph = graph(1000000); //mapping size of 1 million
    string line, source, destin;
    int edge_cost;
    while(getline(input, line)){
        //used stringstream to parse the line from file
        stringstream strline;
        strline << line;            //parse line into strline
        strline >> source;
        strline >> destin;
        strline >> edge_cost;

        myGraph.insert(source, destin, edge_cost);
    }


    //have user input valid vertex
    do {
        cout << "Enter a valid vertex id for the string vertex: ";
        cin >> start_vertex;
    } while(!(myGraph.contains(start_vertex))); //while the heap constains string returns false



    //perform dijkstras algorithm on the Graph
    t1 = clock();
    myGraph.dijkstra(start_vertex);
    t2 = clock();
    timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC;
    cout << "Total time (in seconds) to apply Dijkstra's algorithm: " << timeDiff << endl;


    //open and write to output file
    cout << "Enter name of output file: ";
    cin >> outfile;
    ofstream output(outfile.c_str());
    if(!output){
        cerr << "Error: could not open " << outfile << endl;
        exit(1);
    }
    
    //write to output file
    myGraph.printPaths(start_vertex, output);

    //close files;
    input.close();
    output.close();

    return 0;
}