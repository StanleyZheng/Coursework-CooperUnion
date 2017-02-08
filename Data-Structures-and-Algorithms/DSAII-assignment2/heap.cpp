/*
    Stanley Zheng
    Data Structures and Algorithms II, Fall 2016
    Programming Assignment #2
    Due: October 18, 2016

    -   implementation of heap functions for the heap class
    -   using hashTable for mapping (for linear time search/lookup)
    -   
*/

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "hash.h"
#include "heap.h"
using namespace std;

// heap - The constructor allocates space for the nodes of the heap
// and the mapping (hash table) based on the specified capacity
heap::heap(int capacity){
    max_size = capacity;
    cur_size = 0;
    data = vector<node> (capacity + 1);
    mapping = new hashTable(capacity * 2);
}


//
// insert - Inserts a new node into the binary heap
//
// Inserts a node with the specified id string, key,
// and optionally a pointer. The key is used to
// determine the final position of the new node.
//
// Returns:
//   0 on success
//   1 if the heap is already filled to capacity
//   2 if a node with the given id already exists (but the heap
//     is not filled to capacity)
//
int heap::insert(const std::string &id, int key, void *pv){
    if (cur_size == max_size)
        return 1;
    if (mapping->contains(id)) //node already exists
        return 2;

    ++cur_size;
    mapping->insert(id, &data[cur_size]);
    data[cur_size].id = id;
    data[cur_size].key = key;
    data[cur_size].pData = pv;
    percolateUp(cur_size);
    return 0;
}


//
// setKey - set the key of the specified node to the specified value
//
// I have decided that the class should provide this member function
// instead of two separate increaseKey and decreaseKey functions.
//
// Returns:
//   0 on success
//   1 if a node with the given id does not exist
//
int heap::setKey(const std::string &id, int key){
    if (!(mapping->contains(id)))
        return 1;

    node *pn = static_cast<node *> (mapping->getPointer(id));
    int pos = getPos(pn);
    if (key > pn->key){
        pn->key = key;
        percolateDown(pos);
    }
    else{
        pn->key = key;
        percolateUp(pos);
    }
    return 0;
}


//
// deleteMin - return the data associated with the smallest key
//             and delete that node from the binary heap
//
// If pId is supplied (i.e., it is not NULL), write to that address
// the id of the node being deleted. If pKey is supplied, write to
// that address the key of the node being deleted. If ppData is
// supplied, write to that address the associated void pointer.
//
// Returns:
//   0 on success
//   1 if the heap is empty
//
int heap::deleteMin(std::string *pId, int *pKey, void *ppData){
    if (cur_size == 0)
        return 1;

    if (pId != NULL)
        *pId = data[1].id;
    if(pKey != NULL)
        *pKey = data[1].key;
    if(ppData != NULL)
        ppData = data[1].pData;
    
    mapping->remove(data[1].id);
    data[1] = data[cur_size];
    mapping->setPointer(data[1].id, &data[1]);
    
    --cur_size;
    percolateDown(1);
    return 0;
}


//
// remove - delete the node with the specified id from the binary heap
//
// If pKey is supplied, write to that address the key of the node
// being deleted. If ppData is supplied, write to that address the
// associated void pointer.
//
// Returns:
//   0 on success
//   1 if a node with the given id does not exist
//
int heap::remove(const std::string &id, int *pKey, void *ppData){
    if (!(mapping->contains(id)))
        return 1;

    node *pn = static_cast<node *> (mapping->getPointer(id));
    int index = getPos(pn);

    if (pKey != NULL)
        *pKey = pn->key;
    if (ppData != NULL)
        ppData = pn->pData;

    data[index] = data[cur_size];
    mapping->remove(id);

    --cur_size;
    mapping->setPointer(pn->id, pn);
    percolateUp(index);
    percolateDown(index);
    return 0;
}


//percolateUp - go up a node
void heap::percolateUp(int posCur){
    while (posCur / 2 > 0){
        int parent = posCur / 2;
        if (data[posCur].key < data[parent].key){
            node temp = data[posCur];
            data[posCur] = data[parent];
            data[parent] = temp;
            mapping->setPointer(data[posCur].id, &data[posCur]);
            mapping->setPointer(data[parent].id, &data[parent]);

            posCur = parent;
        }
        else
            break;
    }
    return;
}


//percolateDown - go down a node
void heap::percolateDown(int posCur){
    int child1, child2, smaller_child;

    while (posCur * 2 <= cur_size){
        child1 = posCur * 2;
        child2 = child1 + 1;
        if (child1 == cur_size)          //only has a right child
            smaller_child = child1;
        else
            smaller_child = (data[child1].key < data[child2].key ? child1 : child2);

        if (data[posCur].key > data[smaller_child].key){
            node temp = data[posCur];
            data[posCur] = data[smaller_child];
            data[smaller_child] = temp;
            mapping->setPointer(data[posCur].id, &data[posCur]);
            mapping->setPointer(data[smaller_child].id, &data[smaller_child]);

            posCur = smaller_child;
        }
        else
            break;
    }
    return;
}


//getPos - get the position of a node
int heap::getPos(node *pn){
    int pos = pn - &data[0];
    return pos;
}
