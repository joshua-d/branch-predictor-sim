#pragma once

typedef unsigned long long Address;

struct Instruction {
	Address addr;
	bool outcome;
	Address target;
	Instruction() {};
	Instruction(Address addr, bool outcome, Address target) : addr(addr), outcome(outcome), target(target) {};
};

class Predictor
{
public:
	virtual unsigned get_index(Address addr) = 0;
	virtual bool get_prediction(unsigned index) = 0;
	virtual void update(unsigned index, bool prediction, bool actual) = 0;

	virtual bool predict(Instruction instr);
};

