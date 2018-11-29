// tokenizer cpp
#include "storytokenizer.h"

void Story::parseStory()
{
	StoryTokenizer st(story);
	int pass = 0;
	while (st.hasNextPassage())
	{
		PassageToken ptok = st.nextPassage();
		pass++;
		//cout << "Passage " << pass << ":  " << endl;
		PassageTokenizer pt(ptok.getText());
		if (pass == 1)
		{
			startPassage = ptok.getName();
		}
		//cout << ptok.getName() << endl;
		vector<pair<section_t, string>> temp;
		while (pt.hasNextSection())
		{
			SectionToken stok = pt.nextSection();
			section_t tempSection = stok.getType();
			string tempText = stok.getText();
			temp.emplace_back(tempSection, tempText);
			//cout << stok.getText() << endl;
		}
		passageMap.emplace(ptok.getName(), temp);
	}
}

vector<pair<section_t, string>> Story::getTokens(const string& p)
{
	vector<pair<section_t, string>> temp;
	if (passageMap.count(p) == 0)
	{
		return temp;
	}
	else
	{
		return passageMap.at(p);
	}

}

bool Story::getVariableVal(const string& str)
{
	if (variableVec.count(str) == 0)
		cout << "Error in setting passage variable." << endl;
	else
		return variableVec.at(str);
}

void Story::setVar(string str, bool p)
{
	bool isPresent = false;
	// check for whether variable is already present in variableVec
	for (auto it : variableVec) {
		if (it.first == str)
			isPresent = true;
	}
	// create/edit variable as needed
	if (isPresent) 
		variableVec[str] = p;
	else 
		variableVec.emplace(str, p);
}

Interp::Interp(const string& text)
{
	s.setStory(text);
	s.parseStory();
}

string Interp::iterate(const string& passName)
{
	bool tryNext;
	int nextPassage;
	string passage;
	string passageText; 						// passage text to display for user
	vector<pair<string, string>> linkList;		// list of links to display after the passage text <displayText, passageName>
	vector<pair<section_t, string>> tokens;
	if (passName == STARTING_PASSAGE)
		passage = s.getFirstPassage();
	else
		passage = passName;
	tokens = s.getTokens(passage);
	cout << "Passage Name: " << passage << endl;


	for (int i = 0; i < tokens.size(); i++)
	{
		switch (tokens[i].first)
		{
		case LINK:
		{
			Link L(tokens[i].second);
			passageText += L.getDisplayText();									// put displayText from LINK in passageText
			linkList.emplace_back(L.getDisplayText(), L.getPassageName());		// add linked passage to list of options
		}
		//cout << "  Link:  " << endl << s.second << endl;
		break;
		case SET:
		{
			Set var(tokens[i].second);
			s.setVar(var.getVar(), var.getVal());
		}
		break;
		case GOTO:
		{
			GoTo go(tokens[i].second);
			//this->iterate(go.getName());		// KJ - EDIT
			//i = tokens.size();					// KJ - EDIT
			passageText += go.getName();							// basically handle GOTO like a LINK... don't know if that's right, though.
			linkList.emplace_back(go.getName(), go.getName());
		}
		break;
		case IF:
		{
			bool tempBool;
			tryNext = false;
			If compare(tokens[i].second);
			tempBool = (s.getVariableVal(compare.getVar()) == compare.getExpectedValue()) ? true : false;

			if (tempBool)
			{
				/*** BLOCK HANDLING ***/
				Block b(tokens[i + 1].second);
				b.setStoryVars(s.getStoryVars());
				b.iterate();

				// update passage linkList
				for (auto it : b.getBlockLinks())
					linkList.emplace_back(it.first, it.second);
				// update story variables
				for (auto it : b.getBlockVars())
					s.setVar(it.first, it.second);
				// update passage text display
				passageText += b.getBlockText();
				/*** END BLOCK HANDLING ***/

				i++;
			}
			else
			{
				tryNext = true;		// if IF condition isn't met, allow next else/else-if block to be evaluated
				i++;				// if IF is not true, do nothing and skip over its block.
			}

		}
		break;
		case ELSEIF:
		{
			if (!tryNext)		// if tryNext is FALSE (meaning a previous statement was executed), skip over else-if and the following block
				break;
			else
			{
				bool tempBool;
				ElseIf compare(tokens[i].second);
				tempBool = (s.getVariableVal(compare.getVar()) == compare.getExpectedValue()) ? true : false;

				if (tempBool)
				{
					/*** BLOCK HANDLING ***/
					Block b(tokens[i + 1].second);
					b.setStoryVars(s.getStoryVars());
					b.iterate();

					// update passage linkList
					for (auto it : b.getBlockLinks())
						linkList.emplace_back(it.first, it.second);
					// update story variables
					for (auto it : b.getBlockVars())
						s.setVar(it.first, it.second);
					// update passage text display
					passageText += b.getBlockText();
					/*** END BLOCK HANDLING ***/

					tryNext = false;	// don't evaluate next block (if any) in the sequence
					i++;
				}
				else
					i++;
			}
		}
		break;
		case ELSE:
		{
			if (!tryNext)
				break;
			else
			{
				Block b(tokens[i + 1].second);
				b.setStoryVars(s.getStoryVars());
				b.iterate();
				passageText += b.getBlockText();
				//cout << b.getText() << endl;

			}
		}
		//cout << "  Else:  " << endl << s.second << endl;
		break;
		case TEXT:
		{
			Text t(tokens[i].second);
			passageText += t.getText();
		}
		break;
		default:
			cout << "";
			// cout << "  Unknown token:  " << tokens[i].second << endl;
		}
	}

	// display passage and list of links for user to choose from
	cout << passageText << endl;
	for (int i = 0; i < linkList.size(); i++)
		cout << i + 1 << ". " << linkList[i].first << endl;

	if (linkList.size() >= 1)
	{
		cin >> nextPassage;
		return linkList[--nextPassage].second;
	}
	else
		return END_STORY;
}

