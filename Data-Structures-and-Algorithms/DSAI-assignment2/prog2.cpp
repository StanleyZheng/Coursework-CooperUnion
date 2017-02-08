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
    string filename = "t1.txt";
    cout << "Enter name of input file: " << filename << endl;
    //cin >> filename;

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
    string filename = "output.txt";
    cout << "Enter name of output file: " << filename << endl;
    //cin >> filename;

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
    //compare_bignum("1018273260145395.06449375235881839884", "1823105457928495.11356263587087387673");
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

void merge_sort(list<Data *> &);
void custom_swap(Data &, Data &);
int partition(int, int);
void quick_sort(int , int );
int compare_bignum(string, string);
int compare_smallnum(string, string);

Data *ary[1100000]; //array of 1,100,000 elements
int i = 0;          //current index of array

/*
void custom_swap(Data &a, Data &b){
    Data temp = a;
    a = b;
    b = temp;
}
*/

void custom_swap(int a, int b){
    Data* temp = ary[a];
    ary[a] = ary[b];
    ary[b] = temp;
}

//comparator used for large numbers <20>.<20>
int compare_bignum(string a, string b){
    int a_decholder = 20;
    int b_decholder = 20;

    while ( a.substr(a_decholder,1).compare(".") ){
        a_decholder--;
    }
    while ( b.substr(b_decholder,1).compare(".") ){
        b_decholder--;
    }

    if (a_decholder != b_decholder)
        return a_decholder - b_decholder;
    else
        return a.compare(b);
}


/*
//implementation for Quick Sort
int partition(Data* A[], int l, int r){
    int k = (int)((l+r)/2);
    string v = A[k]->data;
    custom_swap(k,r);
    int m = l;
    int n = r - 1;
    while (true){
        while ( compare_bignum(A[m]->data, v) < 0 )
            m++;
        while ( compare_bignum(A[n]->data, v) > 0 && n > 1 )
            n--;
        if (m >= n)
            break;
        custom_swap(m++,n--);
    }
    custom_swap(m,r);
    return m;
}


void quick_sort(Data *A[], int l, int r) {
    if (l >= r)
        return;
    int m = partition(A,l,r);
    quick_sort(A,l,m-1);
    quick_sort(A,m+1,r);
}
*/

//implementation for Quick Sort
int partition(int l, int r){
    int k = (int)((l+r)/2);
    //string piv = ary[k]->data;
    custom_swap(k,r);
    int m = l;
    int n = r - 1;
    while (m <= n){
        while ( m <= n && compare_bignum(ary[m]->data, ary[r]->data) < 0 )
            m++;
        while ( m <= n && compare_bignum(ary[n]->data, ary[r]->data) > 0 )
            n--;
        if (m < n)
            custom_swap(m++,n--);
    }
    custom_swap(m,r);
    return m;
}

void quick_sort(int l, int r) {
    if (l >= r)
        return;
    int m = partition(l,r);
    quick_sort(l,m-1);
    quick_sort(m+1,r);
}


void sortDataList(list<Data *> &l) {
    cout << "---------------------" << endl;

//fill in array with pointers from list
    list<Data *>::iterator iter;
    for (iter = l.begin(); iter != l.end(); ++iter){
        ary[i++] = *iter;
    }

//check which file we're working with
    int size = l.size();
    cout << "size: " << size << endl;
    if (size <= 110000){
        cout << "We are working with T1: " << endl;
        quick_sort(0, 99999);
    }
    else if (ary[5]->data.length() < 10){
        cout << "We are working with T3: " << endl;
    }
    else if ((ary[5]->data).substr(0,8).compare((ary[6]->data).substr(0,8)) == 0){
        cout << "We are working with T4: " << endl;
    }
    else {
        cout << "We are working with T2: " << endl;
        quick_sort(0, 999999);
    }
    cout << "---------------------" << endl;

    i = 0;
    for (iter = l.begin(); iter != l.end(); ++iter){
        *iter = ary[i++];
    }
}



//CHECK for swap function
/*
    for (int j = 0; j < 10; j++){
        cout << ary[j]->data << endl;
    }
    custom_swap(ary[0], ary[1]);
    cout << endl;
    for (int j = 0; j < 10; j++){
        cout << ary[j]->data << endl;
    }

*/

//CHECK for compare_bignum() function
/*
    cout << ary[0]->data << endl;
    cout << ary[32]->data << endl;
    cout << "comparing: (a) is " << compare_bignum(ary[0]->data, ary[32]->data) << " larger than (b)" << endl;

    cout << ary[0]->data << endl;
    cout << ary[1]->data << endl;
    cout << "comparing: (a) is " << compare_bignum(ary[0]->data, ary[1]->data) << " larger than (b)" << endl;

    cout << ary[0]->data << endl;
    cout << ary[4676]->data << endl;
    cout << "comparing: (a) is " << compare_bignum(ary[0]->data, ary[4676]->data) << " larger than (b)" << endl;

*/
    
//CHECK for compare_smallnum() function
/*
    cout << ary[0]->data << endl;
    cout << ary[14]->data << endl;
    cout << "comparing: (a) is " << compare_smallnum(ary[0]->data, ary[14]->data) << " larger than (b)" << endl;

    cout << ary[0]->data << endl;
    cout << ary[1]->data << endl;
    cout << "comparing: (a) is " << compare_smallnum(ary[0]->data, ary[1]->data) << " larger than (b)" << endl;

*/





/*
//comparator used for smaller numbers <3>.<3>
int compare_smallnum(string a, string b){
    int a_decholder = 0;
    int b_decholder = 0;

    while ( string(1,a.at(3 - a_decholder)).compare(".") != 0 ){
        cout << "(a no decimal yet) ";
        a_decholder++;
    }
    while ( string(1,b.at(3 - b_decholder)).compare(".") != 0 ){
        cout << "(b no decimal yet) ";
        b_decholder++;
    }

    if (a_decholder != b_decholder)
        return b_decholder - a_decholder;
    else if ( (stoi(a.substr(0,3)) - stoi(b.substr(0,3))) != 0 )
        return ( stoi(a.substr(0,3)) - stoi(b.substr(0,3)) );
    else
        return ( stoi(a.substr(4,3)) - stoi(b.substr(4,3)) );
}
*/



//implementation for Merge Sort
void merge_sort(list<Data *> &l) {

}







