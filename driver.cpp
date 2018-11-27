#include <iostream>
#include <string>
#include <unordered_map>
#include "storytokenizer.h"
using namespace std;

int main() {
	string line, s;

	getline(cin, line);
	while (cin && line != "</html>") {
		s += line + '\n';
		getline(cin, line);
	}

	Interp I(s);
	I.iterate("start");
	system("pause");

	return 0;
}