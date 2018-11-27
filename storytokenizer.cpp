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

Interp::Interp(const string& text)
{
	s.setStory(text);
	s.parseStory();
}

void Interp::iterate(const string& passName)
{
	string passage;
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


		}
		//cout << "  Link:  " << endl << s.second << endl;
		break;
		case SET:
		{
			Set var(tokens[i].second);
			s.setVar(var.getVar(), var.getVal());
			//this->iterate("deadline");
		}
		break;
		case GOTO:
		{
			GoTo go(tokens[i].second);
			this->iterate(go.getNextP());
			i = tokens.size();
		}
		break;
		case IF:
		{
			bool tempBool;
			If compare(tokens[i].second);
			tempBool = compare.compareVariables(s.getVariableVal(compare.getVar()));
			if (tempBool)
			{
				Block b(tokens[i + 1].second);
				cout << b.getText() << endl;
				i++;
			}
			/*
			if (compare.getResult() == false && tokens[i + 2].first == ELSEIF)
			{
			ElseIf reComp(tokens[i + 2].second);
			bool tBool;
			tBool == reComp.compVar(s.getVariableVal(reComp.getVar()));
			if (tBool)
			{
			Block b1(tokens[i + 3].second);
			cout << b1.getText() << endl;
			}
			if (compare.getResult() == false && reComp.getResult() == false && tokens[i + 2].first == ELSE)
			{

			}
			//int j = 4;
			//while (tBool == false && tokens[i + j].first == ELSEIF)
			//{
			//
			//}
			}*/

		}
		break;
		case ELSEIF:
		{

		}
		break;
		case ELSE:
			//cout << "  Else:  " << endl << s.second << endl;
			break;
		case TEXT:
		{
			Text t(tokens[i].second);
			cout << t.getText() << endl;
		}
		break;
		default:
			cout << "  Unknown token:  " << tokens[i].second << endl;
		}
	}
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

	if (!t.find(LINK_SEPARATOR))
	{
		displayText = linkText;
		passageName = linkText;
	}
	else
	{
		int startName, endName, nameLen;
		int startText, endText, textLen;

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

bool If::compareVariables(const bool& value)
{
	if (expectedValue == value)
		result = true;
	return result;
}

ElseIf::ElseIf(const string& t)
{
	int startVar, endVar, varLen;
	int startBool, endBool, boolLen;
	string temp;

	startVar = t.find(ELSEIF_START) + ELSEIF_START.size();
	endVar = t.find(" ", startVar + ELSEIF_START.size() - 1);
	varLen = endVar - startVar;
	var = t.substr(startVar, varLen);

	cout << var << endl;
}

bool ElseIf::compVar(const bool& value)
{
	if (expectedValue == value)
		result = true;
	return result;
}

Block::Block(const string& t)
{
	text = t.substr(1, t.length() - 2);
}


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

SectionToken PassageTokenizer::nextSection() {
	unsigned int start, end;
	SectionToken ret;

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
		while (level > 0 && end < pass.size())
		{
			if (pass.substr(end, BLOCK_START.size()) == BLOCK_START)
				level++;
			else if (pass.substr(end, BLOCK_START.size()) == BLOCK_END)
				level--;
			end++;
		}
		if (level == 0)
			lastsect = BLOCK;
		else
			lastsect = TEXT; //Treat a bad block as text?
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
