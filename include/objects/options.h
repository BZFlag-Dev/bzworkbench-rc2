#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "../DataEntry.h"
#include <string>

using namespace std;

class options : public DataEntry {
public:
	
	options() : DataEntry("options", "") {
		this->optionsString = string("");
	}
	
	options(string& data) : DataEntry("options", "", data.c_str()) { this->update(data); }
	
	// get method
	string get(void) {
		return this->toString();
	}
	
	// update method
	int update(string& data) {
		
		const char* header = this->getHeader().c_str();
		// get options objects
		vector<string> optionses = BZWParser::getSectionsByHeader(header, data.c_str());
		
		if(optionses[0] == BZW_NOT_FOUND)
			return 0;
		
		if(!hasOnlyOne(optionses, "options"))
			return 0;
		
		const char* opts = optionses[0].c_str();
		
		// get the lines
		vector<string> options = BZWParser::getLines(header, opts);
		
		// concat the lines
		this->optionsString.clear();
		
		for(vector<string>::iterator i = options.begin(); i != options.end(); i++) {
			this->optionsString += *i + " ";
		}
		
		return (DataEntry::update(data));
		
	}
	
	// toString method
	string toString(void) {
		return string(string("options\n") +
							"  " + optionsString + "\n" + 
							this->getUnusedText() + 
							"end\n");
	}
	
	// render method
	int render(void) { return 0; }
private:
	string optionsString;
};

#endif /*OPTIONS_H_*/
