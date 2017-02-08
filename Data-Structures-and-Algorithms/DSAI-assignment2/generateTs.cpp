//Jack Zheng

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

void t1(ofstream *out, int num);
void t3(ofstream *out, int num);
void t4(ofstream *out, int num);
void add(int *left, int *right, int *fraction);
void sub(int *left, int *right, int *fraction);

int main()
{
	ofstream out1, out2, out3, out4;
	out1.open("t1.txt");
	out2.open("t2.txt");
	out3.open("t3.txt");
	out4.open("t4.txt");
	srand(time(NULL));
	cout << "Creating \"t1.txt\"" <<endl;
	t1(&out1,100000);
	cout << "Creating \"t2.txt\"" <<endl;
	t1(&out2,1000000);//t2
	cout << "Creating \"t3.txt\"" <<endl;
	t3(&out3,1000000);
	cout << "Creating \"t4.txt\"" <<endl;
	t4(&out4,1000000);
	cout << "Done" <<endl;
	return 0;
}

void t1(ofstream *out, int num)
{
	int i;
	int j;
	int pos;
	int left[20];
	int right[20];
	bool canBegin;
	*out<<num<<endl;
	
	for (i = 0; i < num; i++)
	{
		for (j = 0; j < 20; j++)
		{
			left[j] = rand() % 10;
			right[j] = rand() % 10;
		}
		
		canBegin = false;
		for (j = 0; j < 20; j++)
		{
			if (canBegin)
				*out << left[j];
			else
			{
				if (left[j] != 0 || j == 19)
				{
					canBegin = true;
					*out << left[j];
				}
			}
		}
		
		pos = 0;
		*out << ".";
		for (j = 20 - 1; j >= 0; j--)
		{
			if (right[j] != 0)
			{
				pos = j;
				break;
			}
		}
		
		for (j = 0; j <= pos; j++)
			*out << right[j];
			
		*out << endl;
	}
	return;
}

void t3(ofstream *out, int num)
{
	int i;
	int j;
	int pos;
	int left[20];
	int right[20];
	bool canBegin;
	*out<<num<<endl;
	
	for (i = 0; i < num; i++)
	{
		for (j = 0; j < 3; j++)
		{
			left[20-j-1] = rand() % 10;
			right[j] = rand() % 10;
		}
		for (j = 0; j < 17; j++)
		{
			left[j] = 0;
			right[20-j-1] = 0;
		}
		
		canBegin = false;
		for (j = 0; j < 20; j++)
		{
			if (canBegin)
				*out << left[j];
			else
			{
				if (left[j] != 0 || j == 19)
				{
					canBegin = true;
					*out << left[j];
				}
			}
		}
		
		pos = 0;
		*out << ".";
		for (j = 20 - 1; j >= 0; j--)
		{
			if (right[j] != 0)
			{
				pos = j;
				break;
			}
		}
		
		for (j = 0; j <= pos; j++)
			*out << right[j];
		
		*out << endl;
	}
	return;
}

void t4(ofstream *out, int num)
{
	int i;
	int j;
	int pos;
	int left[20];
	int right[20];
	int fraction[20];
	bool canBegin;
	int chance;
	*out<<num<<endl;
	
	for (i = 0; i < num; i++)
	{
		if (i == 0)
		{
			for (j = 0; j < 20; j++)
			{
				left[j] = rand() % 10;
				right[j] = rand() % 10;
			}
		}
		else
		{	
			for (j = 0; j < 20; j++)
			{
				fraction[j] = rand() % 10;
			}
			chance = rand() % 10 + 1;
			if (chance != 10)
				add(left, right, fraction);
			else
				sub(left, right, fraction);
		}
		
		canBegin = false;
		for (j = 0; j < 20; j++)
		{
			if (canBegin)
				*out << left[j];
			else
			{
				if (left[j] != 0 || j == 19)
				{
					canBegin = true;
					*out << left[j];
				}
			}
		}
		
		pos = 0;
		*out << ".";
		for (j = 20 - 1; j >= 0; j--)
		{
			if (right[j] != 0)
			{
				pos = j;
				break;
			}
		}
		
		for (j = 0; j <= pos; j++)
			*out << right[j];
			
		*out << endl;
	}
	return;
}

void add(int *left, int *right, int *fraction)
{
	int carrier = 0;
	int i, tmp;
	for (i = 20 - 1; i >= 0; i--)
	{
		tmp = (right[i] + fraction[i] + carrier) / 10;
		right[i] = (right[i] + fraction[i] + carrier) % 10;
		carrier = tmp;
	}
	for (i = 20 - 1; i >= 0; i--)
	{
		if (carrier == 0)
			break;
		tmp = (left[i] + carrier) / 10;
		left[i] = (left[i] + carrier) % 10;
		carrier = tmp;
	}
	if (carrier == 1)
		cout << "Error: OVERFLOW" << endl;
	return;
}

void sub(int *left, int *right, int *fraction)
{
	int i,j;
	int *tmpl = left;
	int *tmpr = right;
	int num[40];
	for (i = 0; i < 20; i++)
	{
		num[i] = *tmpl++;
		num[i + 20] = *tmpr++;
	}
	for (i = 40 - 1; i >= 20 ; i--)
	{
		if (fraction[i - 20] > num[i])
		{
			j = i - 1;
			while (num[j] == 0)
			{
				num[j] = 9;
				j--;
				if (j < 0)
				{
					cout << "Error: UNDERFLOW" << endl;
					return;
				}
			}
			num[j]--;
			num[i] += 10; 
		}
		num[i] = num[i] - fraction[i - 20];
		
	}
	for (i = 0; i < 20; i++)
	{
		*left++ = num[i];
		*right++ = num[i + 20];
	}
	return;
}