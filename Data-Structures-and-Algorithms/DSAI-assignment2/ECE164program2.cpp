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

/* Kevin Wong
 * ECE164
 * Programming Assignment 2
 */

Data* arr[1010000];

// if decimal points !=, then you're done.  otherwise, compare strings
int myCompare(string x, string y)
{
    int difference = x.find(".", 0) - y.find(".", 0);
    if (difference == 0)
    {
        return x.compare(y);
    }
    else
    {
        return (difference); // negative when x is smaller, positive when y is bigger
    }
}

//Quick Sort
void quicksort(int left, int right)
{
    //parititon
    int i = left, j = right, k = (left + right)/2;
    Data *v = arr[k];
    while (i <= j)
    {
        for (i; myCompare(arr[i]->data, v->data) < 0; ++i){}
        for (j; (myCompare(arr[j]->data, v->data) > 0); --j){}
        if (i <= j)
        {
            swap(arr[i],arr[j]);
            ++i;
            --j;
        }
    }
    
    //recursion
    if (left < j)
        quicksort(left, j);
    if (i < right)
        quicksort(i, right);
}

// for T3 only
int myCompareT3(string x, string y)
{
    const char *s1 = x.c_str();
    const char *s2 = y.c_str();
    int i = 0, diff, dot1 = 0, dot2 = 0;
    
    do
    {
        diff = s1[i] - s2[i];
        if (s1[i] == '\0' || s2[i] == '\0')
        {
            break;
        }
        if (s1[i] == '.')
        {
            dot1 = i;
        }
        if (s2[i] == '.')
        {
            dot2 = i;
        }
        ++i;
    } while (diff == 0);
    
    while (!(dot1 && dot2))
    {
        if (s1[i] == '.')
        {
            dot1 = i;
        }
        if (s2[i] == '.')
        {
            dot2 = i;
        }
        ++i;
    }
    
    if (dot1 == dot2)
    {
        return diff;
    }
    else
    {
        return (dot1 - dot2);
    }
}

// for T3 only
void quicksortT3(int left, int right)
{
    //parititon
    int i = left, j = right, k = (left + right)/2;
    Data *v = arr[k];
    while (i <= j)
    {
        for (i; myCompareT3(arr[i]->data, v->data) < 0; ++i){}
        for (j; (myCompareT3(arr[j]->data, v->data) > 0); --j){}
        if (i <= j)
        {
            swap(arr[i],arr[j]);
            ++i;
            --j;
        }
    }
    
    //recursion
    if (left < j)
        quicksortT3(left, j);
    if (i < right)
        quicksortT3(i, right);
}

//Insertion Sort
void insertionsort(int len)
{
    int i, j, k;
    Data *temp;
    for (i = 1; i <= len; ++i)
    {
        if ((arr[i]->data).compare(arr[i-1]->data) < 0)
        {
            temp = arr[i];
            k = i;
            for (j = i - 1; j >= 0; --j)
            {
                if ((temp->data).compare(arr[j]->data) >= 0)
                {
                    arr[k] = temp;
                    break;
                }
                else
                {
                    arr[k] = arr[j];
                    --k;
                }
            }
            if (k == 0)
            {
                arr[0] = temp;
            }
        }
    }
    return;
}

/************************************************* MAIN SORT DATA LIST FUNCITON *************************************************/
void sortDataList(list<Data *> &l) {
  // Fill in this function
  
  //load list into arr
  int i = 0;
  for (list<Data *>::const_iterator ipD = l.begin(); ipD != l.end(); ipD++) {
    arr[i] = *ipD;
    ++i;
  }
  
  int arrSize = l.size() - 1;
  
  //choose sort
  if (arrSize < 101000)
  {
    //T1
    quicksort(0, arrSize);
  }
  else if ((arr[0]->data).find(".", 0) < 5)
  {
    //T3
    quicksortT3(0, arrSize);
  }
  else if ((arr[0]->data).substr(0, 7) == (arr[1]->data).substr(0, 7))
  {
    //T4
    insertionsort(arrSize);
  }
  else
  {
    //T2
    quicksort(0, arrSize);
  }
  
  //write sorted arr to list
  i = 0;
  for (list<Data *>::iterator ipD = l.begin(); ipD != l.end(); ipD++) {
    *ipD = arr[i];
    ++i;
  }
}

