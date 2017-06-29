// NumberedStrings.cc

#include <string.h>
#include <stdio.h>
#include "NumberedStrings.h"

NumberedStrings::NumberedStrings(int nbOfErrs)
// create a base set of number-string pairs
{
 	Init(nbOfErrs);
 	itsBaseSet = NULL;
}

NumberedStrings::NumberedStrings(NumberedStrings * baseSet, int nbOfErrs)
// create a set of number-string pairs with an existing subset of pairs
{
 	Init(nbOfErrs);
 	itsBaseSet = baseSet;
}

void NumberedStrings::Init(int nbOfErrs) {
	mxErrors = nbOfErrs;
	firstFree = 0;
	errNbs = new int[nbOfErrs];
	strings = new char*[nbOfErrs];
}

NumberedStrings::~NumberedStrings() {
// destructor
	for(int i = 0; i<firstFree; i++) {
		delete strings[i];
	}
}

void NumberedStrings::Append(int id, const char * str) {
	// append an error text
	if(firstFree < mxErrors) {
		char * newStr = new char[strlen(str)+1];
		strings[firstFree] = strcpy(newStr, str);
		errNbs[firstFree] = id;
		firstFree++;
	}
}


char * NumberedStrings::Get(int id) { 
	for(int i = 0; i<firstFree; i++) {
		if(errNbs[i] == id) return strings[i];
	}
	if(itsBaseSet != NULL) return itsBaseSet->Get(id);

	sprintf(unclErr, "Unclassified error %d", id);
	return unclErr;
}

// the 2 text parts are separeted by "//"
// return the 1st part to bufPart1 (if bufPart1 is not NULL)
// return the 2nd part to bufPart2 (if bufPart2 is not NULL)
// If there is no separating "//", return the whole text to both bufPart1 and bufPart2
void NumberedStrings::CopyTextParts(int id, char* bufPart1, char* bufPart2) {
    char* start1 = Get(id);
    CopyTextParts(start1, bufPart1, bufPart2);
}

void NumberedStrings::CopyTextParts(const char* start1, char* bufPart1, char* bufPart2) {
    const char* endp1 = 0;
    const char* start2 = start1;
    const char* endp2 = start1;
    for(;;endp2++) {
        if(*endp2 == (char)0) { // end of both parts
	    if(!endp1) { // bufPart1 not set yet. So both parts will be the same.
	       endp1 = endp2;
	       start2 = start1;
	    }
	    break;
	}
	if((*endp2 == '/') && (*(endp2+1) == '/') ) { // end of first part
	    endp1 = endp2;
	    start2 =  endp2+2;
	}
    }
    if(bufPart1) {
        for(const char* p1=start1; p1<endp1;) *bufPart1++ = *p1++;
	*bufPart1 = (char)0; 
    }
    if(bufPart2) {
        for(const char* p1=start2; p1<endp2;) *bufPart2++ = *p1++;
	*bufPart2 = (char)0;
    }
}
