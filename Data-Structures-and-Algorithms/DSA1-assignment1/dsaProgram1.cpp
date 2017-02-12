/*
    Stanley Zheng
    Data Structures and Algorithms I, Spring 2016
    Programming Assignment #1

    -Creates an abstract base class (SimpleList) with derived classes (Stack, Queue)
    -Parses a file and follows commands (operates on the SimpleLists)
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <list>
#include <algorithm>
using namespace std;



/***** SimpleList base class *****/
template <typename T> 
class SimpleList {
protected:
    string name;
    int size;

    class Node{
    public:
        T data;
        Node *next;
        Node(T input);
    };
    Node *head;
    Node *tail;

    void insert_start(T data);
    void insert_end(T data);
    T remove_start();

public:
    SimpleList(string input);
    string get_name();
    int get_size();
    virtual T pop() = 0;
    virtual void push(T data) = 0;
};



/********** Functions for SimpleList base class **********/
//Constructors for Node and SimpleList
template <typename T>
SimpleList<T>::Node::Node(T value){
    data = value;
    next = 0;
};
template <typename T>
SimpleList<T>::SimpleList(string input){
    name = input;
    size = 0;
    head = 0;
    tail = 0;
};

//insert node at start
template <typename T>
void SimpleList<T>::insert_start(T value){
    typename SimpleList<T>::Node *temp = new typename SimpleList<T>::Node(value);
    if (this->size==0){
        this->head = temp;
        this->tail = temp;
        temp->next = 0;
    }
    else {
        temp->next = this->head;
        this->head = temp;
    }
    this->size++;
};

//insert node at end
template <typename T>
void SimpleList<T>::insert_end(T value){
    typename SimpleList<T>::Node *temp = new typename SimpleList<T>::Node(value);
    if (this->size==0){
        this->head = temp;
        this->tail = temp;
    }
    else {
        this->tail->next = temp;
        this->tail = temp;
    }
    this->size++;
};

//remove node from start
template <typename T> 
T SimpleList<T>::remove_start(){
    T retval = this->head->data;
    SimpleList<T>::Node *temp = this->head;
    this->head = temp->next;
    delete(temp);
    this->size--;
    return retval;
};

//getter functions for name and size
template <typename T> 
string SimpleList<T>::get_name(){
    return name;
};
template <typename T> 
int SimpleList<T>::get_size(){
    return size;
};



/********** Stack and Queue implementation **********/
//Stack (derived class of SimpleList)
template <typename T>
class Stack: public SimpleList<T>{
public:
    //LIFO - push and pop from the beginning of the list
    void push(T input);
    T pop();
    Stack(string input) : SimpleList<T>(input){};     //inherit base class constructor
};

//Stack functions
template <typename T>
void Stack<T>::push(T input){
    this->insert_start(input);
}
template <typename T>
T Stack<T>::pop(){
    return this->remove_start();
}


//Queue (derived class of SimpleList)
template <typename T>
class Queue: public SimpleList<T>{
public:
    //FIFO - push to end and pop from beginning of the list
    void push(T input);
    T pop();
    Queue(string input) : SimpleList<T>(input){};     //inherit base class constructor
};

//Queue functions
template <typename T>
void Queue<T>::push(T input){
    this->insert_end(input);
}
template <typename T>
T Queue<T>::pop(){
    return this->remove_start();
}



/********** Processes commands **********/
template <typename T> 
void process_command(list<SimpleList<T> *> *listSL, string comm, string listname, string listType, T data, ofstream *outputstream){
    SimpleList<T> *temp = 0;

    //iterator to check if listname is in the list, if not there nothing changes, if there set temp to found list.
    typename list<SimpleList<T> *>::iterator findIter;
    for (findIter = listSL->begin(); findIter != listSL->end(); ++findIter){
        if ( !(*findIter)->get_name().compare(listname) ){
            temp = *findIter;
            break;
        }
    }
    //not found ->  temp = 0
    //found ->      temp = (wanted list)

    //process 'create'
    if ( !(comm.compare("create")) ){
        if (temp == 0){
            SimpleList<T> *pSLi;

            //check which to make (stack or queue)
            if ( !listType.compare("stack"))
                pSLi = new Stack<T>(listname);
            else if ( !listType.compare("queue") )
                pSLi = new Queue<T>(listname);

            listSL->push_back(pSLi);
        }
        else
            (*outputstream) << "ERROR: This name already exists!" << endl;
    }

    //process 'push'
    else if ( !(comm.compare("push")) ){
        if (temp == 0)
            (*outputstream) << "ERROR: This name does not exist!" << endl;
        else
            temp->push(data);
    }

    //process 'pop'
    else {
        if (temp == 0)
            (*outputstream) << "ERROR: This name does not exist!" << endl;
        else if ( !(temp->get_size()) )
                (*outputstream) << "ERROR: This list is empty!" << endl;
        else {
            T data = temp->pop();
            (*outputstream) << "Value popped: " << data << endl;
        }
    }

};//closes process_command(~~)


/********** Main function **********/
int main(){
    list<SimpleList<int> *> integerList;    //list of all integer stacks and queues
    list<SimpleList<double> *> doubleList;  //list of all double stacks and queues
    list<SimpleList<string> *> stringList;  //list of all string stacks and queues

    string input;
    string output;
    cout << "Enter name of input file: ";
    cin >> input;
    cout << "Enter name of output file: ";
    cin >> output;

    //open files and check if successful and then process commands
    ofstream outputfile (output.c_str());
    ifstream inputfile (input.c_str());
    string line;
    if ( inputfile.is_open() ){
        while ( getline(inputfile, line) ){
            outputfile << "PROCESSING COMMAND: " << line << endl;

            //used stringstream to parse the line from file
            stringstream strline;
            string firstArg;
            string secondArg;
            string thirdArg; 

            strline << line;            //parse line into strline
            strline >> firstArg;        //sets (nth)Arg equal to (nth) word of strline
            strline >> secondArg;       
            strline >> thirdArg;

            //gets first letter of listname and sends the appropriate list of SimpleList to parse_command
            string dataType = secondArg.substr(0,1);
            if ( !(dataType.compare("i")) )
                process_command(&integerList, firstArg, secondArg, thirdArg, atoi(thirdArg.c_str()), &outputfile);
            else if ( !(dataType.compare("d")) )
                process_command(&doubleList, firstArg, secondArg, thirdArg, strtod(thirdArg.c_str(),0), &outputfile);
            else
                process_command(&stringList, firstArg, secondArg, thirdArg, thirdArg, &outputfile);
        }

        inputfile.close();
        outputfile.close();
    }
    else {
        cout << "Unable to open/find input file";
    }
    return 0;
};