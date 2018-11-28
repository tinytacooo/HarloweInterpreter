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
		cout << "BLOCK" << tempSection << "::" << tempText << "BLOCK" << endl;	// DELETE
	}
}

/*
Interp::Interp(const string& text, bool isBlock)
{
	s.setStory(text);
} */