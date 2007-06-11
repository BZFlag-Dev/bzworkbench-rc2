#ifndef FTOA_H_
#define FTOA_H_

#include <string>

using namespace std;

inline const char* ftoa(float f) {
	char buff[30];
	sprintf(buff, "%f", f);
	return (string(buff).c_str());
}

inline const char* itoa(int i) {
	char buff[30];
	sprintf(buff, "%d", i);
	return (string(buff).c_str());	
}

#endif /*FTOA_H_*/
