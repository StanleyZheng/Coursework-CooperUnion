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


// MY CODE
Data *sortthis[1010000];
Data *now;
Data *next;
// Data **nodeloc[1010000];
Data *bins [100000][30];
int bins_filled[100000];
int decposT4;

void quicksort(Data *array[], int begin, int end, int loc);
bool T3comp(Data *a, Data *b);
bool T2comp(Data *a, Data *b);
bool T2comp0 (Data *a, Data*b);
bool T4compout(Data *a, Data*b);
bool T4compin(Data *a, Data*b);


void sortDataList(list<Data *> &l) {
	// Fill in this function
	
	int size = l.size();
	
	if ( (**(l.begin())).data.size() < 8 ) {
		cout << "T3" << endl;
		list<Data *>::iterator itr = l.begin();
		int ms, sms, tms, fms, ffms, stringsize;
		
		// fill up the bins
		for (int i = 0; i<size; i++) {
			now = *itr;
			stringsize = now->data.size();
			if (now->data[3] == '.') {
				ms = now->data[0] - 48;
				sms = now->data[1] - 48;
				tms = now->data[2] - 48;
				fms = now->data[4] - 48;
				if(stringsize > 5)
					ffms = now->data[5] - 48;
				else
					ffms = 0;
			}
			else if(now->data[2] == '.') {
				ms = 0;
				sms = now->data[0] - 48;
				tms = now->data[1] - 48;
				fms = now->data[3] - 48;
				if(stringsize > 4)
					ffms = now->data[4] - 48;
				else
					ffms = 0;
			}
			else {
				ms = 0;
				sms = 0;
				tms = now->data[0] - 48;
				fms = now->data[2] - 48;
				if(stringsize > 3)
					ffms = now->data[3] - 48;
				else
					ffms = 0;
			}
			ms = ms*10000+sms*1000+tms*100+fms*10+ffms;
			bins[ms][bins_filled[ms]] = now;
			bins_filled[ms]++;
			itr++;
		}
		
		for(int i = 0; i<100000; i++) {
			// sort each bin
			sort(bins[i],bins[i]+bins_filled[i],T3comp);
		}
		
		itr = l.begin();
		for (int i =0; i<100000; i++) {
			for(int j = 0; j<bins_filled[i]; j++) {
				*itr = bins[i][j];
				itr++;
			}
		}
	}
	else if( size < 110000 ) {
		cout << "T1" << endl;
		goto T2;
	}
	else if( (**(l.begin())).data.substr(0,8) == (**(++l.begin())).data.substr(0,8)) {
		cout << "T4" << endl;
		list<Data *>::iterator itr = l.begin();
		
		// list<Data *>::iterator sitr = l.begin();
		// for (int i = 0; i<size-1; i++) {
			// now = *itr++;
			// next = *itr;
			
			// if (T4compout(next,now)) {
				// sitr = itr;
				// sitr--;
				// sitr--;
				
				// while (true) {
					// if (T4compin(*sitr,next)) {
						// l.insert(++sitr, next);
						// itr = l.erase(itr);
						// itr--;
						// break;
					// }
					// else {
						// sitr--;
					// }
				// }
			// }
		// }
		
		for (int i = 20; i>0; i--) {
			if((*itr)->data[i] == '.') {
				decposT4 = i-1;
				break;
			}				
		}
		
		for (int i = 0; i<size; i++ ) {
			sortthis[i] = *itr;
			itr++;
		}
		for (int i = 0; i<size-1; i++) {
			if (T4compout(sortthis[i+1], sortthis[i])) {
				now = sortthis[i+1];
				sortthis[i+1] = sortthis[i];
				
				for (int j = 1;;j++) {
					if(T4compin(now,sortthis[i-j])) {
						sortthis[i-j+1] = sortthis[i-j];
					}
					else {
						sortthis[i-j+1] = now;
						break;
					}
				}
			}
		}
		itr = l.begin();
		for (int i = 0; i< size; i++) {
			*itr = sortthis[i];
			itr++;
		}
	}
	else {
		cout << "T2" << endl;
		T2:
		list<Data *>::iterator itr = l.begin();
		int ms, sms, tms, fms, ffms, stringsize;
		
		// fill up the bins
		for (int i = 0; i<size; i++) {
			now = *itr;
			stringsize = now->data.size();
			if (now->data[20] == '.') {
				ms = now->data[0] - 48;
				sms = now->data[1] - 48;
				tms = now->data[2] - 48;
				fms = now->data[3] - 48;
				ffms = now->data[4] - 48;
			}
			else if(now->data[19] == '.') {
				ms = 0;
				sms = now->data[0] - 48;
				tms = now->data[1] - 48;
				fms = now->data[2] - 48;
				ffms = now->data[3] - 48;
			}
			else if (now->data[18] == '.') {
				ms = 0;
				sms = 0;
				tms = now->data[0] - 48;
				fms = now->data[1] - 48;
				ffms = now->data[2] - 48;
			}
			else if (now->data[17] == '.') {
				ms = 0;
				sms = 0;
				tms = 0;
				fms = now->data[0] - 48;
				ffms = now->data[1] - 48;
			}
			else if (now->data[16] == '.') {
				ms = 0;
				sms = 0;
				tms = 0;
				fms = 0;
				ffms = now->data[0] - 48;
			}
			else {
				ms = 0;
				sms = 0;
				tms = 0;
				fms = 0;
				ffms = 0;
			}
				
			ms = ms*10000+sms*1000+tms*100+fms*10+ffms;
			bins[ms][bins_filled[ms]] = now;
			bins_filled[ms]++;
			itr++;
		}
		
		sort(bins[0],bins[0]+bins_filled[0],T2comp0);
		for(int i = 1; i<100000; i++) {
			// sort each bin
			sort(bins[i],bins[i]+bins_filled[i],T2comp);
		}
		
		itr = l.begin();
		for (int i =0; i<100000; i++) {
			for(int j = 0; j<bins_filled[i]; j++) {
				*itr = bins[i][j];
				itr++;
			}
		}
		
	}
}
char s;
bool T3comp(Data *a, Data*b) { //less than
	if (a->data[2] < b->data[2])
		return true;
	else if (a->data[2] > b->data[2])
		return false;
	if (a->data[3] < b->data[3])
		return true;
	else if (a->data[3] > b->data[3])
		return false;
	if (a->data[4] < b->data[4])
		return true;
	else if (a->data[4] > b->data[4])
		return false;
	if (a->data[5] < b->data[5])
		return true;
	else if (a->data[5] > b->data[5])
		return false;
	if (a->data[6] < b->data[6])
		return true;
	else if (a->data[6] > b->data[6])
		return false;
	return false;
}

