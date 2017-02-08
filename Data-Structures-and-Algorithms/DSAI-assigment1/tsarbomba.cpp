#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
//Zhengqi Xi, Assignment 1
//Creates a SimpleList abstract base class with derived Stack and Queue classes.
//Parses through a command file and creates and operates on stacks and queues of various data types
using namespace std;
template <class T>
class SimpleList {		//SimpleList base class
	protected:	
	string name;
	int size;
	class Node{			//Nested node class; only SimpleList and derivative classes can access it
		public:
		T data;
		Node* next;
		Node(T stuff);
	};
	Node* first;	//Points to the first node - there are no header nodes
	Node* last;
	public:	
	T pop(){	//Both Stack and Queue will pop values from the front of the list, so pop will be implemented in the base class to avoid duplicate code
		T value = this->first->data;
		typename SimpleList<T>::Node* temp = this->first;
		this->first = temp->next;
		delete(temp);
		this->size--;
		return value;
	};
	virtual void push(T stuff)=0;	//Virtual push function to be overwritten
	string getname(){
		return name;
	};
	int getsize(){
		return size;
	};
	SimpleList(string input);
};

template <class T>
SimpleList<T>::SimpleList(string input){	//Base class constructor
	name = input;
	size = 0;
	first = last = 0;
}

template <class T>
SimpleList<T>::Node::Node(T stuff){			//Node constructor
	data = stuff;
	next = 0;
}
/************************************************/
template <class T>
class Stack: public SimpleList<T> {
	public:
	void push(T stuff){			//Stack pushes to the front of the list. This allows for constant time push and pop
		typename SimpleList<T>::Node* temp = new typename SimpleList<T>::Node(stuff);
		if (this->size==0){		//Empty stack is an edge case
			this->first = this->last = temp;
			temp->next = 0;
			this->size++;
			return;
		}
		temp->next = this->first;
		this->first = temp;
		this->size++;
	};
	Stack(string input):SimpleList<T>(input){};		//Call the base class constructor, since Stack doesn't do anything different during instantiation
};
/**********************************************/
template<class T>
class Queue: public SimpleList<T> {
	public:
	virtual void push(T stuff){		//Queue pushes to the end of the list to allow for constant time operations
		typename SimpleList<T>::Node* temp = new typename SimpleList<T>::Node(stuff);
		if (this->size==0){		//Edge case again
			this->first = this->last = temp;
			temp->next = 0;
			this->size++;
			return;
		}
		this->last->next = temp;
		this->last = temp;
		this->size++;
	};
	Queue(string input):SimpleList<T>(input){};		//Call the base class constructor for Queue as well
};
/*********************************************/
template <class T>		//Searches through given vector to find matching stack or queue with the given name. Returns the pointer to the stack/queue, else returns null
SimpleList<T>* linearFind(vector< SimpleList<T> * > * toSearch, string name){
	vector< SimpleList<T> * > temp = *toSearch;
	int size = temp.size();
	size--;
	SimpleList<T> * foundValue = 0;		//Default return value is null
	while (size >= 0){
		if (!name.compare(temp[size]->getname())){
			foundValue = temp[size];
			break;
		}
		size--;
	}
	return foundValue;
}

template <class T>		//Creates a stack/queue with the given name and type in the appropriate given vector. Writes Error to given ofstream if operation fails
void ListCreate(vector< SimpleList<T> * > * toCreate, string type, string name, ofstream * outputStream){
	if (linearFind<T>(toCreate, name)) {
		(*outputStream) << "ERROR: This name already exists!" << endl;
		} else {
			SimpleList<T> * temp;
			if (!type.compare("stack")){
				temp = new Stack<T>(name);
				toCreate->push_back(temp);
			} else {
				temp = new Queue<T>(name);
				toCreate->push_back(temp);
		}
	}
}

template <class T>		//Pushes data onto a stack/queue in a given vector with a given name. Writes result to the given ofstream object
void ListPush(vector<SimpleList<T> * > * toPush, T data, string name, ofstream * outputStream){
	SimpleList<T>* temp = linearFind<T>(toPush, name);
	if (temp == 0){
		(*outputStream) << "ERROR: This name does not exist!" << endl;
		return;
	}
	temp->push(data);
}

template <class T>		//Pops data from a stack/queue in a given vector with a given name. Writes results to the given ofstream object: value of the data if successful, error if operation fails
void ListPop(vector<SimpleList<T> * > * toPop, string name, ofstream * outputStream){
	SimpleList<T>* temp = linearFind<T>(toPop, name);
	if (temp == 0){
		(*outputStream) << "ERROR: This name does not exist!" << endl;
		return;
	}
	if (!temp->getsize()){
		(*outputStream) << "ERROR: This list is empty!" << endl;
		return;
	}
	T value = temp->pop();
	(*outputStream) << "Value popped: " << value << endl;
}
/******************************************/
int main(){
	vector<SimpleList<int> * > intList;
	vector<SimpleList<double> * > doubleList;
	vector<SimpleList<string> * > stringList;
	string inputFile = "";
	string outputFile = "";
	cout<< "Enter name of input file: " << endl;
	getline(cin,inputFile);
	cout<< "Enter name of output file: " <<endl;
	getline(cin, outputFile);
	string commands;	//Buffer where each line of the input file will be stored
	ifstream inputStream;
	inputStream.open(inputFile.c_str());
	ofstream outputStream;
	outputStream.open(outputFile.c_str());
	if(!inputStream.is_open()){		//Check to make sure that the file exists. Not necessary for the assignment but it makes me feel better
		cout << "File not found" << endl;
		return 0;
	}
	while(getline(inputStream,commands)){
		outputStream << "PROCESSING COMMAND: " << commands << endl;
		stringstream parse;		//stringstream object to parse the commands
		parse << commands;
		string firstWord;		//First word of the command: "create", "push", or "pop"
		parse >> firstWord;
		string name;			//Name of stack/queue
		parse >> name;
		string dataType = name.substr(0,1);		//Get the data type, which is the first letter of the name
		if (!firstWord.compare("create")){
			string type;		//Type of data structure to use: stack or queue
			parse >> type;
			if (!dataType.compare("i"))		//Determines appropriate ListCreate to call based on data type
				ListCreate<int>(&intList, type, name, &outputStream);
			else if (!dataType.compare("d"))
				ListCreate<double>(&doubleList, type, name, &outputStream);
			else
				ListCreate<string>(&stringList, type, name, &outputStream);
		} else if (!firstWord.compare("push")){
			string data;	//Data to be pushed
			parse >> data;
			if (!dataType.compare("i")){	//Determine appropriate ListPush to call based on data type
				int i = atoi(data.c_str());		//Convert to int
				ListPush<int>(&intList, i, name, &outputStream);
			}
			else if (!dataType.compare("d")) {
				double d = strtod(data.c_str(),0);		//Convert to double
				ListPush<double>(&doubleList, d, name, &outputStream);
			}
			else 
				ListPush<string>(&stringList, data, name, &outputStream);		//No need to convert string to string
		} else{
			if (!dataType.compare("i"))		//Determine appropriate ListPop to call based on data type
				ListPop<int>(&intList, name, &outputStream);
			else if (!dataType.compare("d"))
				ListPop<double>(&doubleList, name, &outputStream);
			else 
				ListPop<string>(&stringList, name, &outputStream);
		}
	}
	return 0;
}
