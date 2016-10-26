#pragma once

typedef unsigned short WORD;

class Set {
private:
	int minElem;
	int maxElem;
	WORD *elems;
	int numWords;

	friend const Set & operator | (const Set & s1, const Set & s2);
	friend const Set & operator & (const Set & s1, const Set & s2);
	friend const Set & operator - (const Set & s1, const Set & s2);
	friend const Set & operator - (const Set & s);

public:
	Set(int min = 0, int max = 1023);

	Set(const Set & s);

	~Set();

	bool Has(int n) const;

	Set & operator |= (int n);
	Set & operator |= (const Set & other);

	Set & operator &= (const Set & other);

	Set & operator -= (int n);
	Set & operator -= (const Set & other);

	Set & Inverse();
};