#include "predictor_interface.h"

bool Predictor::predict(Instruction instr) {
	unsigned index = this->get_index(instr.addr);
	bool prediction = this->get_prediction(index);
	this->update(index, prediction, instr.outcome);
	return prediction;
}