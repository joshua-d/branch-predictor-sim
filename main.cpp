#include "predictors.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#define LOADED_INSTR_MAX 1000

using namespace std;

Instruction* load_next_instruction(ifstream& infile) {
	string line;
	if (!getline(infile, line))
		return nullptr;

	Address addr;
	Address target;
	bool outcome;
	string behavior;

	stringstream s(line);
	s >> std::hex >> addr >> behavior >> std::hex >> target;
	if (behavior == "T")
		outcome = true;
	else
		outcome = false;

	return new Instruction(addr, outcome, target);
}

int main(int argc, char** argv) {

	if (argc < 3) {
		cout << "usage: predictors <predictor> <input_trace_file> [bits]" << endl;
		return -1;
	}
	
	string predictor_type = argv[1];
	string input_filename = argv[2];
	int bits = 4;
	if (argc > 3) {
		bits = stoi(argv[3]);
	}
	

	Predictor* predictor;

	if (!predictor_type.compare("always_taken")) {
		predictor = new AlwaysTakenPredictor;
	}
	else if (!predictor_type.compare("always_non_taken")) {
		predictor = new AlwaysNonTakenPredictor;
	}
	else if (!predictor_type.compare("bimodal_one_bit")) {
		predictor = new BimodalOneBitPredictor(bits);
	}
	else if (!predictor_type.compare("bimodal_saturating")) {
		predictor = new BimodalSaturatingPredictor(bits);
	}
	else if (!predictor_type.compare("gshare")) {
		predictor = new GSharePredictor(bits);
	}
	else if (!predictor_type.compare("tournament")) {
		predictor = new TournamentPredictor;
	}
	else if (!predictor_type.compare("btb")) {
		predictor = new BTBPredictor;
	}
	else {
		cout << "Invalid predictor type.";
		return -1;
	}


	ifstream infile(input_filename);

	Instruction* loaded_instructions[LOADED_INSTR_MAX];
	unsigned n_loaded_instructions = 0;

	unsigned correct_predictions = 0;
	unsigned long total_predictions = 0;
	cout << "Executing trace..." << endl;
	while (true) {
		Instruction* instr = load_next_instruction(infile);

		if (instr == nullptr)
			break;

		loaded_instructions[n_loaded_instructions++] = instr;

		if (predictor->predict(*instr) == instr->outcome)
			correct_predictions++;
		total_predictions++;

		if (n_loaded_instructions == LOADED_INSTR_MAX) {
			for (unsigned i = 0; i < LOADED_INSTR_MAX; i++) {
				delete loaded_instructions[i];
			}
			n_loaded_instructions = 0;
		}
	}


	cout << "Correct predictions: " << correct_predictions << endl;
	cout << "Total predictions: " << total_predictions << endl;
	cout << "Prediction rate: ";
	printf("%.2f\%\n", (double)correct_predictions / total_predictions * 100);

	infile.close();
	for (unsigned i = 0; i < n_loaded_instructions; i++) {
		delete loaded_instructions[i];
	}
	delete predictor;

	return 0;
}