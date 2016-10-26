#include <stdexcept>

#include "Set.h"

using namespace std;

Set::Set(int min, int max) {
	if (min > max)
	{
		minElem = max;
		maxElem = min;
	} 
	else
	{
		minElem = min;
		maxElem = max;
	}
	int num = maxElem - minElem + 1;
	numWords = (num + 15) >> 4;
	elems = new WORD[numWords];

	for (int i = 0; i < numWords; i++) {
		elems[i] = 0;
	}
}

Set::Set(const Set & s) {
	minElem = s.minElem;
	maxElem = s.maxElem;
	elems = new WORD[numWords = s.numWords];
	for (int i = 0; i < numWords; i++) {
		elems[i] = s.elems[i];
	}
}

Set::~Set() {
	delete[] elems;
}

bool Set::Has(int n) const {
	if (n > maxElem || n < minElem)
		return false;

	int word = (n - minElem) >> 4;
	int bit = (n - minElem) & 15;
	return (elems[word] & (1 << bit)) != 0;
}

Set & Set::operator |= (int n) {
	if (n <= maxElem && n >= minElem) {
		int word = (n - minElem) >> 4;
		int bit = (n - minElem) & 15;
		elems[word] |= (1 << bit);
	} 
	else
	{
		throw out_of_range("Cannot add an element: it is out of range");
	}
	return *this;
}

Set & Set::operator |= (const Set & other) {
	if (other.minElem != minElem || other.maxElem != maxElem)
	{
		throw out_of_range("Sets incomparable");
	}
	for (int i = 0; i < numWords; i++)
	{
		elems[i] |= other.elems[i];
	}
	return *this;
}

Set & Set::operator &= (const Set & other) {
	if (other.minElem != minElem || other.maxElem != maxElem)
	{
		throw out_of_range("Sets incomparable");
	}
	for (int i = 0; i < numWords; i++)
	{
		elems[i] &= other.elems[i];
	}
	return *this;
}

Set & Set::operator -= (int n) {
	if (n <= maxElem && n >= minElem) {
		int word = (n - minElem) >> 4;
		int bit = (n - minElem) & 15;
		elems[word] &= ~(1 << bit);
	}
	return *this;
}

Set & Set::operator -= (const Set & other) {
	if (other.minElem != minElem || other.maxElem != maxElem)
	{
		throw out_of_range("Sets incomparable");
	}
	for (int i = 0; i < numWords; i++)
	{
		elems[i] &= ~other.elems[i];
	}
	return *this;
}

Set & Set::Inverse() {
	for (int i = 0; i < numWords; i++)
	{
		elems[i] = ~elems[i];
	}
	return *this;
}

const Set & operator | (const Set & s1, const Set & s2) {
	return Set(s1) |= s2;
}

const Set & operator & (const Set & s1, const Set & s2) {
	return Set(s1) &= s2;
}

const Set & operator - (const Set & s1, const Set & s2) {
	return Set(s1) -= s2;
}

const Set & operator - (const Set & s) {
	return Set(s).Inverse();
}