/*
    Stanley Zheng
    Data Structures and Algorithms I, Spring 2016
    Programming Assignment #2
    Due: May 4, 2016

    -   An attempt to optimize the cpu time of sorting various lists
    -   Provided code creates the list of items to sort from text file and outputs sorted
        list back into a text file
    -   Additional code sorts the created list (mainly used bin sort and insertion sort)
*/

// THIS IS THE PROVIDED CODE FOR PROGRAM #2, DSA 1, SPRING 2016

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

// A simple class; each object holds one string
class Data {
public:
  string data;
  Data(const string &s) { data = s; }
};

// Load the data from a specified input file
void loadDataList(list<Data *> &l) {
  string filename;
  cout << "Enter name of input file: ";
  cin >> filename;

  ifstream input(filename.c_str());
  if (!input) {
    cerr << "Error: could not open " << filename << endl;
    exit(1);
  }

  // The first line indicates the size
  string line;
  getline(input, line);
  stringstream ss(line);
  int size;
  ss >> size;

  // Load the data
  for (int i = 0; i < size; i++) {
    string line;
    getline(input, line);
    l.push_back(new Data(line));
  }

  input.close();
}

// Output the data to a specified input file
void writeDataList(const list<Data *> &l) {
  string filename;
  cout << "Enter name of output file: ";
  cin >> filename;

  ofstream output(filename.c_str());
  if (!output) {
    cerr << "Error: could not open " << filename << endl;
    exit(1);
  }

  // Write the size first
  int size = l.size();
  output << size << endl;

  // Write the data
  for (list<Data *>::const_iterator ipD = l.begin(); ipD != l.end(); ipD++) {
    output << (*ipD)->data << endl;
  }

  output.close();
}

void sortDataList(list<Data *> &);

// The main function calls routines to get the data, sort the data,
// and output the data. The sort is timed according to CPU time.
int main() {
  list<Data *> theList;
  loadDataList(theList);

  cout << "Data loaded.  Executing sort..." << endl;

  clock_t t1 = clock();
  sortDataList(theList);
  clock_t t2 = clock();
  double timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC;

  cout << "Sort finished. CPU time was " << timeDiff << " seconds." << endl;

  writeDataList(theList);
}

// -------------------------------------------------
// YOU MAY NOT CHANGE OR ADD ANY CODE ABOVE HERE !!!
// -------------------------------------------------

// You may add global variables, functions, and/or
// class defintions here if you wish.


// -------------------------------------------------
// THIS IS THE ADDITIONAL CODE FOR THE ASSIGNMENT

void insertion_sort(list<Data *> &);
bool comp_T1(Data *a, Data *b);
bool comp_T1a(Data *a, Data *b);
bool comp_T3(Data *a, Data *b);
bool comp_T4(Data *a, Data *b);
Data *ary[1010000]; //array of 1,010,000 elements

Data *bins [100000][30];        //bins to help sort, up to multiplicity 30 (of first 5 digits)
int num_filled[100000];         //helper array to keep track of where to put the multiples from bin
int a,b,c,d,e,sum;              //letters used for bin sort, abc.de_ & abcde___
string tempstr;


