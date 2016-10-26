#include <iostream>
#include <string>
#include "Set.h"
using namespace std;
static int size = 1023;
void PrintSet(const Set & s) {
	cout<<'[';
	bool first = true;
	for (int i = 0; i < 256; i++)
		if (s.Has(i)) 
		{
			cout<<(first ? "" : ", ") << i;
			first = false;
		}
	cout<<']'<<endl;
}

void test() {
	Set s1, s2;
	(((s1 |= s2)  |= 5) |= 25) |= 32;
	((((s2 |= s1)  |= 1) |= 5) |= 12) |= 18;

	cout<<"s1 = ";
	PrintSet(s1);
	cout<<"s2 = ";
	PrintSet(s2);

	Set s3 = s2;
	s3 -= s1;
	cout<<"s3 = ";
	PrintSet(s3);

	try {
		s1 |= 1000;
	} 
	catch (const exception & x) {
		cout<<"Exception: "<<x.what()<<endl;
	}

}

Set processData(const string & str) {
	Set set(0, size);
	string s, command = "";
	bool isDigit;
	int member;
	for (int i = 0; i < str.length() + 1; ++i)
	{

		isDigit = isdigit(str[i]);
		if (isDigit)
		{
			s += str[i];
			continue;
		}
		else if (str[i] == ' ')
			continue;
		else if (s.length())
		{
			member = atoi(s.c_str());
			if (command == "") 
			{
				set |= member;
			}
			else 
			{
				if (command == "|")
					set |= member;
				else if (command == "&")
					set &= member;
				else if (command == "-")
					set -= member;
			}
			s = "";
			s.clear();
			if ( (str[i] == '|') ||
				 (str[i] == '&') ||
				 (str[i] == '-') )
				command = str[i];
		}
	}
	PrintSet(set);
	return set;
}

int main()
{
	//cin>>size;
	string s;
	cin>>s;
	Set &set = processData(s);
	//set = processData(s);
	PrintSet(set);
	return 0;
}