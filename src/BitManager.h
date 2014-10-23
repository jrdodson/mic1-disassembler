//John Ryan Dodson
//BitManager.h

#ifndef BITMANAGER_H
#define BITMANAGER_H
#include <string>

//instruction structure
typedef struct {
	unsigned int alu, bbus, cbus,jam,memory,next, address;
	std::string label;
} INSTRUCTION;

//number of items to read in to buf[]
const int MAX = 5;

//manager class for performing file consumption & bitwise operations
class BitManager {

public: 
	BitManager();
	void execute(); 
	bool verify();
	void parseInstruction(INSTRUCTION inst, int i); //does all the hard work
	std::string convert(unsigned num, int l); //converting to binary
	std::string space(std::string n, int l) ;//formatting appropriately 
	std::string print(INSTRUCTION inst); 
	std::string getLabel(INSTRUCTION inst, int a); //label generation 
	std::string heading(); 

private:
	unsigned char buf[MAX];
	bool isLegal;
	int counter;
	int nwln; //for keeping track of spacing

};
#endif
