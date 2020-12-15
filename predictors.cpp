#include "predictors.h"
#include <math.h>

unsigned AlwaysTakenPredictor::get_index(Address addr) {
	return 0;
}

bool AlwaysTakenPredictor::get_prediction(unsigned index) {
	return true;
}

void AlwaysTakenPredictor::update(unsigned index, bool prediction, bool actual) {}


unsigned AlwaysNonTakenPredictor::get_index(Address addr) {
	return 0;
}

bool AlwaysNonTakenPredictor::get_prediction(unsigned index) {
	return false;
}

void AlwaysNonTakenPredictor::update(unsigned index, bool prediction, bool actual) {}



BimodalOneBitPredictor::BimodalOneBitPredictor(unsigned table_bits) {
	this->table_bits = table_bits;
	this->table_size = pow(2, table_bits);
	this->table = new bool[this->table_size];

	for (unsigned i = 0; i < this->table_size; i++) {
		this->table[i] = true;
	}
}

unsigned BimodalOneBitPredictor::get_index(Address addr) {
	return addr & (table_size - 1);
}

bool BimodalOneBitPredictor::get_prediction(unsigned index) {
	return this->table[index];
}

void BimodalOneBitPredictor::update(unsigned index, bool prediction, bool actual) {
	this->table[index] = actual;
}


BimodalSaturatingPredictor::BimodalSaturatingPredictor(unsigned table_bits) {
	this->table_bits = table_bits;
	this->table_size = pow(2, table_bits);
	this->table = new unsigned[this->table_size];

	for (unsigned i = 0; i < this->table_size; i++) {
		this->table[i] = 3;
	}
}

unsigned BimodalSaturatingPredictor::get_index(Address addr) {
	return addr & (table_size - 1);
}

bool BimodalSaturatingPredictor::get_prediction(unsigned index) {
	return this->table[index] > 1;
}

void BimodalSaturatingPredictor::update(unsigned index, bool prediction, bool actual) {
	if (!actual && this->table[index] != 0)
		this->table[index]--;
	else if (actual && this->table[index] != 3)
		this->table[index]++;
}


GSharePredictor::GSharePredictor(unsigned history_bits) {
	this->table = new unsigned[this->table_size];
	this->history_bits = history_bits;
	this->history_num = pow(2, history_bits) - 1;

	for (unsigned i = 0; i < this->table_size; i++) {
		this->table[i] = 3;
	}
}

unsigned GSharePredictor::get_index(Address addr) {
	return (addr ^ ghr) & (table_size - 1);
}

bool GSharePredictor::get_prediction(unsigned index) {
	return this->table[index] > 1;
}

void GSharePredictor::update(unsigned index, bool prediction, bool actual) {
	if (!actual && this->table[index] != 0)
		this->table[index]--;
	else if (actual && this->table[index] != 3)
		this->table[index]++;

	ghr = ((ghr << 1) + actual) & history_num;
}


TournamentPredictor::TournamentPredictor() {
	this->bsp = new BimodalSaturatingPredictor(11);
	this->gsp = new GSharePredictor(11);

	this->table = new unsigned[this->table_size];

	for (unsigned i = 0; i < this->table_size; i++) {
		this->table[i] = 0;
	}
}

unsigned TournamentPredictor::get_index(Address addr) {
	this->gs_index = this->gsp->get_index(addr);
	return this->bsp->get_index(addr);
}

bool TournamentPredictor::get_prediction(unsigned index) {
	if (this->table[index] < 2)
		return gsp->get_prediction(gs_index);
	else
		return bsp->get_prediction(index);
}

void TournamentPredictor::update(unsigned index, bool prediction, bool actual) {
	bool bsp_correct = bsp->get_prediction(index) == actual;
	bool gsp_correct = gsp->get_prediction(gs_index) == actual;

	if (bsp_correct && !gsp_correct && this->table[index] != 3)
		this->table[index]++;
	else if (!bsp_correct && gsp_correct && this->table[index] != 0)
		this->table[index]--;

	this->bsp->update(index, bsp->get_prediction(index), actual);
	this->gsp->update(gs_index, gsp->get_prediction(gs_index), actual);
}


BTBPredictor::BTBPredictor() {
	this->bobp = new BimodalOneBitPredictor(9);

	this->table = new Address[this->table_size];

	for (unsigned i = 0; i < this->table_size; i++) {
		this->table[i] = 0;
	}
}

unsigned BTBPredictor::get_index(Address addr) {
	return this->bobp->get_index(addr);
}

bool BTBPredictor::get_prediction(unsigned index) {
	return this->bobp->get_prediction(index);
}

Address BTBPredictor::get_predicted_address(unsigned index) {
	return this->table[index];
}

void BTBPredictor::update(unsigned index, bool prediction, bool actual) {
	this->bobp->update(index, prediction, actual);
}

bool BTBPredictor::predict(Instruction instr) {
	unsigned index = this->get_index(instr.addr);
	bool prediction = this->get_prediction(index);
	this->update(index, prediction, instr.outcome);
	
	if (prediction)
		this->n_predictions++;
	
	bool correct = prediction && this->get_predicted_address(index) == instr.target;

	if (instr.outcome)
		this->table[index] = instr.target;

	if (correct)
		return instr.outcome;

	return !instr.outcome;
}

unsigned BTBPredictor::get_n_predictions() {
	return this->n_predictions;
}