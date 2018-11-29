#include "storytokenizer.h"

Block::Block(const string& t)
{
	text = t.substr(1, t.length() - 2);
	PassageTokenizer bt(text);		// "bt" for "block tokenizer"

	while (bt.hasNextSection()) {
		SectionToken stok = bt.nextSection();
		section_t tempSection = stok.getType();
		string tempText = stok.getText();
		blockSections.emplace_back(tempSection, tempText);
	}
}

void Block::iterate() {

	//for (int i = 0; i < blockSections.size(); i++)
		//cout << "BLOCK" << blockSections[i].first << "::" << blockSections[i].second << endl;	//delete
	
	bool tryNext;

	for (int i = 0; i < blockSections.size(); i++)
	{
		switch (blockSections[i].first)
		{
		case LINK:
		{
			Link L(blockSections[i].second);
			displayText += L.getDisplayText();									// put displayText from LINK in displayText
			blockLinks.emplace_back(L.getDisplayText(), L.getPassageName());	// add to list of links for block
		}
		break;

		case GOTO:
		{
			GoTo go(blockSections[i].second);
			displayText += go.getName();
			blockLinks.emplace_back(go.getName(), go.getName());
		}
		break;

		case SET:
		{
			Set var(blockSections[i].second);
			bool isPresent = false;

			// check for whether variable is already present in variableVec
			for (auto it : storyVars) {
				if (it.first == var.getVar())
					isPresent = true;
			}
			// create/edit variable in temporary master list as needed - to be used as reference inside Block class
			if (isPresent)
				storyVars[var.getVar()] = var.getVal();
			else
				storyVars.emplace(var.getVar(), var.getVal());
			// create variable in block variable list - to return to outside the Block class
			blockVars.emplace(var.getVar(), var.getVal());
		}
		break;
		case IF:
		{
			bool tempBool;
			tryNext = false;
			If compare(blockSections[i].second);

			tempBool = (storyVars[compare.getVar()] == compare.getExpectedValue()) ? true : false;

			if (tempBool)
			{
				Block b(blockSections[i + 1].second);
				b.setStoryVars(storyVars);
				b.iterate();
				displayText += b.getBlockText();
				i++;
			}
			else
			{
				tryNext = true;
				i++;
			}

		}
		break;
		case ELSEIF:
		{
			if (!tryNext)
				break;
			else
			{
				bool tempBool;
				tryNext = false;
				ElseIf compare(blockSections[i].second);

				tempBool = (storyVars[compare.getVar()] == compare.getExpectedValue()) ? true : false;

				if (tempBool)
				{
					Block b(blockSections[i + 1].second);
					b.setStoryVars(storyVars);
					b.iterate();
					displayText += b.getBlockText();
					tryNext = false;
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
				Block b(blockSections[i + 1].second);
				b.setStoryVars(storyVars);
				b.iterate();
				displayText += b.getBlockText();
			}
		}
		break;
		case TEXT:
		{
			Text t(blockSections[i].second);
			displayText += t.getText();
		}
		break;
		default:
			cout << ""; //do nothing
		}
	}
}