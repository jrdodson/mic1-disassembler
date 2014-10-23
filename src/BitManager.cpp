//John Ryan Dodson
//BitManager.cpp

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include "BitManager.h"
using namespace std;

//for output to file
ofstream of("result.txt");

//opcodes structure
typedef struct {
	char* label;
	int opcode[27]; //array of addresses for opcodes
} OPCODES;

//all the codes we will need to parse through
static OPCODES opmap[] = {
	"false",		  {0x01,0x40,0x41},
	"main",           {0x02},
	"bipush",         {0x10,0x16,0x17},
	"ldc_w",          {0x13,0x27,0x28,0x29},
	"iload",          {0x15,0x18},
	"wide_iload",     {0x115,0x19,0x1a,0x1b,0x21,0x22,0x23},
	"istore",         {0x36,0x1c},
	"wide_istore",    {0x136,0x1d,0x1e,0x1f,0x20,0x24,0x25,0x26},
	"pop",            {0x57, 0x0c,0x0d},
	"dup",            {0x59, 0x0b},
	"swap",           {0x5F,0x0e,0x0f,0x11,0x12,0x14},
	"iadd",           {0x60, 0x03, 0x04},
	"isub",           {0x64, 0x05, 0x06},
	"iand",           {0x7E,0x07,0x08},
	"iinc",           {0x84,0x2a,0x2b,0x2c,0x2d,0x2e},
	"ifeq",           {0x99,0x38,0x39,0x3a},
	"iflt",           {0x9B,0x34,0x35,0x37},
	"if_icmpeq",      {0x9F,0x3b,0x3c,0x3d,0x3e,0x3f},
	"goto",           {0xA7,0x2f,0x30,0x31,0x32,0x33},
	"ireturn",        {0xAC,0x58,0x5a,0x5b,0x5c,0x5d,0x5e,0x61},
	"ior",            {0xB0, 0x09, 0x0a},
	"invokevirtual",  {0xB6,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
						0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,
						0x53,0x54,0x55,0x56},
	"wide",           {0xC4},
	"halt",           {0xFF},
	"err",            {0xFE},
	"out",            {0xFD,0x86,0x87,0x89,0x8a,0x8b,0x8c},
	"in",             {0xFC,0x8d,0x8e,0x8f,0x90},
};


BitManager::BitManager() {
	isLegal = false;
	counter = 0;
	nwln = 0;
}

//main func
void BitManager::execute() {

	
	//open and grab first four bytes
	ifstream in("mic1ijvm.mic1", ios::binary);

	if(in.fail()) {
		cout << "Failed to open mic1ijvm.mic1" << endl;
		return;
	}

	in.read((char*)buf,MAX-1);
	
	//verify first four bytes
	if (verify() == false) {
		cout << " Invalid file" << endl;
		return;
	}

	//print heading
	of << heading();

	//read data, create instruction, parse, and print
	int i =0;
	while(!in.eof()) {
		in.read((char*)buf, MAX);
		INSTRUCTION inst;
		parseInstruction(inst, i);
		i++;
	}
	in.close();
	cout << "Please look in \"result.txt\" for your disassembled code." << endl;
}

//makes sure this is a legal jvm file
bool BitManager::verify() {
	unsigned short foo[MAX-1];
	for (int i = 0; i < MAX-1; i++) {
		foo[i] = (buf[i]);
	}
	
	if( foo[0] == 0x12 &&
		foo[1] == 0x34 &&
		foo[2] == 0x56 &&
		foo[3] == 0x78 ) {
			isLegal = true;
	}

	return isLegal;
}

//top heading
string BitManager::heading() {
	stringstream os;
	os << "ADR  NXT J J J  S S F F E E I I  H O T C L S P M M  W R F  B  LABEL\n";
	os << "         M A A  L R 0 1 N N N N    P O P V P C D A  R E E  - \n";
	os << "         P M M  L A     A B V C    C S P       R R  I A T  B \n";
	os << "         C N Z  8 1         A                       T D C  U \n";
	os << "                                                    E   H  S \n";
	os << endl;
	return os.str();
}

//this, or overload << operator for INSTRUCTION
string BitManager::print(INSTRUCTION inst) {
	stringstream os;
	os.fill('0');
	os << hex << setw(3) << inst.address << "  " << setw(3) << inst.next << " ";
	os << convert(inst.jam,3) << " " << convert(inst.alu, 8) << " ";
	os << convert(inst.cbus,9) << " " << convert(inst.memory,3) << " ";
	os << hex << inst.bbus << "  " << inst.label << endl;
	return os.str();
}

//does the majority of the work
void BitManager::parseInstruction(INSTRUCTION inst, int i) {
	//temp ints
	int q, u;

	//address
	inst.address = i;

	//next
	q = buf[0];
	u = buf[1] & 0x70;
	inst.next = (q << 1) | u;

	//jam
	inst.jam = (buf[1] >> 4) & 0x7;

	//alu
	q = (buf[2] >> 4);
	u = buf[1] & 0xf;
	inst.alu =(u << 4) | q;

	//cbus
	q = (buf[3] >> 3);
	u = buf[2] & 0xf;
	inst.cbus = (u << 5) | q;

	//memory
	inst.memory = (buf[3] & 0x7);

	//bbus
	q = (buf[4] >> 4);
	inst.bbus = (q & 0xf);

	//label
	inst.label = getLabel(inst, i);

	//print
	of << print(inst);
	
	//checking if 8 rows have been printed
	nwln++;
	if (nwln == 8) {
		of << endl << endl;
		nwln = 0;
	}
};

//converts from int to binary
string BitManager::convert(unsigned int n, int l) {
    
	string newString= "0";
    for (int j = 256; j > 0; j >>= 1) {

		if( (n & j) == j) {
			newString.append("1");
		}
		else {
			newString.append("0");	
		}
    }

	string sp = space(newString, l);

	return sp;
}

//manages spacing as to adhere to output format
string BitManager::space(string n, int l) {
	int temp = n.length();
	int i = 0;

	string st = n.substr(temp-l,temp-1);
	
	string retstring;
	while (i < st.length()){
		retstring = retstring + st[i] + " ";
		i++;
	}

	return retstring;
}

//grabs the specific opcode label
string BitManager::getLabel(INSTRUCTION inst, int a) {
	stringstream os;

	//i iterates through opmap array, j through opcode array
	for (int i = 0; i < 27; i++) {
		int j = 0;
		while(opmap[i].opcode[j] != NULL) {

			if (opmap[i].opcode[j] == a) {
				os << opmap[i].label << j+1;
				return os.str();
			}
			j++;
		}
	}
	//0x00 = nop
	if (a == 0x00) {
		os << "nop1";
		return os.str();
	}

	//if nothing else returns, return error
	if (a <= 0x88) {
		counter++;
		os << "err" <<counter;
		return os.str();
	}
	return " ";
}
