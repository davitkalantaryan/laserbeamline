// NumberedStrings.h
// M. Winde, DESY-Zeuthen, 28.06.06

#ifndef __INCNumberedStringsh
#define __INCNumberedStringsh

class NumberedStrings
{
// Handles a set of strings, each allocated to an integer
// (for example pairs of error numbers and error texts).

public:
	
	// --- creators
	NumberedStrings(int nbOfErrs = 20);	// create a base set of number-string pairs
	// create a set of number-string pairs with an existing subset of pairs
	NumberedStrings(NumberedStrings * baseSet, int nbOfErrs = 20);	


	// --- destructor
	~NumberedStrings();	
				
	// --- functions
	void Append(int id, const char * str);		// append a text

        void ChangeBaseSet(NumberedStrings* aPtr) {itsBaseSet = aPtr;}
        NumberedStrings* GetBaseSet() {return itsBaseSet;}
	
//	char * Get(int id) {return "NumberedStrings not working - sorry";}
	char * Get(int id);
	
	void CopyTextParts(int id, char* bufPart1, char* bufPart2);
	void CopyTextParts(const char* start1, char* bufPart1, char* bufPart2);	
private:

	void Init(int nbOfErrs);

	NumberedStrings * itsBaseSet;

	int mxErrors;	// max. number of errors that can be managed
	int firstFree;	// first free index in the list
	
	int * errNbs;	// array of error numbers
	char** strings;	// array of error strings
	
	char unclErr[40];
};

#endif
