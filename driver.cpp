#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include "storytokenizer.h"
using namespace std;

int main() {
	string line, s;
	string next;
	ifstream file;

	file.open("test-example.html");
	if (file.is_open())
	{
	getline(file, line);
	//cout << line << endl;
	while (cin && line != "</html>")
	{
	s += line + '\n';
	getline(file, line);
	//cout << line << endl;
	}
	}
	Interp I(s);
	next = I.iterate(I.getFirstPass());

	while (next != END_STORY)
	next = I.iterate(next);

	file.close();

	return 0;
}