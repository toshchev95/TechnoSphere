#include <iostream>
#include "Set.h"
using namespace std;

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

int main()
{
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
	
	return 0;
}