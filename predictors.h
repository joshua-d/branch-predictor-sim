#pragma once
#include "predictor_interface.h"

class AlwaysTakenPredictor : public Predictor {
public:
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
};

class AlwaysNonTakenPredictor : public Predictor {
public:
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
};

class BimodalOneBitPredictor : public Predictor {
private:
	unsigned table_bits;
	unsigned table_size;
	bool* table;

public:
	BimodalOneBitPredictor(unsigned table_bits);
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
};

class BimodalSaturatingPredictor : public Predictor {
private:
	unsigned table_bits;
	unsigned table_size;
	unsigned* table;

public:
	BimodalSaturatingPredictor(unsigned table_bits);
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
};

class GSharePredictor : public Predictor {
private:
	unsigned history_bits;
	unsigned history_num;
	unsigned table_size = 2048;
	unsigned* table;
	unsigned ghr = 0;

public:
	GSharePredictor(unsigned history_bits);
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
};

class TournamentPredictor : public Predictor {
private:
	BimodalSaturatingPredictor* bsp;
	GSharePredictor* gsp;
	unsigned table_size = 2048;
	unsigned* table;
	unsigned gs_index;

public:
	TournamentPredictor();
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
};


class BTBPredictor : public Predictor {
private: 
	BimodalOneBitPredictor* bobp;
	unsigned table_size = 512;
	Address* table;
	unsigned n_predictions = 0;

public:
	BTBPredictor();
	unsigned get_index(Address addr);
	bool get_prediction(unsigned index);
	Address get_predicted_address(unsigned index);
	void update(unsigned index, bool prediction, bool actual);
	bool predict(Instruction instr);
	unsigned get_n_predictions();
};