GoTo::GoTo(const string& t)
{
	int start, end, len;

	start = t.find(GOTO_NAME_START);
	end = t.find(GOTO_NAME_END, start + GOTO_NAME_START.size());
	len = end - start - GOTO_NAME_START.size();

	name = t.substr(start + GOTO_NAME_START.size(), len);

}

Set::Set(const string& t)
{
	int startVar, endVar, varLen;
	int startBool, endBool, boolLen;
	string temp;

	startVar = t.find(SET_START) + SET_START.size();
	endVar = t.find(" ", startVar);
	varLen = endVar - startVar;
	var = t.substr(startVar, varLen);

	startBool = t.find("to ", endVar) + 3;
	endBool = t.find(")", startBool);
	boolLen = endBool - startBool;
	temp = t.substr(startBool, boolLen);

	if (temp == "true")
		val = true;
	else
		val = false;
}

Link::Link(const string& t)
{
	int startLink, endLink, linkLen;
	string temp;

	startLink = t.find(LINK_START) + LINK_START.size();
	endLink = t.find(LINK_END, startLink);
	linkLen = endLink - startLink;
	linkText = t.substr(startLink, linkLen);

	if (t.find(LINK_SEPARATOR) == string::npos)
	{
		displayText = linkText;
		passageName = linkText;
	}
	else
	{
		int startText, endText, textLen;
		int startName, endName, nameLen;
		// get displayText
		startText = t.find(LINK_START) + LINK_START.size();
		endText = t.find(LINK_SEPARATOR, startText);
		textLen = endText - startText;
		displayText = t.substr(startText, textLen);
		// get passageName
		startName = endText + LINK_SEPARATOR.size();
		endName = t.find(LINK_END, startName);
		nameLen = endName - startName;
		passageName = t.substr(startName, nameLen);
	}

}

If::If(const string& t)
{
	int startVar, endVar, varLen;
	int startBool, endBool, boolLen;
	string temp;

	startVar = t.find(IF_START) + IF_START.size();
	endVar = t.find(" ", startVar + IF_START.size() - 1);
	varLen = endVar - startVar;
	var = t.substr(startVar, varLen);

	startBool = t.find("is ", endVar) + 3;
	endBool = t.find(")", startBool);
	boolLen = endBool - startBool;
	temp = t.substr(startBool, boolLen);

	if (temp == "true")
		expectedValue = true;
	else
		expectedValue = false;

}

ElseIf::ElseIf(const string& t)
{
	int startVar, endVar, varLen;
	int startBool, endBool, boolLen;
	string temp;

	startVar = t.find(ELSEIF_START) + ELSEIF_START.size() + 2;
	endVar = t.find(" ", startVar);
	varLen = endVar - startVar;
	var = t.substr(startVar, varLen);

	startBool = t.find("is ", endVar) + 3;
	endBool = t.find(")", startBool);
	boolLen = endBool - startBool;
	temp = t.substr(startBool, boolLen);

	if (temp == "true")
		expectedValue = true;
	else
		expectedValue = false;
}

