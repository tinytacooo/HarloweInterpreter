// tokenizer header
#ifndef __STORYTOKENIZER_H
#define __STORYTOKENIZER_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
using namespace std;

enum section_t { NA, LINK, GOTO, SET, IF, ELSEIF, ELSE, BLOCK, TEXT };

const string PASSAGE_START_TAG = "<tw-passagedata";
const string PASSAGE_START_TAG_CLOSE = ">";
const string PASSAGE_END_TAG = "</tw-passagedata>";
const string LINK_START = "[[";
const string LINK_SEPARATOR = "-&gt;";
const string LINK_END = "]]";
const string GOTO_START = "(go-to:";
const string SET_START = "(set: $";
const string IF_START = "(if: $";
const string ELSEIF_START = "(else-if:";
const string ELSE_START = "(else:";
const string COMMAND_END = ")";
const string BLOCK_START = "[";
const string BLOCK_END = "]";

const string PASSAGE_NAME_START = "name=\"";
const string PASSAGE_NAME_END = "\"";

//GoTo
const string GOTO_NAME_START = "&quot;";
const string GOTO_NAME_END = "&quot;";

//Starting Passage
const string STARTING_PASSAGE = "First %% Passage %%";

//End story condition
const string END_STORY = "%% YOU_SHALL_NOT_PASS %%";

class PassageToken
{
private:
	string pass;		// passage text
	string name; 		// passage name
public:
	PassageToken() {}
	PassageToken(const string& str) : pass(str) {}
	PassageToken(const string& n, const string& p) : name(n), pass(p) {}
	const string& getName() const { return name; }
	const string& getText() const { return pass; }
};

class SectionToken {
private:
	string text;		// section text
	section_t type; 	// section type
public:
	SectionToken() : type(TEXT) {}		// for a blank (bad) section
	SectionToken(string str, section_t t) : text(str), type(t) {}
	section_t getType() const { return type; }
	const string& getText() const { return text; }
};

class PassageTokenizer {
private:
	string pass;
	unsigned int pos;
	section_t lastsect;
public:
	PassageTokenizer() : pos(0) {}
	PassageTokenizer(string str) : pass(str), pos(0), lastsect(TEXT) {}
	bool hasNextSection() const;
	SectionToken nextSection();
};

class StoryTokenizer
{
private:
	string story;
	unsigned int pos;
public:
	StoryTokenizer() : pos(0) {}
	StoryTokenizer(const string& s) : story(s), pos(0) {}
	bool hasNextPassage() const;
	PassageToken nextPassage();
};

class Story
{
private:
	unordered_map<string, vector<pair<section_t, string>>> passageMap;
	unordered_map<string, bool> variableVec;
	string story, startPassage;
public:
	Story() {}
	void setStory(const string& st) { story = st; }
	void parseStory();
	string getFirstPassage() { return startPassage; }
	vector<pair<section_t, string>> getTokens(const string& p);
	void setVar(string str, bool p) { variableVec.emplace(str, p); }
	bool getVariableVal(const string& str);

};

class Interp
{
private:
	string storyText;
	Story s;
public:
	Interp(const string& text);
	string iterate(const string& passName);
};

class GoTo
{
private:
	string name;
public:
	GoTo(const string& t);
	const string& getNextP() { return name; }
};

class Text
{
private:
	string text;
public:
	Text(const string& t) { text = t; }
	const string& getText() { return text; }
};

class Set
{
private:
	string var;
	bool val;
public:
	Set(const string& t);
	const string& getVar() { return var; }
	const bool& getVal() { return val; }
};

class If
{
private:
	string var;
	bool expectedValue, result;
public:
	If(const string& t);
	const string& getVar() { return var; }
	bool compareVariables(const bool& value);
	bool getResult() { return result; }
};

class ElseIf
{
private:
	string var;
	bool expectedValue, result;
public:
	ElseIf(const string& t);
	const string& getVar() { return var; }
	bool compVar(const bool& value);
	bool getResult() { return result; }

};

class Block
{
private:
	string text;
public:
	Block(const string& t);
	const string& getText() { return text; }
};

class Link
{
private:
	string linkText, displayText, passageName;
public:
	Link(const string& t);
	const string& getLink() { return linkText; }
	const string& getPassageName() { return linkText; }
	const string& getDisplayText() { return displayText; }
};

#endif