/************************************************* OLD, IRRELEVANT CODE PRESERVED HERE *************************************************/

//int bin[1000000] = {0};
//Data* radArr[12][1000000];

/*
//Quick Sort OLD
void quicksort(int left, int right)
{
    if (left >= right)
    {
        return;
    }
    //parititon
    int i = left, j = right, k = (left + right)/2;
    Data *v = arr[k];
//    swap(arr[k], arr[right]);
//    cout << arr[k]->data << " " << v->data << " " << arr[right]->data << endl;
    while (true)
    {
        for (i; myCompare(arr[i]->data, v->data) < 0; ++i){}
        for (j; (myCompare(arr[j]->data, v->data) > 0) && (j > 0); --j){}
        if (i >= j)
        {
//            cout << "hi" << endl;
            break;
        }
        swap(arr[i],arr[j]);
//        cout << k << "\n" << i << "\n" << j << "\n" << arr[i]->data << " swapped with " << arr[j]->data << endl;
    }
//    swap(arr[i],arr[right]);
    
    //recursion
    quicksort(left, i - 1);
    quicksort(i + 1, right);
}*/

/*
string float_to_string(float f) //for bin sort
{
    ostringstream s;
    s << f;
    if (f == 0)
    {
        s << ".0";
    }
    else if (!(((int) (f * 1000 + 0.5)) % 1000))
    {
        s << ".0";
    }
    return s.str();
}

//Bin Sort
void binsort(int len)
{
    int i, temp, j, k = 0;
    
    //loads arr into bin after converting to ints
    for (i = 0; i <= len; ++i)
    {
        temp = (int) (atof((arr[i]->data).c_str()) * 1000 + 0.5);
//        cout << temp << endl;
        ++bin[temp];
//        cout << i << " is i, and bin[i] is " << bin[i] << endl;
    }
    
    for (i = 0; i <= len; ++i)
    {
        if (i + k > len)
        {
            break;
        }
//        cout << i << endl;
        j = bin[i];
//        cout << j << " " << k << endl;
        while (j > 0)
        {
            arr[i + k]->data = float_to_string(((float) i) / 1000);
            --j;
            ++k;
        }
        --k;
    }
    
    return;
}*/

/*
//Radix Sort
void radixsort(int len)
{
    string tempVal;
    int digit, i, j, k[12] = {0}, s, t;
    for (digit = 1; digit <= 7; ++digit)
    {
//        cout << digit << endl;
        for (i = 0; i < 12; ++i)
        {
            k[i] = 0; //reset k
        }
//        cout << "k reset to " << k[11] << " and arr[0] is " << arr[0] << endl;
        
        for (i = 0; i <= len; ++i)
        {
//            cout << "i is " << i << " in the for loop, and arr[i] is " << arr[i] << endl;
            tempVal = arr[i]->data;
//            cout << tempVal << endl;
            j = tempVal[tempVal.length() - digit];
//            cout << j << endl;
            if (j > 47)
            {
                radArr[j - 46][k[j - 46]] = arr[i];
                ++k[j - 46];
            }
            else if (j = 46) //decimal point
            {
                radArr[1][k[1]] = arr[i];
                ++k[1];
            }
            else // j = 32 (space)
            {
                radArr[0][k[0]] = arr[i];
                ++k[0];
            }
            
        }
        
        //rewrite to arr
        i = 0;
        for (s = 0; s < 12; ++s)
        {
//            cout << "s is " << s << " and k[s] is " << k[s] << endl;
            t = 0;
            while (k[s])
            {
                arr[i] = radArr[s][t];
//                cout << arr[i]->data << endl;
                ++t;
                --k[s];
                ++i;
            }
        }
    }
    
    return;
}*/