// DELETEd old block code; moved complex shit to block.cpp

bool StoryTokenizer::hasNextPassage() const
{
	return story.find(PASSAGE_START_TAG, pos) != string::npos;
}

PassageToken StoryTokenizer::nextPassage()
{
	unsigned int next = story.find(PASSAGE_START_TAG, pos);
	unsigned int start = story.find(PASSAGE_START_TAG_CLOSE, next) + 1;
	unsigned int end = story.find(PASSAGE_END_TAG, start);
	if (next == string::npos || end == string::npos)
		return PassageToken();
	else
	{
		pos = end;
		unsigned int namestart = story.find(PASSAGE_NAME_START, next) + PASSAGE_NAME_START.size();
		unsigned int nameend = story.find(PASSAGE_NAME_END, namestart);
		if (namestart == string::npos || nameend == string::npos)
			return PassageToken(story.substr(start, end - start));
		else
			return PassageToken(story.substr(namestart, nameend - namestart), story.substr(start, end - start));
	}
}

bool PassageTokenizer::hasNextSection() const
{
	return pos < pass.size();
}

SectionToken PassageTokenizer::nextSection()
{
	unsigned int start, end;
	SectionToken ret;

	// when end of passage has been reached
	// pos is where we left off; updated at the end of this function
	if (pos >= pass.size())
	{
		lastsect = TEXT;
		return ret;
	}

	//Handle blocks separately
	start = pass.find(BLOCK_START, pos);
	if ((lastsect == IF || lastsect == ELSEIF || lastsect == ELSE) && start != string::npos)
	{
		end = start + 1;
		unsigned int level = 1;

		// iterate through the block and find all the text in it, even links denoted with square brackets
		while (level > 0 && end < pass.size())
		{
			if (pass.substr(end, BLOCK_START.size()) == BLOCK_START)
				level++;
			else if (pass.substr(end, BLOCK_START.size()) == BLOCK_END)
				level--;
			end++;
		}

		// if a closing bracket was found for the opening bracket, the text was a block.
		if (level == 0)
			lastsect = BLOCK;
		else
			lastsect = TEXT; //Treat a bad block as text?
							 // cout << pass.substr(start, end - start) << endl;
		ret = SectionToken(pass.substr(start, end - start), BLOCK);
		pos = end;
		return ret;
	}
	else
	{
		unsigned int nextset = pass.find(SET_START, pos);
		unsigned int nextgoto = pass.find(GOTO_START, pos);
		unsigned int nextlink = pass.find(LINK_START, pos);
		unsigned int nextif = pass.find(IF_START, pos);
		unsigned int nextelseif = pass.find(ELSEIF_START, pos);
		unsigned int nextelse = pass.find(ELSE_START, pos);

		unsigned int nexttok = nextset;
		if (nextgoto < nexttok)
			nexttok = nextgoto;
		if (nextgoto < nexttok)
			nexttok = nextgoto;
		if (nextlink < nexttok)
			nexttok = nextlink;
		if (nextif < nexttok)
			nexttok = nextif;
		if (nextelseif < nexttok)
			nexttok = nextelseif;
		if (nextelse < nexttok)
			nexttok = nextelse;

		end = string::npos;
		lastsect = TEXT;
		if (nexttok > pos)
			end = nexttok;
		else if (nexttok == nextlink)
		{
			end = pass.find(LINK_END, nextlink) + LINK_END.size();
			lastsect = LINK;
		}
		else if (nexttok != string::npos)
		{
			end = pass.find(COMMAND_END, pos) + 1;
			if (nexttok == nextgoto)
				lastsect = GOTO;
			else if (nexttok == nextset)
				lastsect = SET;
			else if (nexttok == nextif)
				lastsect = IF;
			else if (nexttok == nextelseif)
				lastsect = ELSEIF;
			else if (nexttok == nextelse)
				lastsect = ELSE;
			else
				lastsect = TEXT;
		}

		if (end != string::npos)
			ret = SectionToken(pass.substr(pos, end - pos), lastsect);
		else
			ret = SectionToken(pass.substr(pos), lastsect);
		pos = end;
		return ret;
	}
}