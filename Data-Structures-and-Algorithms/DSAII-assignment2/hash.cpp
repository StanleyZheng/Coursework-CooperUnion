/*
    Stanley Zheng
    Data Structures and Algorithms II, Fall 2016
    Programming Assignment #1
    Due: September 27, 2016

    -   hashTable functions for the hashTable class
    -   missing getpointer() and setpointer() functions
    -   used the provided hash function from the textbook
    -   using linear probing
*/


#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "hash.h"
using namespace std;

// The constructor initializes the hash table.
// Uses getPrime to choose a prime number at least as large as
// the specified size for the initial size of the hash table.
hashTable::hashTable(int size){
    capacity = getPrime(size);
    filled = 0;
    data = vector<hashItem> (capacity);
}

// Insert the specified key into the hash table.
// If an optional pointer is provided,
// associate that pointer with the key.
// Returns 0 on success,
// 1 if key already exists in hash table,
// 2 if rehash fails.
int hashTable::insert(const string &key, void *pv){
    if (filled > (capacity / 2) && !rehash())
        return 2;

    int num = hash(key);
    while (data[num].isOccupied == true){
        if (data[num].isDeleted == false && data[num].key == key)
            return 1;
        ++num;
        if (num == capacity)
            num = 0;
    }

    data[num].isOccupied = true;
    data[num].key = key;
    data[num].pv = pv;
    ++filled;
    return 0;
}

// Check if the specified key is in the hash table.
// If so, return true; otherwise, return false.
bool hashTable::contains(const string &key){
    if (findPos(key) == -1)
        return false;
    return true;
}


// Get the pointer associated with the specified key.
// If the key does not exist in the hash table, return NULL.
// If an optional pointer to a bool is provided,
// set the bool to true if the key is in the hash table,
// and set the bool to false otherwise.
void* hashTable::getPointer(const std::string &key, bool *b){
    int num = findPos(key);
    if (num < 0){
        if (b != NULL)
            *b = false;
        return NULL;
    }
    else {
        if (b != NULL)
            *b = true;
        return data[num].pv;
    }
}



// Set the pointer associated with the specified key.
// Returns 0 on success,
// 1 if the key does not exist in the hash table.
int hashTable::setPointer(const std::string &key, void *pv){
    int num = findPos(key);
    if (num < 0){
        return 1;
    }
    else {
        data[num].pv = pv;
        return 0;
    }
}



// Delete the item with the specified key.
// Returns true on success,
// false if the specified key is not in the hash table.
bool hashTable::remove(const string &key){
    int num = findPos(key);
    if (num < 0)
        return false;
    data[num].isDeleted = true;
    return true;
}

// The hash function.
int hashTable::hash(const string &key){
    int hashVal = 0;

    for (int i = 0; i < key.length(); i++)
        hashVal = 37 * hashVal + key[i];

    hashVal %= capacity;
    if (hashVal < 0)
        hashVal += capacity;

    return hashVal;
}

// Search for an item with the specified key.
// Return the position if found, -1 otherwise.
int hashTable::findPos(const string &key){
    int num = hash(key);
    while (data[num].isOccupied == true){
        if (data[num].isDeleted == false && data[num].key == key)
            return num;
        else 
            num++;
    }
    return -1;
}

// The rehash function; makes the hash table bigger.
// Returns true on success, false if memory allocation fails.
bool hashTable::rehash(){
    int old_capacity = capacity;
    capacity = getPrime(capacity);
    vector<hashItem> temp = data;

    try{
        data = vector<hashItem> (capacity);
    }
    catch(...){
        return false;
    }

    int num, newNum;
    for (num = 0; num < old_capacity; ++num){
        if (temp[num].isOccupied == true && temp[num].isDeleted == false)
            insert(temp[num].key, temp[num].pv);
    }
    return true;
}

// Return a prime number at least as large as size.
// Uses a precomputed sequence of selected prime numbers.
unsigned int hashTable::getPrime(int size){
    int primeNumbers[] = {24989,50021,100043,200087,400187,800399,1600811,3201641};
    int i = 0;
    while (size >= primeNumbers[i])
        ++i;
    return primeNumbers[i];
}
