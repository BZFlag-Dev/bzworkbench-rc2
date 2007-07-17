#ifndef WORLDOPTIONSDATA_H_
#define WORLDOPTIONSDATA_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include "../DataEntry.h"
#include "../model/BZWParser.h"

class world : public DataEntry {
	public:
		// default constructor
		world();
		
		// constructor with data
		world(string& data);
		
		static DataEntry* init() { return new world(); }
		static DataEntry* init(string& data) { return new world(data); }
		
		// send the data
		string get(void);
		
		// receive the data
		int update(string& data);
		
		// toString method
		string toString(void);
		
		// render method
		int render(void);
		
		// bzwb-specific api
		string& getName() { return worldName; }
		float getSize() { return size; }
		float getFlagHeight() { return flagHeight; }
		bool hasWalls() { return !noWalls; }
		
		void setName( const char* name ) { this->worldName = name; }
		void setSize( float size ) { this->size = size; }
		void setFlagHeight( float height ) { this->flagHeight = height; }
		void setWalls( bool value ) { this->noWalls = !value; }
		
	private:
		string worldName;
		float size;
		float flagHeight;
		bool noWalls;
};

#endif /*WORLDOPTIONSDATA_H_*/
