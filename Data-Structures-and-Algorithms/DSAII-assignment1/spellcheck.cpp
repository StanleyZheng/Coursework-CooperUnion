/*
    Stanley Zheng
    Data Structures and Algorithms II, Fall 2016
    Programming Assignment #1
    Due: September 27, 2016

    -   A Spellchecker program that requests the input of a dictionary, input file, 
        and output file. The dictionary will populate a hashtable which will be used
        to check the words that are read from the input file. Errors will be printed
        to the output file.
    -   Errors include words that are too long (> 20 characters) and words that do not
        appear in the provided dictionary
    -   Characters considered valid are letters, digits, dashes, and apostrophes.
    -   Every other character is considered a word separator
*/


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cctype>
#include "hash.h"
using namespace std;

void load_dictionary(hashTable *dict, ifstream &dictfile){
    int i;
    string line;
    clock_t t1, t2;
    double timeDiff;

    t1 = clock();
    while(getline(dictfile,line)){
        i = 0;
        while (i < line.size()){
            line[i] = tolower(line[i]);
            ++i;
        }
        dict->insert(line);
    }
    t2 = clock();
    timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC;
    cout << "Total time (in seconds) to load dictionary: " << timeDiff << endl;

    dictfile.close();   
}

void spell_check(hashTable *dict, ifstream &input, ofstream &output){
    int i;
    string word, temp;
    string line = "";
    int linenumber = 0;
    int containsnum = 0; // if 1, word contains a number
    clock_t t1, t2;
    double timeDiff;

    t1 = clock();
    while (getline(input,line)){
        i = 0;
        ++linenumber;
        while (i <= line.size()){
            temp = tolower(line[i++]);
            if (isdigit(temp[0]))
                containsnum = 1;
            if (isalnum(temp[0]) || temp[0] == '-' || temp[0] == '\'')
                word = word + temp;
            else if (word.size() != 0 && containsnum == 0){
                if (word.size() > 20)
                    output << "Long word at line " << linenumber << ", starts: "<< word.substr(0,20) << endl;
                else if (!dict->contains(word))
                    output << "Unknown word at line " << linenumber << ": " << word << endl;
                word = "";
            }
            else{
                word = "";
                containsnum = 0;
            }
        }
    }
    t2 = clock();
    timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC;
    cout << "Total time (in seconds) to check document: " << timeDiff << endl;
}

int main(){
    hashTable dict = hashTable(24989);

    //load dictionary
    string dictname;
    cout << "Enter name of dictionary: ";
    cin >> dictname;
    ifstream dictfile(dictname.c_str());
    if (!dictfile) {
        cerr << "Error: could not open " << dictname << endl;
        exit(1);
    }
    
    load_dictionary(&dict, dictfile);


    //open input and output files
    string infile, outfile;
    cout << "Enter name of input file: ";
    cin >> infile;
    ifstream input(infile.c_str());
    if(!input){
        cerr << "Error: could not open " << infile << endl;
        exit(1);
    }
    cout << "Enter name of output file: ";
    cin >> outfile;
    ofstream output(outfile.c_str());
    if(!output){
        cerr << "Error: could not open " << outfile << endl;
        exit(1);
    }

    //spellcheck input file and print errors to output file
    spell_check(&dict, input, output);

    //close files;
    input.close();
    output.close();

    return 0;
}