/*
    Stanley Zheng
    Data Structures and Algorithms II, Fall 2016
    Programming Assignment #4
    Due: December 13, 2016

    -   Program deteremines whether of not a third string (strC) is a valid 
        merge of two strings (strA & strB)
        - if yes, program will output strC with the characters from A converted
          to uppercase; the letters of A should be made to occur as early as possible
        - if no, will output "*** NOT A MERGE ***"
    -   prompts user for input and output file
        - input file consists of multiple sets of three strings, each separated 
          by a new line (number of rows in the file should be a multiple of three; 
          if not, will ignore trailing string
    -   utilizes a global array (ary[1001][1001]) and a dynamic programming approach.
        - strA and strB were guaranteed to be at most 1000 letters long

*/


#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
using namespace std;

bool ary[1001][1001];

// returns whether or not strA and strB are interleaved in strC
// fills in global array ary[][]
bool check_subseq(string strA, string strB, string strC){
    int i, j;               //indexes used for looping through global array 
    int M = strA.length();
    int N = strB.length();

    //if length of strA and strB do not sum to strC --> "not a merge"
    if (M + N != strC.length())
        return false;

    //fill out first row and first column
    ary[0][0] = true;
    for(j = 1; j <= N; j++){
        if (strB[j-1] == strC[j-1])
            ary[0][j] = ary[0][j-1];
        else
            ary[0][j] = false;
    }
    for(i = 1; i <= M; i++){
        if (strA[i-1] == strC[i-1])
            ary[i][0] = ary[i-1][0];
        else
            ary[i][0] = false;
    }

    //all other lengths of substrings for strA and strB (aside from empty string)
    for(i = 1; i <= M; i++){
        for(j = 1; j <= N; j++){
            if (strA[i-1] == strC[i+j-1]){
                if (strB[j-1] == strC[i+j-1])
                    ary[i][j] = ary[i-1][j] || ary[i][j-1];
                else 
                    ary[i][j] = ary[i-1][j];
            }
            else {
                if (strB[j-1] == strC[i+j-1])
                    ary[i][j] = ary[i][j-1];
                else
                    ary[i][j] = false;
            }
        }
    }
    return ary[M][N];
}


// prints out third string with uppercased chars of first string subsequence
// has a priority for strA to take on the leftmost characters as its subsequence
void print_subseq(string strA, string strB, ofstream &output){
    string temp = "";
    int i = strA.length();
    int j = strB.length();

    while (i > 0 && j > 0){
        if (ary[i][j] == ary[i][j-1])
            temp = strB[--j] + temp;
        else if (ary[i][j] == ary[i-1][j])
            temp = (char)toupper(strA[--i]) + temp;
        else {
            cout << "ERROR!: we should not get here" << endl;
            break;
        }
    }
    while (i == 0 && j > 0)
        temp = strB[--j] + temp;
    while (j == 0 && i > 0)
        temp = (char)toupper(strA[--i]) + temp;
    
    output << temp << endl;
    return;
}


//return 0 on success, 1 if files were unable to be opened
int main(){
    string infile, outfile;
    cout << "Enter name of input file: ";
    cin >> infile;
    ifstream input(infile.c_str());
    if(!input){
        cerr << "Error: could not open " << infile << endl;
        return 1;
    }
    cout << "Enter name of output file: ";
    cin >> outfile;
    ofstream output(outfile.c_str());
    if(!output){
        cerr << "Error: could not open " << outfile << endl;
        return 1;
    }

    int i = 1;
    string line, s1, s2, s3;
    //get set of three lines and process strings when the third is obtained
    while(getline(input,line)){
        if (i == 1){
            s1 = line;
            i++;
        }
        else if (i == 2){
            s2 = line;
            i++;
        }
        else if (i == 3){
            s3 = line;
            if ( check_subseq(s1, s2, s3) )
                print_subseq(s1,s2,output);
            else
                output << "*** NOT A MERGE ***" << endl;
            i = 1;  //reset for next set of strings
        }
    }
    return 0;
}