bool T2comp (Data *a, Data*b) {
	for (int i = 0; i<41; i++) {
		if (a->data[i] < b->data[i])
			return true;
		else if (a->data[i] > b->data[i])
			return false;
	}
}
int decimala, decimalb, hits;
bool T2comp0 (Data *a, Data*b) {
	decimala=0;
	decimalb=0;
	hits=0;
	for (int i = 20; i > 0; i--) {
		if (a->data[i] == '.') {
			decimala = i;
			hits++;
		}
		if (b->data[i] == '.') {
			decimalb = i;
			hits++;
		}
		if (hits == 2) 
			break;
	}
	if (decimala == decimalb) {
		for (int i = 0; i<41; i++) {
			if (a->data[i] < b->data[i])
				return true;
			else if (a->data[i] > b->data[i])
				return false;
		}
		return false;
	}
	else {
		return decimala < decimalb;
	}
}
bool T4compout(Data *a, Data*b) {
	if (a->data[decposT4]+1 == b->data[decposT4])
		return true;
	else if (a->data[decposT4]-9 == b->data[decposT4])
		return true;
	
	if (b->data[decposT4]+1 == a->data[decposT4])
		return false;
	else if (b->data[decposT4]-9 == a->data[decposT4])
		return false;
	
	for (int i = 21; i<41; i++) {
		if (a->data[i] < b->data[i])
			return true;
		else if (a->data[i] > b->data[i])
			return false;
	}
	return true;
}
bool T4compin(Data *a, Data*b) {
	for (int i = decposT4-4; i<41; i++) {
		if (a->data[i] < b->data[i])
			return true;
		else if (a->data[i] > b->data[i])
			return false;
	}
	return true;
}