void sortDataList(list<Data *> &l) {
    list<Data *>::iterator iter = l.begin();
    int size = l.size();

// Checks to see which file we're working with (T1, T2, T3, T4)
    // T1 SORT
    if (size <= 110000){
        cout << "T1: " << endl;
        for (iter = l.begin(); iter != l.end(); iter++) {
            tempstr = (*iter)->data;
            a = b = c = d = e = sum = 0;

            if (tempstr[20] == '.') {
                a = tempstr[0] - 48;
                b = tempstr[1] - 48;
                c = tempstr[2] - 48;
                d = tempstr[3] - 48;
                e = tempstr[4] - 48;
            }
            else if(tempstr[19] == '.') {
                b = tempstr[0] - 48;
                c = tempstr[1] - 48;
                d = tempstr[2] - 48;
                e = tempstr[3] - 48;
            }
            else if(tempstr[18] == '.') {
                c = tempstr[0] - 48;
                d = tempstr[1] - 48;
                e = tempstr[2] - 48;
            }
            else if(tempstr[17] == '.') {
                d = tempstr[0] - 48;
                e = tempstr[1] - 48;
            }
            else if(tempstr[16] == '.') {
                e = tempstr[0] - 48;
            }
            sum = 10000*a + 1000*b + 100*c + 10*d + e;
            bins[sum][num_filled[sum]] = *iter;
            num_filled[sum]++;
        }

        for (int i = 0; i<100000; i++) {
            sort( bins[i], bins[i] + num_filled[i], comp_T1 );
        }
        
        iter = l.begin();
        for (int i = 0; i < 100000; i++) {
            for (int j = 0; j< num_filled[i]; j++) {
                *iter = bins[i][j];
                iter++;
            }
        }
    }//ends check for T1 Case

    // T3 SORT
    else if ((*iter)->data.length() < 8){
        cout << "T3" << endl;
        int stringsz;
        for (iter = l.begin(); iter != l.end(); iter++) {
            tempstr = (*iter)->data;
            stringsz = tempstr.length();
            a = b = c = d = e = sum = 0;

            if (tempstr[3] == '.') {
                a = tempstr[0] - 48;
                b = tempstr[1] - 48;
                c = tempstr[2] - 48;
                d = tempstr[4] - 48;
                if(stringsz > 5)
                    e = tempstr[5] - 48;
            }
            else if(tempstr[2] == '.') {
                b = tempstr[0] - 48;
                c = tempstr[1] - 48;
                d = tempstr[3] - 48;
                if(stringsz > 4)
                    e = tempstr[4] - 48;
            }
            else {
                c = tempstr[0] - 48;
                d = tempstr[2] - 48;
                if(stringsz > 3)
                    e = tempstr[3] - 48;
            }

            sum = 10000*a + 1000*b + 100*c + 10*d + e;
            bins[sum][num_filled[sum]] = *iter;
            num_filled[sum]++;
        }
        
        for (int i = 0; i<100000; i++) {
            sort( bins[i], bins[i] + num_filled[i], comp_T3 );
        }
        
        iter = l.begin();
        for (int i = 0; i < 100000; i++) {
            for (int j = 0; j< num_filled[i]; j++) {
                *iter = bins[i][j];
                iter++;
            }
        }
    }//ends T3 Case

    // T4 SORT
    else if ( (*l.begin())->data.substr(0,8) == (*(++l.begin()))->data.substr(0,8) ){
        cout << "T4: " << endl;
        insertion_sort(l);
    }//ends T4 Case

    // T2 SORT
    else {
        cout << "T2: " << endl;
        for (iter = l.begin(); iter != l.end(); iter++) {
            tempstr = (*iter)->data;
            a = b = c = d = e = sum = 0;

            if (tempstr[20] == '.') {
                a = tempstr[0] - 48;
                b = tempstr[1] - 48;
                c = tempstr[2] - 48;
                d = tempstr[3] - 48;
                e = tempstr[4] - 48;
            }
            else if(tempstr[19] == '.') {
                b = tempstr[0] - 48;
                c = tempstr[1] - 48;
                d = tempstr[2] - 48;
                e = tempstr[3] - 48;
            }
            else if(tempstr[18] == '.') {
                c = tempstr[0] - 48;
                d = tempstr[1] - 48;
                e = tempstr[2] - 48;
            }
            else if(tempstr[17] == '.') {
                d = tempstr[0] - 48;
                e = tempstr[1] - 48;
            }
            else if(tempstr[16] == '.') {
                e = tempstr[0] - 48;
            }
            sum = 10000*a + 1000*b + 100*c + 10*d + e;
            bins[sum][num_filled[sum]] = *iter;
            num_filled[sum]++;
        }

        sort( bins[0], bins[0] + num_filled[0], comp_T1a );
        for (int i = 1; i<100000; i++) {
            sort( bins[i], bins[i] + num_filled[i], comp_T1 );
        }
        
        iter = l.begin();
        for (int i = 0; i < 100000; i++) {
            for (int j = 0; j< num_filled[i]; j++) {
                *iter = bins[i][j];
                iter++;
            }
        }
    }// ends T2 Case
}

//Comparison functions for sort() --> return true if a < b
bool comp_T1(Data *a, Data *b){
    return (a->data.compare(b->data) < 0);
}
bool comp_T1a(Data *a, Data *b){
    int a_decholder = 15;
    int b_decholder = 15;
    while ( a->data[a_decholder] != '.' ){
        a_decholder--;
    }
    while ( b->data[b_decholder] != '.' ){
        b_decholder--;
    }
    if (a_decholder != b_decholder)
        return (a_decholder - b_decholder < 0);
    else
        return (a->data.compare(b->data) < 0);
}

bool comp_T3(Data *a, Data *b){
    return (a->data.compare(b->data) < 0);
}

bool comp_T4(Data *a, Data *b){
    return (a->data.compare(b->data) < 0);
}


// Insertion sort for T4
void insertion_sort(list<Data *> &l){
    int i = 0;          //current index of array
    int j = 1;          //used to keep track of how far to insert element

    //put the unsorted list into an array
    list<Data *>::iterator iter;
    for (iter = l.begin(); iter != l.end(); ++iter){
        ary[i++] = *iter;
    }

    //insertion sort with half swaps
    for (int i = 1; i < l.size(); i++){
        j = 1;
        while ( (i - j >= 0) && comp_T4(ary[i],ary[i-j]) ){
            j++;
        }
        if (j > 1){
            Data *temp = ary[i];
            int k = j;
            for (; j > 1; j--){
                ary[i + j - k] = ary[i + j - k - 1];
            }
            ary[i - k + 1] = temp;
        }
    }

    //put the sort array back into the list
    i = 0;
    for (iter = l.begin(); iter != l.end(); ++iter){
        *iter = ary[i++];
    }
    return;
}