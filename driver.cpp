#include <iostream>
#include <string>
#include <unordered_map>
#include "storytokenizer.h"
using namespace std;

int main() {
	string line, s;
	string next;

	getline(cin, line);
	while (cin && line != "</html>") {
		s += line + '\n';
		getline(cin, line);
	}

	Interp I(s);
	next = I.iterate("start");

	while (next != END_STORY)
		next = I.iterate(next);

	system("pause");

	return 0;
}